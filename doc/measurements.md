# Measurements

The design notes state each measured claim once and link here. Each section gives the method, the machine, the
compiler and its flags, and the issue comment that holds the full results and the code that produced them.

Unless a section says otherwise, the timings are [nanobench](https://github.com/martinus/nanobench) runs on one core
of a 2.8 GHz x86-64 Xeon, built with GCC 15 at `-O2`, and measure a call through `xstd::hash` or `xstd::digest`: the
copy of the seeded prototype, `hash_append` of the key, and the result.

## The prototype copy

Measured with nanobench on GCC 15 at `-O2`, on one core of a 2.8 GHz
Xeon, copying `xxhash_64`'s 80-byte prototype takes about 1 ns of a call that costs 11–18 ns on
x64 for a 64-bit integer or a 16-byte string; `llvm-mca` puts its five 16-byte loads and stores at
5 cycles. Starting each call from a default-constructed algorithm instead, or seeding one per call,
takes as long as the whole call does with the copy, so the fixed cost over `std::hash` is Hash2's
buffered `update` and its `result`, not the copy. `xxh3_128`'s 560-byte state is another matter:
its copy takes 19–23 ns of a 40–63 ns call on x64, and 17–28 ns of 55–105 on i686. Building that
state any other way costs as much or more, so the copy is still the cheapest start a seeded call
has.

The ablation and the `llvm-mca` runs are in
[#53](https://github.com/rhalbersma/xstd-misc/issues/53#issuecomment-6250956313).

## The cost of a call

- **FNV-1a against xxHash:** FNV-1a is faster by 3–14 ns on keys of up to 16 bytes, and four to eight times slower
  from 64 bytes. `xxhash_64`'s cost through `hash` is a fixed 15–19 ns a call for strings up to 64 bytes, 8.5 ns for a
  64-bit integer, and 37 ns at 256 bytes.
- **`xxhash_32` on x86-64:** it costs 1.3 to 2.9 times what `xxhash_64` does from 16 bytes up.
- **`xxh3_128` against `xxhash_64`:** through these classes, Hash2's `xxh3_128` costs 3.4 to 4.6 times what
  `xxhash_64` does per call on x86-64, 39 to 67 ns against 8.5 to 19 on keys up to 64 bytes, and 1.3 to 3 times on
  i686. Its state is 560 bytes against 80, and the prototype is copied on every call.

The method, the build and the per-length tables are in
[#53](https://github.com/rhalbersma/xstd-misc/issues/53#issuecomment-6250956313).

## On a 32-bit target

A 32-bit CPU emulates xxHash's 64-bit multiplies, and still pays no more for them than for `xxhash_32`'s: measured
through `hash` on i686 with GCC 15 at `-O2`, both cost 7.2 ns for a 64-bit integer, 16–19 ns for strings up to 16
bytes, 25 ns at 64 and 59 ns at 256.

## Avalanche

The tests measure the Strict Avalanche Criterion (Webster and Tavares, CRYPTO '85) through the
hasher's whole path: the seeded prototype, `hash_append` of a random 64-bit key, and the fold to
`std::size_t`, and they check the trait against it for every Hash2 algorithm. For every key bit
and hash bit, the rate at which flipping the one flips the other must lie within Hoeffding's
bound of ½, union-bounded over every pair at a false-failure probability of 10⁻⁶. The algorithms
a table runs are measured over 4096 keys, where the bound is about 0.053; the cryptographic ones
avalanche by design and cost far more per hash, so 256 keys check them coarsely, against a bound
of about 0.21. Measured over 4096 keys, every algorithm but FNV-1a stays near 0.03, the sampling
noise of an ideal hash, at either width of `std::size_t`; FNV-1a comes close to 0.5, some hash bit
almost never flipping with some key bit. In a `boost::unordered_flat_set` of 16384 sequential,
strided or common-prefix keys, xxHash, XXH3 and SipHash have the same probe lengths without the
mix as with it, while FNV-1a declared avalanching lengthens insertion probes by five to eight
percent. That second measurement is evidence rather than a test: probes in an
open-addressing table are not independent, so no threshold on it is calibrated. [SMHasher3](https://gitlab.com/fwojcik/smhasher3)'s
Avalanche test, run through `hash`'s whole path at both widths of `std::size_t` and at every key
length from 3 to 128 bytes, agrees with the trait for all 28 Hash2 algorithms. Its stricter BIC
test fails XXH3-128, and both Murmur3 variants mildly, on 3-byte keys only. SMHasher3's own
reference XXH3 fails it worse, so this is XXH3's short-input path rather than Hash2's or this
library's, and bit independence is no part of what the trait claims. The method and the
per-algorithm table are in [#53](https://github.com/rhalbersma/xstd-misc/issues/53#issuecomment-6250933037).

Measured over 24-byte keys, FNV-1a has input and output bit pairs that never flip together, where xxHash is at the
sampling noise.

## Hash flooding

The seed is what defeats an attacker who knows the hash function. An offline
search for keys that share a slot costs 2.7·10⁸ hashes for 32,768 keys in one group of a
`boost::unordered_flat_map`, whose average insertion probe then goes from 1.07 groups to 786,
and 7·10⁷ for 8,192 keys in one bucket of a `std::unordered_map` reserved to them. Insertion
runs 17–40 and 105–177 times slower, on libstdc++ and libc++, under unseeded xxHash, unseeded
FNV-1a and SipHash with a public seed alike. A secret seed per container leaves both tables as
they are with random keys, under SipHash and under xxHash. That search does not tell the two
apart: SipHash's half of the advice answers collisions found independently of the seed, as
Aumasson and Bernstein found for MurmurHash and CityHash in 2012, which this measurement does
not attempt. The method, its source and the full results are in
[#63](https://github.com/rhalbersma/xstd-misc/issues/63#issuecomment-6247746524).

## xxHash and SipHash in SMHasher3

[SMHasher3's results](https://gitlab.com/fwojcik/smhasher3/-/tree/main/results), measured on the
reference implementations, compare the two algorithms:

| | `xxhash_64` (XXH-64) | `siphash_64` (SipHash-2-4) |
| :-- | :-- | :-- |
| SMHasher3 | 241 of its 250 tests passed | all 235 of its tests passed |
| Cycles a call, keys of 1–32 bytes | 53 | 152 |
| Bytes a cycle, long keys | 3.99 | 0.32 |
| The seed | added to the state | the key of a keyed function |
| For | keys the program makes itself | keys an adversary chooses |

- **What XXH-64 fails:** all nine failures are seed tests, and it passes every test that hashes
  keys under one seed.
    - In [SeedBlockLen](https://gitlab.com/fwojcik/smhasher3/-/raw/main/results/raw/XXH-64.txt)
      at 15, 19, 21, 26, 29 and 30 bytes, and SeedBlockOffset at offset 8, two pairs of seed and key,
      each seed with at most two bits set, share all 64 bits. Every one of those keys is under 32
      bytes, where the seed enters once, as `seed + P5`, and a small change in it can cancel a small
      change in the key.
    - In SeedBIC, flipping seed bit 39 of a 4-byte key, or bit 35 of an 8-byte one, flips output
      bits 24 and 56 with a correlation of 0.82 and 0.94, where the worst pair at every other length
      stays within 0.006.
- **The cost:** on short keys, the length of most keys a table holds, SipHash takes about three
  times as long as xxHash. Hash2's `xxhash_64(std::uint64_t)` seeds as the reference XXH64 does,
  so its test results carry over.

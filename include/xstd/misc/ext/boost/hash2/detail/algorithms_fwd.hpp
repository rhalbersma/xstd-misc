//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef XSTD_MISC_EXT_BOOST_HASH2_DETAIL_ALGORITHMS_FWD_HPP
#define XSTD_MISC_EXT_BOOST_HASH2_DETAIL_ALGORITHMS_FWD_HPP

// Hash2's algorithms, declared exactly as Hash2 declares them, so that a trait can name each without including it.
namespace boost::hash2 {

class blake2b_512;
class blake2s_256;
class fnv1a_32;
class fnv1a_64;
class md5_128;
class murmur3_128;
class murmur3_32;
class ripemd_128;
class ripemd_160;
class sha1_160;
class sha2_224;
class sha2_256;
class sha2_384;
class sha2_512;
class sha2_512_224;
class sha2_512_256;
class sha3_224;
class sha3_256;
class sha3_384;
class sha3_512;
class shake_128;
class shake_256;
class siphash_32;
class siphash_64;
class spooky2_128;
class xxh3_128;
class xxhash_32;
class xxhash_64;

template<class H>
class hmac;

} // namespace boost::hash2

#endif // XSTD_MISC_EXT_BOOST_HASH2_DETAIL_ALGORITHMS_FWD_HPP

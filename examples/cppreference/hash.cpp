// Ported from the example on https://en.cppreference.com/w/cpp/utility/hash by cppreference.com contributors, at
// revision https://en.cppreference.com/mwiki/index.php?title=cpp/utility/hash&oldid=179088, and adapted to
// xstd::hash by Rein Halbersma, 2026: the key appends its members to a Boost.Hash2 algorithm that the hasher names
// and seeds, in place of a std::hash specialization that combines hashes; and the example prints whether hashes
// agree rather than their values, which differ between 32-bit and 64-bit targets, and looks keys up rather than
// listing the set, whose order differs between standard libraries.
// Distributed under the Creative Commons Attribution-ShareAlike 3.0 Unported License and the GNU Free Documentation
// License, as cppreference.com's content is: see LICENSE in this directory.

#include <xstd/misc/functional/hash.hpp> // hash
#include <boost/hash2/hash_append.hpp>   // hash_append_tag
#include <boost/hash2/siphash.hpp>       // siphash_64
#include <cstdint>                       // uint64_t
#include <iomanip>                       // quoted
#include <iostream>                      // boolalpha, cout
#include <string>                        // string
#include <string_view>                   // string_view
#include <unordered_set>                 // unordered_set

struct S
{
        std::string first_name;
        std::string last_name;

        [[nodiscard]] auto operator==(S const&) const -> bool = default;

        // A key says what it hashes, its members in order, and leaves the algorithm to the hasher.
        template<class Provider, class Hash, class Flavor>
        friend constexpr auto tag_invoke(boost::hash2::hash_append_tag const&, Provider const& provider, Hash& h, Flavor const& f, S const* s)
                -> void
        {
                provider.hash_append(h, f, s->first_name);
                provider.hash_append(h, f, s->last_name);
        }
};

// A hasher can name its algorithm: SipHash, seeded, where an adversary chooses the keys.
using siphasher = xstd::hash<S, boost::hash2::siphash_64>;

auto main() -> int
{
        auto const str      = std::string("Meet the new boss...");
        auto const str_hash = xstd::hash<std::string>()(str);
        std::cout << std::boolalpha << "hash(" << std::quoted(str) << ") == hash of its view: "
                  << (str_hash == xstd::hash<std::string>()(std::string_view(str))) << '\n';

        // A fixed seed, for the example; a program draws one at random, per container.
        auto const seed = std::uint64_t{0x9e37'79b9'7f4a'7c15};

        auto const obj  = S{"Hubert", "Farnsworth"};
        auto const copy = obj;
        std::cout << "hash(" << std::quoted(obj.first_name) << ", " << std::quoted(obj.last_name) << ") == hash of a copy: "
                  << (xstd::hash<S>()(obj) == xstd::hash<S>()(copy)) << " (using xxhash_64) and "
                  << (siphasher(seed)(obj) == siphasher(seed)(copy)) << " (using a seeded siphash_64)\n";

        // A key that appends its members can be used in unordered containers, under whichever algorithm they name.
        auto const names = std::unordered_set<S, siphasher>({obj, {"Bender", "Rodriguez"}, {"Turanga", "Leela"}}, 0, siphasher(seed));
        for (auto const& s : {S{"Bender", "Rodriguez"}, S{"Turanga", "Leela"}, obj, S{"Philip", "Fry"}}) {
                std::cout << std::quoted(s.first_name) << ' ' << std::quoted(s.last_name) << " is in the set: " << names.contains(s) << '\n';
        }
}

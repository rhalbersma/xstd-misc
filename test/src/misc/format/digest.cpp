//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/format/digest.hpp>     // formatter<boost::hash2::digest<N>>
#include <xstd/misc/functional/digest.hpp> // digest
#include <boost/hash2/digest.hpp>          // digest, to_string
#include <boost/hash2/sha2.hpp>            // sha2_256
#include <boost/test/unit_test.hpp>        // BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_CHECK_THROW
#include <algorithm>                       // copy
#include <array>                           // array
#include <cstddef>                         // size_t
#include <format>                          // format, format_error, format_parse_context, formatter, make_format_args, vformat
#include <string>                          // string

namespace {

template<std::size_t N>
auto digest_of(std::array<unsigned char, N> const& bytes)
        -> boost::hash2::digest<N>
{
        auto d = boost::hash2::digest<N>();
        std::ranges::copy(bytes, d.begin());
        return d;
}

constexpr auto four_bytes = std::array<unsigned char, 4>{0x00, 0x01, 0xab, 0xff};

} // namespace

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(Format)
BOOST_AUTO_TEST_SUITE(Digest)

// A leading zero stays, so a printed digest reads back as the bytes stored, as boost::hash2::to_string prints them.
BOOST_AUTO_TEST_CASE(PrintsLowercaseHexInByteOrder)
{
        auto const d = digest_of(four_bytes);

        BOOST_CHECK_EQUAL(std::format("{}", d), "0001abff");
        BOOST_CHECK_EQUAL(std::format("{:x}", d), boost::hash2::to_string(d));
}

BOOST_AUTO_TEST_CASE(PrintsUppercaseHexWithX)
{
        BOOST_CHECK_EQUAL(std::format("{:X}", digest_of(four_bytes)), "0001ABFF");
}

// Two hex digits per byte, at the width of whichever algorithm produced it.
BOOST_AUTO_TEST_CASE(PrintsEveryByte)
{
        auto const d = xstd::digest<std::string, boost::hash2::sha2_256>()("abc");

        BOOST_CHECK_EQUAL(std::format("{}", d).size(), 2 * d.size());
        BOOST_CHECK_EQUAL(std::format("{}", d), boost::hash2::to_string(d));
}

// A decimal or width specification is the habit from integers, and a digest is no integer.
BOOST_AUTO_TEST_CASE(RejectsAnyOtherSpecification)
{
        auto const d = digest_of(four_bytes);

        BOOST_CHECK_THROW(static_cast<void>(std::vformat("{:d}", std::make_format_args(d))), std::format_error);
        BOOST_CHECK_THROW(static_cast<void>(std::vformat("{:08x}", std::make_format_args(d))), std::format_error);
}

// A formatter nested in another's is handed a specification that ends where its parse context does.
BOOST_AUTO_TEST_CASE(ParsesASpecificationThatEndsItsContext)
{
        auto formatter = std::formatter<boost::hash2::digest<4>>();
        auto empty     = std::format_parse_context("");
        auto upper     = std::format_parse_context("X");

        BOOST_CHECK(formatter.parse(empty) == empty.end());
        BOOST_CHECK(formatter.parse(upper) == upper.end());
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

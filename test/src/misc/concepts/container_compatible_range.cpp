//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <xstd/misc/concepts/container_compatible_range.hpp> // container_compatible_range
#include <boost/test/unit_test.hpp>                          // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_AUTO_TEST_CASE, BOOST_CHECK
#include <cstddef>                                           // size_t
#include <forward_list>                                      // forward_list
#include <string>                                            // string
#include <vector>                                            // vector

BOOST_AUTO_TEST_SUITE(Misc)
BOOST_AUTO_TEST_SUITE(Concepts)
BOOST_AUTO_TEST_SUITE(ContainerCompatibleRange)

// An input range whose references convert to the element type, and nothing else, as the from_range constructors take.
BOOST_AUTO_TEST_CASE(AnInputRangeOfConvertibleReferencesModelsIt)
{
        static_assert(xstd::container_compatible_range<std::vector<int>, std::size_t>);
        static_assert(xstd::container_compatible_range<std::forward_list<bool> const&, int>);
        static_assert(not xstd::container_compatible_range<std::vector<std::string>, int>);
        static_assert(not xstd::container_compatible_range<int, int>);
        BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

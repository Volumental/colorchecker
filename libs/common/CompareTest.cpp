#define BOOST_TEST_DYN_LINK

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <boost/test/unit_test.hpp>

#include "algorithm/Range.hpp"
#include "Compare.hpp"
#include "Logging.hpp"
#include "Random.hpp"

BOOST_AUTO_TEST_SUITE(komb)
BOOST_AUTO_TEST_SUITE(common)

BOOST_AUTO_TEST_CASE(numberAwareStrcmpSort)
{
    const std::vector<std::string> kSorted{
        "bar_001.png",
        "bar_002.png",
        "bar_010.png",
        "bar_011.png",
        "bar_011.png",
        "bar_014.png",
        "bar_020.png",
        "bar_021.png",
        "bar_100.png",
        "bar_200.png",
        "foo_0.png",
        "foo_1.png",
        "foo_1.png",
        "foo_2.png",
        "foo_10.png",
        "foo_11.png",
        "foo_21.png",
        "foo_100.png",
        "foo_101.png",
        "foo_1000.png",
        "foo_1001.png",
        "foo_010000.png",
        "foo_10010.png",
    };

    for (auto seed : irange<std::uint_fast64_t>(10))
    {
        std::vector<std::string> test_strings = kSorted;
        auto rng = getRandomEngine(seed);
        std::shuffle(test_strings.begin(), test_strings.end(), rng);
        BOOST_CHECK(test_strings != kSorted);

        std::sort(test_strings.begin(), test_strings.end(), numberAwareStringLess);

        if (test_strings != kSorted)
        {
            std::cout << "Expected:\n    " << kSorted << "\nGot:\n    "
                << test_strings << std::endl;
        }
        BOOST_CHECK(test_strings == kSorted);
    }
}

BOOST_AUTO_TEST_CASE(numberAwareStrcmpLess)
{
    BOOST_CHECK(numberAwareStrcmp("",     "")      == 0);
    BOOST_CHECK(numberAwareStrcmp("",     "a")     <  0);
    BOOST_CHECK(numberAwareStrcmp("a",    "a")     == 0);
    BOOST_CHECK(numberAwareStrcmp("a001", "a2")    <  0);
    BOOST_CHECK(numberAwareStrcmp("a001", "a02")   <  0);
    BOOST_CHECK(numberAwareStrcmp("a001", "a002")  <  0);
    BOOST_CHECK(numberAwareStrcmp("a001", "a0002") <  0);
    BOOST_CHECK(numberAwareStrcmp("a001", "a001")  == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

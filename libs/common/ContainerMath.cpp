#include "ContainerMath.hpp"
#define BOOST_TEST_DYN_LINK

#include <vector>

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include "algorithm/Range.hpp"

static float kPercentTolerance = 1E-5f;

BOOST_AUTO_TEST_SUITE(komb)
BOOST_AUTO_TEST_SUITE(common)

BOOST_AUTO_TEST_CASE(math_linspace)
{
    const auto vec = linspace(0.1f, 16.f, 4u);
    const auto correct = std::vector<float>{0.1f, 5.4f, 10.7f, 16.f};

    BOOST_REQUIRE_EQUAL(vec.size(), correct.size());
    for (auto i : indices(vec))
    {
        BOOST_CHECK_CLOSE(vec[i], correct[i], kPercentTolerance);
    }
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

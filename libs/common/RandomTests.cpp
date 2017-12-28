#define BOOST_TEST_DYN_LINK

#include <array>

#include <boost/test/unit_test.hpp>

#include "algorithm/Container.hpp"
#include "algorithm/Range.hpp"
#include "Random.hpp"

BOOST_AUTO_TEST_SUITE(komb)
BOOST_AUTO_TEST_SUITE(common)

// We expect this to succeed on all platforms.
BOOST_AUTO_TEST_CASE(random_engine_test)
{
    // Arrange.
    using SampleType       = std::uint_fast64_t;
    const auto num_samples = size_t{10};
    auto random_engine     = getRandomEngine();
    auto dice              = [&](){return random_engine();};
    auto samples           = std::array<SampleType, num_samples>{};

    // Act.
    generate(samples, dice);

    // Assert.
    std::array<SampleType, num_samples> expected = {
        2947667278772165694u,
        18301848765998365067u,
        729919693006235833u,
        11021831128136023278u,
        10003392056472839596u,
        1054412044467431918u,
        11649642299870863663u,
        7813497161378842344u,
        15536964167022953318u,
        16718309832681015833u};
    for (const auto i : irange(num_samples))
    {
        BOOST_REQUIRE_EQUAL(expected[i], samples[i]);
    }
}

// Workaround for boost test unit filtering which is implemented in boost 1.59.
// Ubuntu 16.04 has boost 1.58 and cannot deal with two parameters to BOOST_AUTO_TEST_CASE.
// Macports has boost 1.59 and can therefore disable the test.
#ifndef __linux__
#  define ONLY_ON_LINUX   , * boost::unit_test::disabled()
#else
#  define ONLY_ON_LINUX
#endif

// We expect this to succeed on all Xubuntu machines.
BOOST_AUTO_TEST_CASE(uniform_int_distribution_test ONLY_ON_LINUX)
{
    // Arrange.
    using SampleType       = size_t;
    const auto minimum     = SampleType{0};
    const auto maximum     = SampleType{10};
    const auto num_samples = size_t{10};
    auto random_engine     = getRandomEngine();
    auto distribution      = std::uniform_int_distribution<SampleType>(minimum, maximum);
    auto dice              = [&](){return distribution(random_engine);};
    auto samples           = std::array<SampleType, num_samples>{};

    // Act.
    generate(samples, dice);

    // Assert.
    std::array<SampleType, num_samples> expected = {1u, 10u, 0u, 6u, 5u, 0u, 6u, 4u, 9u, 9u};
    for (const auto i : irange(num_samples))
    {
        BOOST_REQUIRE_EQUAL(expected[i], samples[i]);
    }
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

#define BOOST_TEST_DYN_LINK
#include <vector>

#include <boost/test/unit_test.hpp>

#include <geometry_toolbox/Angle.hpp>

BOOST_AUTO_TEST_SUITE(komb)
BOOST_AUTO_TEST_SUITE(geometry_toolbox)

static const double kTestFloatPercentTolerance = 1E-3;

BOOST_AUTO_TEST_CASE(AngleWrapTest)
{
    std::vector<float> test_angles{-3 * kPi_f, -kPi_f, -kPi_f / 2, kPi_f / 2, 3 * kPi_f};
    std::vector<float> wrapped_angles_zero_tau{kPi_f, kPi_f, 3 * kPi_f / 2, kPi_f / 2, kPi_f};
    std::vector<float> wrapped_angles_mpi_pi{-kPi_f, -kPi_f, -kPi_f / 2, kPi_f / 2, kPi_f};

    for (size_t i = 0; i < test_angles.size(); ++i)
    {
        auto& test_angle = test_angles[i];
        auto& wrapped_angle_zero_tau = wrapped_angles_zero_tau[i];
        auto& wrapped_angle_mpi_pi = wrapped_angles_mpi_pi[i];
        BOOST_CHECK_CLOSE(wrapped_angle_zero_tau, wrapZeroToTwoPi(test_angle),
            kTestFloatPercentTolerance);
        BOOST_CHECK_CLOSE(wrapped_angle_mpi_pi, wrapMinusPiToPi(test_angle),
            kTestFloatPercentTolerance);
    }
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

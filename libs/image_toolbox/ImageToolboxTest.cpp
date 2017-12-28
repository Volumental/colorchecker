#define BOOST_TEST_DYN_LINK

#include <cmath>

#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/unit_test.hpp>
#include <opencv2/opencv.hpp>

#include <image_toolbox/Gamma.hpp>

static float kIsSmallTolerance = 0.00001f;
static double kPercentTolerance = 0.00001;
static float kDecodedHalf = 0.215860531;
static float kEncodedHalf = 188.f;

BOOST_AUTO_TEST_SUITE(komb)
BOOST_AUTO_TEST_SUITE(image_toolbox)

BOOST_AUTO_TEST_CASE(GammaTestVec3f)
{
    cv::Mat3f img(20, 20, cv::Vec3f(0.0f, 0.5f, 1.0f));
    cv::Mat3f img_decoded = komb::linearFromByte3(img * 255.f);
    cv::Mat3f img_encoded = komb::byteFromLinear3(img);
    BOOST_REQUIRE_EQUAL(img.size(), img_decoded.size());
    BOOST_REQUIRE_EQUAL(img.size(), img_encoded.size());
    BOOST_REQUIRE_EQUAL(img.channels(), img_decoded.channels());
    BOOST_REQUIRE_EQUAL(img.channels(), img_encoded.channels());
    BOOST_CHECK_SMALL(img_decoded.at<cv::Vec3f>(0, 0)[0], kIsSmallTolerance);
    BOOST_CHECK_CLOSE(img_decoded.at<cv::Vec3f>(0, 0)[1], kDecodedHalf, kPercentTolerance);
    BOOST_CHECK_CLOSE(img_decoded.at<cv::Vec3f>(0, 0)[2], 1.0f, kPercentTolerance);
    BOOST_CHECK_SMALL(img_encoded.at<cv::Vec3f>(0, 0)[0], kIsSmallTolerance);
    BOOST_CHECK_CLOSE(img_encoded.at<cv::Vec3f>(0, 0)[1], kEncodedHalf, kPercentTolerance);
    BOOST_CHECK_CLOSE(img_encoded.at<cv::Vec3f>(0, 0)[2], 255.0f, kPercentTolerance);
}

BOOST_AUTO_TEST_CASE(GammaTestFloat)
{
    cv::Mat1f img(20, 20, 0.5f);
    cv::Mat1f img_decoded = komb::linearFromByte1(img * 255.f);
    cv::Mat1b img_encoded = komb::byteFromLinear1(img);
    BOOST_REQUIRE_EQUAL(img.size(), img_decoded.size());
    BOOST_REQUIRE_EQUAL(img.size(), img_encoded.size());
    BOOST_REQUIRE_EQUAL(img.channels(), img_decoded.channels());
    BOOST_REQUIRE_EQUAL(img.channels(), img_encoded.channels());
    BOOST_CHECK_CLOSE(img_decoded(0, 0), kDecodedHalf, kPercentTolerance);
    BOOST_CHECK_CLOSE(img_encoded(0, 0), kEncodedHalf, kPercentTolerance);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

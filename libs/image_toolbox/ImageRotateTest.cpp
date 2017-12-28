#define BOOST_TEST_DYN_LINK
#include <vector>

#include <boost/test/unit_test.hpp>

#include <geometry_toolbox/Angle.hpp>
#include <image_toolbox/ImageIo.hpp>
#include <image_toolbox/Tests.hpp>
#include <image_toolbox/Visualization.hpp>

static const fs::path kTestFilesFolder = "test_files/image_toolbox";

BOOST_AUTO_TEST_SUITE(komb)
BOOST_AUTO_TEST_SUITE(image_toolbox)

BOOST_AUTO_TEST_CASE(rotateTest)
{
    const cv::Mat rot_0   = readCvImage(kTestFilesFolder / "rot_0.png");
    const cv::Mat rot_90  = readCvImage(kTestFilesFolder / "rot_90.png");
    const cv::Mat rot_180 = readCvImage(kTestFilesFolder / "rot_180.png");
    const cv::Mat rot_270 = readCvImage(kTestFilesFolder / "rot_270.png");

    BOOST_REQUIRE(!rot_0.empty());
    BOOST_REQUIRE(!rot_90.empty());
    BOOST_REQUIRE(!rot_180.empty());
    BOOST_REQUIRE(!rot_270.empty());

    // Positive test cases
    BOOST_CHECK(areEqual(rot_0,   komb::rotateCcw(rot_0.clone(), radiansFromDegrees(0.f))));
    BOOST_CHECK(areEqual(rot_90,  komb::rotateCcw(rot_0.clone(), radiansFromDegrees(90.f))));
    BOOST_CHECK(areEqual(rot_180, komb::rotateCcw(rot_0.clone(), radiansFromDegrees(180.f))));
    BOOST_CHECK(areEqual(rot_270, komb::rotateCcw(rot_0.clone(), radiansFromDegrees(270.f))));
    BOOST_CHECK(areEqual(rot_270, komb::rotateCcw(rot_0.clone(), radiansFromDegrees(-90.f))));
    BOOST_CHECK(areEqual(rot_90,  komb::rotateCcw(rot_0.clone(), radiansFromDegrees(90.f - 360.f * 2.f)))); // NOLINT
    BOOST_CHECK(areEqual(rot_90,  komb::rotateCcw(rot_0.clone(), radiansFromDegrees(90.f + 360.f * 2.f)))); // NOLINT

    // Negative test cases
    BOOST_CHECK(!areEqual(rot_0, komb::rotateCcw(rot_0.clone(), radiansFromDegrees(90.0f))));
}

BOOST_AUTO_TEST_CASE(rotate90Test)
{
    const cv::Mat rot_0   = readCvImage(kTestFilesFolder / "rot_0.png");
    const cv::Mat rot_90  = readCvImage(kTestFilesFolder / "rot_90.png");
    const cv::Mat rot_180 = readCvImage(kTestFilesFolder / "rot_180.png");
    const cv::Mat rot_270 = readCvImage(kTestFilesFolder / "rot_270.png");

    BOOST_REQUIRE(!rot_0.empty());
    BOOST_REQUIRE(!rot_90.empty());
    BOOST_REQUIRE(!rot_180.empty());
    BOOST_REQUIRE(!rot_270.empty());

    // Write to disk for debugging
    writeCvImage(kTestFilesFolder / "TEST_OUT_rot_0.png",   komb::rotateCcw90(rot_0.clone(), 0));
    writeCvImage(kTestFilesFolder / "TEST_OUT_rot_90.png",  komb::rotateCcw90(rot_0.clone(), 90));
    writeCvImage(kTestFilesFolder / "TEST_OUT_rot_180.png", komb::rotateCcw90(rot_0.clone(), 180));
    writeCvImage(kTestFilesFolder / "TEST_OUT_rot_270.png", komb::rotateCcw90(rot_0.clone(), 270));

    // Positive test cases
    BOOST_CHECK(areEqual(rot_0,   komb::rotateCcw90(rot_0.clone(), 0)));
    BOOST_CHECK(areEqual(rot_90,  komb::rotateCcw90(rot_0.clone(), 90)));
    BOOST_CHECK(areEqual(rot_180, komb::rotateCcw90(rot_0.clone(), 180)));
    BOOST_CHECK(areEqual(rot_270, komb::rotateCcw90(rot_0.clone(), 270)));
    BOOST_CHECK(areEqual(rot_270, komb::rotateCcw90(rot_0.clone(), -90)));
    BOOST_CHECK(areEqual(rot_90,  komb::rotateCcw90(rot_0.clone(), 90 - 360 * 2)));
    BOOST_CHECK(areEqual(rot_90,  komb::rotateCcw90(rot_0.clone(), 90 + 360 * 2)));

    // Negative test cases
    BOOST_CHECK(!areEqual(rot_0, komb::rotateCcw90(rot_0.clone(), 90)));
    BOOST_CHECK_MESSAGE(!areEqual(rot_90, komb::rotateCcw90(rot_0.clone(), -90)),
        "Rotation direction is reversed.");
    BOOST_CHECK_MESSAGE(!areEqual(rot_270, komb::rotateCcw90(rot_0.clone(), 90)),
        "Rotation direction is reversed.");
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

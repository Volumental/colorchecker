#include <gflags/gflags.h>
#include <opencv2/opencv.hpp>

#include <color_calibration/ColorCalibration.hpp>
#include <common/Logging.hpp>
#include <common/LoggingInit.hpp>
#include <image_toolbox/ImageIo.hpp>

using namespace komb;

DEFINE_string(cam_image, "resources/ColorChecker_sRGB_from_Lab_D50.png",
    "Path to camera image with a colorchecker that should be calibrated to reference colors.");
DEFINE_string(ref_image, "resources/ColorChecker_sRGB_from_Lab_D50_AfterNov2014.png",
    "Path to image with colorchecker reference colors.");

void imshow(const cv::String& win_name, cv::Mat image, double scale)
{
    cv::Mat scaled_image;
    cv::resize(image, scaled_image, cv::Size(0, 0), scale, scale, cv::INTER_NEAREST);
    cv::imshow(win_name, scaled_image);
}

int main(int argc, char* argv[])
{
    google::SetUsageMessage(R"(
Debugging tool for color correction with colorchecker calibration target.
)");
    komb::initLogging(argc, argv);

    cv::Mat3b reference_image = readCvImageOrDie(FLAGS_ref_image, cv::IMREAD_COLOR);
    cv::Mat3b camera_image    = readCvImageOrDie(FLAGS_cam_image, cv::IMREAD_COLOR);
    double scale = 500.0 / camera_image.cols;
    cv::resize(camera_image, camera_image, cv::Size(0, 0), scale, scale, cv::INTER_AREA);
    cv::blur(camera_image, camera_image, cv::Size(11, 11));

    cv::Mat3b camera_canvas = camera_image.clone();
    cv::Mat3b camera_checker = findColorChecker(camera_image, camera_canvas);

    cv::Mat3b reference_canvas = reference_image.clone();
    cv::Mat3b reference_checker = findColorChecker(reference_image, reference_canvas);

    cv::imshow("Checker in camera image", camera_canvas);
    cv::imshow("Checker in reference image", reference_canvas);
    cv::imshow("camera checker", bigChecker(camera_checker));
    cv::imshow("reference checker", bigChecker(reference_checker));

    if (camera_checker.empty() || reference_checker.empty())
    {
        LOG_IF(ERROR, camera_checker.empty()) << "findColorChecker failed for camera image.";
        LOG_IF(ERROR, reference_checker.empty()) << "findColorChecker failed for reference image.";
    }
    else
    {
        cv::Matx34f color_transformation =
            findColorTransformation(camera_checker, reference_checker);

        cv::Mat3b adjusted_checker = camera_checker.clone();
        applyColorTransformation(adjusted_checker, color_transformation);
        cv::imshow("adjusted checker", bigChecker(adjusted_checker));

        cv::Mat3b error_checker = cv::Scalar(127, 127, 127) +
            cv::Mat3f(adjusted_checker) - cv::Mat3f(reference_checker);
        cv::imshow("error checker", bigChecker(error_checker));

        const auto num_values = static_cast<double>(reference_checker.total()) * 3.0;
        const auto sqrt_num_values = std::sqrt(num_values);
        LOG(INFO) << "Original checker average L1 error norm: "
                  << cv::norm(camera_checker, reference_checker, cv::NORM_L1) / num_values;
        LOG(INFO) << "Original checker average L2 error norm: "
                  << cv::norm(camera_checker, reference_checker, cv::NORM_L2) / sqrt_num_values;
        LOG(INFO) << "Original checker median absolute deviation: "
                  << medianAbsoluteDeviation(camera_checker, reference_checker);
        LOG(INFO) << "Adjusted checker average L1 error norm: "
            << cv::norm(adjusted_checker, reference_checker, cv::NORM_L1) / num_values;
        LOG(INFO) << "Adjusted checker average L2 error norm: "
            << cv::norm(adjusted_checker, reference_checker, cv::NORM_L2) / sqrt_num_values;
        LOG(INFO) << "Adjusted checker median absolute deviation: "
                  << medianAbsoluteDeviation(adjusted_checker, reference_checker);

        cv::Mat3b adjusted_image = camera_image.clone();
        applyColorTransformation(adjusted_image, color_transformation);
        cv::imshow("adjusted_colors", adjusted_image);
    }

    while ((cv::waitKey(0) & 255) != 27)
    {
    }
    return 0;
}

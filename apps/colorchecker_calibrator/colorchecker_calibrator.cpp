#include <gflags/gflags.h>
#include <opencv2/opencv.hpp>

#include <color_calibration/ColorCalibration.hpp>
#include <common/Logging.hpp>
#include <common/LoggingInit.hpp>
#include <image_toolbox/ImageIo.hpp>

using namespace komb;

DEFINE_string(sample, "resources/ColorChecker_sRGB_from_Lab_D50.png",
    "Path to camera image with a colorchecker that should be calibrated to reference colors.");
DEFINE_string(reference, "resources/ColorChecker_sRGB_from_Lab_D50_AfterNov2014.png",
    "Path to image with colorchecker reference colors.");
DEFINE_string(image, "",
    "Optional path to image to adjust.");

cv::Mat3b preprocess(const cv::Mat3b& image)
{
    const double scale = 500.0 / std::max(image.cols, image.rows);
    cv::Mat3b preprocessed_image;
    cv::resize(image, preprocessed_image, cv::Size(0, 0), scale, scale, cv::INTER_AREA);
    cv::blur(preprocessed_image, preprocessed_image, cv::Size(11, 11));
    return preprocessed_image;
}

int main(int argc, char* argv[])
{
    google::SetUsageMessage(R"(
Debugging tool for color correction with colorchecker calibration target.
)");
    komb::initLogging(argc, argv);

    cv::Mat3b sample_image    = readCvImageOrDie(FLAGS_sample,    cv::IMREAD_COLOR);
    cv::Mat3b reference_image = readCvImageOrDie(FLAGS_reference, cv::IMREAD_COLOR);
    cv::Mat3b camera_image    = readCvImage(FLAGS_image,          cv::IMREAD_COLOR);

    cv::Mat3b preprocessed_sample_image    = preprocess(sample_image);
    cv::Mat3b preprocessed_reference_image = preprocess(reference_image);

    cv::Mat3b sample_canvas = preprocessed_sample_image.clone();
    cv::Mat3b sample_checker = findColorChecker(preprocessed_sample_image, sample_canvas);

    cv::Mat3b reference_canvas = preprocessed_reference_image.clone();
    cv::Mat3b reference_checker = findColorChecker(preprocessed_reference_image, reference_canvas);

    cv::imshow("Checker in sample image", sample_canvas);
    cv::imshow("Checker in reference image", reference_canvas);
    cv::imshow("sample checker", bigChecker(sample_checker));
    cv::imshow("reference checker", bigChecker(reference_checker));

    if (sample_checker.empty() || reference_checker.empty())
    {
        LOG_IF(ERROR, sample_checker.empty()) << "findColorChecker failed for sample image.";
        LOG_IF(ERROR, reference_checker.empty()) << "findColorChecker failed for reference image.";
    }
    else
    {
        cv::Matx34f color_transformation =
            findColorTransformation(sample_checker, reference_checker);

        cv::Mat3b adjusted_checker = sample_checker.clone();
        applyColorTransformation(adjusted_checker, color_transformation);
        cv::imshow("adjusted checker", bigChecker(adjusted_checker));

        cv::Mat3b error_checker = cv::Scalar(127, 127, 127) +
            cv::Mat3f(adjusted_checker) - cv::Mat3f(reference_checker);
        cv::imshow("error checker", bigChecker(error_checker));

        const auto num_values = static_cast<double>(reference_checker.total()) * 3.0;
        const auto sqrt_num_values = std::sqrt(num_values);
        LOG(INFO) << "Original checker sample average L1 error norm: "
                  << cv::norm(sample_checker, reference_checker, cv::NORM_L1) / num_values;
        LOG(INFO) << "Original checker sample average L2 error norm: "
                  << cv::norm(sample_checker, reference_checker, cv::NORM_L2) / sqrt_num_values;
        LOG(INFO) << "Original checker sample median absolute deviation: "
                  << medianAbsoluteDeviation(sample_checker, reference_checker);
        LOG(INFO) << "Adjusted checker sample average L1 error norm: "
            << cv::norm(adjusted_checker, reference_checker, cv::NORM_L1) / num_values;
        LOG(INFO) << "Adjusted checker sample average L2 error norm: "
            << cv::norm(adjusted_checker, reference_checker, cv::NORM_L2) / sqrt_num_values;
        LOG(INFO) << "Adjusted checker sample median absolute deviation: "
                  << medianAbsoluteDeviation(adjusted_checker, reference_checker);

        if (!camera_image.empty())
        {
            cv::imshow("Original image", camera_image);
            cv::Mat3b adjusted_image = camera_image.clone();
            applyColorTransformation(adjusted_image, color_transformation);
            cv::imshow("Adjusted image", adjusted_image);
        }
    }

    while ((cv::waitKey(0) & 255) != 27)
    {
    }
    return 0;
}

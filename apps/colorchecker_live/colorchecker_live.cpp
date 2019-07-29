#include <boost/filesystem.hpp>
#include <gflags/gflags.h>
#include <opencv2/opencv.hpp>

#include <color_calibration/ColorCalibration.hpp>
#include <common/Logging.hpp>
#include <common/LoggingInit.hpp>
#include <common/Path.hpp>
#include <file_io_toolbox/FileSystem.hpp>
#include <image_toolbox/ImageIo.hpp>

using namespace komb;

DEFINE_uint32(camera_index, 0, "Index of camera to use");

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

    cv::VideoCapture cap(FLAGS_camera_index); // Open the default camera.
    CHECK_S(cap.isOpened()) << "Failed to open camera " << FLAGS_camera_index;

    cv::Mat3b last_successful_checker;
    cv::Mat3b frame;
    for(;;)
    {
        cap >> frame; // Get a new frame from camera.
        const cv::Mat3b preprocessed_sample_image = preprocess(frame);
        cv::Mat3b sample_canvas = preprocessed_sample_image.clone();
        const cv::Mat3b sample_checker = findColorChecker(preprocessed_sample_image, sample_canvas);

        cv::imshow("Camera frame", frame);
        cv::imshow("Checker in sample image", sample_canvas);
        cv::imshow("sample checker", bigChecker(sample_checker));
        cv::imshow("last checker", bigChecker(last_successful_checker));

        if (!sample_checker.empty())
        {
            if (!last_successful_checker.empty())
            {
                const double l1_norm = cv::norm(sample_checker, last_successful_checker, cv::NORM_L1);
                if (l1_norm >= 1000)
                {
                    const fs::path file_path = fs::path("frames") / fs::unique_path("%%%%-%%%%-%%%%-%%%%.png");
                    LOG(INFO) << "L1 error norm compared to last sample: " << l1_norm
                        << " (writing frame to " << file_path << ")";
                    writeCvImage(file_path, frame);
                }
                else
                {
                    LOG(INFO) << "L1 error norm compared to last sample: " << l1_norm;
                }
            }
            last_successful_checker = sample_checker;
        }

        if ((cv::waitKey(25) & 255) == 27)
        {
            break;
        }
    }

    return 0;
}

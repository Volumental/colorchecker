#include "ImageIo.hpp"

#include <limits>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <opencv2/opencv.hpp>

#include <common/algorithm/Range.hpp>
#include <common/algorithm/Vector.hpp>
#include <common/Json.hpp>
#include <common/String.hpp>
#include <common/Units.hpp>
#include <file_io_toolbox/FileIo.hpp>
#include <file_io_toolbox/FileSystem.hpp>

#include "Gamma.hpp"
#include "Image.hpp"
#include "OpenCvTools.hpp"

namespace komb {

static const std::vector<std::string> kImageExtensions = {".bmp", ".jpeg", ".jpg", ".png", ".tga"};

bool isImageExtension(std::string extension)
{
    boost::algorithm::to_lower(extension);
    return contains(kImageExtensions, extension);
}

cv::Mat_<uint16_t> readRawDepthPng(const fs::path& file_path)
{
    return readCvImage(file_path, cv::IMREAD_UNCHANGED);
}

cv::Mat readCvImage(const fs::path& path, cv::ImreadModes mode)
{
    cv::Mat result = cv::imread(path.string(), mode);
    if (result.empty())
    {
        LOG(ERROR) << "Failed to load image at " << path;
    }
    return result;
}

cv::Mat readCvImageOrDie(const fs::path& path, cv::ImreadModes mode)
{
    CHECK(fs::exists(path)) << "Missing image: " << path;
    CHECK(fs::is_regular_file(path)) << "Not a file: " << path;
    cv::Mat result = cv::imread(path.string(), mode);
    CHECK(!result.empty()) << "Failed to load image at " << path;
    return result;
}

cv::Mat3b readCvImageBgrOrDie(const fs::path& path)
{
    ERROR_CONTEXT("Image path", path.c_str());
    const cv::Mat mat = readCvImage(path, cv::IMREAD_COLOR);
    CHECK_EQ_F(mat.depth(), CV_8U);
    CHECK_EQ_F(mat.channels(), 3);
    return mat;
}

bool writeCvImage(const fs::path& path, const cv::Mat& image,
             const std::vector<int>& params)
{
    ERROR_CONTEXT("Image path", path.c_str());
    VLOG(3) << "Saving image to " << path.string();

    createParentPath(path);

    if (cv::imwrite(path.string(), image, params))
    {
        return true;
    }
    else
    {
        LOG(ERROR) << "Could not save image to: " << path.string();
        return false;
    }
}

bool writeJpegWithQuality(const fs::path& path, const cv::Mat& mat, int quality)
{
    CHECK(0 <= quality && quality <= 100) << "Bad JPEG quality: " << quality;
    std::vector<int> params = { CV_IMWRITE_JPEG_QUALITY, quality };
    return writeCvImage(path, mat, params);
}

Imagef readGrayImage(const fs::path& file_path)
{
    ERROR_CONTEXT("path", file_path.c_str());
    CHECK_F(fs::exists(file_path));
    CHECK_F(fileSizeBytes(file_path) != 0);
    const cv::Mat1b image_1b = cv::imread(file_path.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
    CHECK_F(!image_1b.empty(), "Failed to load image at '%s'", file_path.c_str());
    CHECK(image_1b.isContinuous());
    return mapImageFromCv(image_1b, linearFromSrgbByte);
}

Imagef readDepthImage(const fs::path& file_path)
{
    return makeImage(readDepthCvImage(file_path));
}

Imagef readAndAverageGrayImages(const fs::path& directory)
{
    const auto image_paths = getFilesInDir(directory, ".png");
    CHECK_GT(image_paths.size(), 0u);
    cv::Mat1d ir_sum;

    for (const auto& image_path : image_paths)
    {
        const auto ir = readGrayImage(image_path);
        if (ir_sum.empty())
        {
            ir_sum = makeCvMatConstPointer(ir);
        }
        else
        {
            ir_sum += makeCvMatConstPointer(ir);
        }
    }

    Imagef image_float{image::dimensions(ir_sum)};
    ir_sum.convertTo(makeCvMatPointer(image_float), CV_32F,
        1.0 / static_cast<double>(image_paths.size()));
    return image_float;
}

Imageb readImageMask(const fs::path& file_path)
{
    ERROR_CONTEXT("image path", file_path.c_str());
    return maskFromCvMat(readCvImageOrDie(file_path));
}

void writeImageMask(const fs::path& file_path, const Imageb& image)
{
    writeGrayCvImage(file_path, cvMaskFromImage(image));
}

void writeGrayImage(const fs::path& path, const Imagef& image)
{
    writeGrayCvImage(path, makeCvMatConstPointer(image));
}

void writeNormalizedGrayCvImage(const fs::path& path, const cv::Mat1f& mat)
{
    auto normalized_image = mat.clone();
    cv::normalize(normalized_image, normalized_image, 1, 0, cv::NORM_MINMAX);
    writeGrayCvImage(path, normalized_image);
}

void writeNormalizedGrayImage(const fs::path& path, const Imagef& image)
{
    const auto image_cv = makeCvMatConstPointer(image);
    writeNormalizedGrayCvImage(path, image_cv);
}

void writeNormalizedGrayImage(const fs::path& path, const Imaged& image)
{
    const auto image_float = convertToFloat(image);
    const auto image_cv = makeCvMatConstPointer(image_float);
    writeNormalizedGrayCvImage(path, image_cv);
}

cv::Mat1f decodeDepthCvImage(const cv::Mat_<uint16_t>& raw_depth)
{
    if (raw_depth.empty()) { return {}; }

    cv::Mat_<float> raw_depth_float;
    raw_depth.convertTo(raw_depth_float, CV_32FC1);
    // The third pixel is maxval when depth is saved in tenths of millimetres (dmm).
    auto encoded_in_dmm = raw_depth(2) == std::numeric_limits<uint16_t>::max();
    if (encoded_in_dmm)
    {
        raw_depth_float(2) = 0.f;
        return kMetersFrom100Micrometers * raw_depth_float;
    }
    else
    {
        return kMetersFromMillimeters * raw_depth_float;
    }
}

cv::Mat1f readDepthCvImage(const fs::path& path)
{
    cv::Mat_<uint16_t> raw_depth = readRawDepthPng(path);
    return decodeDepthCvImage(raw_depth);
}

cv::Mat1b readIrCvImage(const fs::path& path)
{
    return readCvImage(path, cv::IMREAD_GRAYSCALE);
}

void writeDepthCvImage(const fs::path& path, const cv::Mat1f& mat)
{
    ERROR_CONTEXT("path", path.c_str());
    cv::Mat_<uint16_t> depth_frame_ui16;
    mat.convertTo(depth_frame_ui16, CV_16U, k100MicrometersFromMeters);
    // Set third pixel to maxval to indicate dmm depth format.
    depth_frame_ui16(2) = std::numeric_limits<uint16_t>::max();
    CHECK(writeCvImage(path, depth_frame_ui16, {cv::IMWRITE_PNG_COMPRESSION, 5}));
}

void writeGrayCvImage(const fs::path& path, const cv::Mat& mat)
{
    CHECK_EQ(mat.channels(), 1);
    cv::Mat1b gray1b;
    if (mat.type() == CV_8U)
    {
        gray1b = mat;
    }
    else
    {
        CHECK(mat.type() == CV_32F || mat.type() == CV_64F);
        gray1b = makeCvMat(cv::Mat1f(mat), srgbByteFromLinear);
    }
    CHECK(writeCvImage(path, gray1b, {cv::IMWRITE_PNG_COMPRESSION, 5}));
}

void writeColorCvImage(const fs::path& path, const cv::Mat& image)
{
    // using compression=75 over compression=90 makes the file a fifth of the size.
    CHECK(writeCvImage(path, image, {cv::IMWRITE_JPEG_QUALITY, 75}));
}

} // namespace komb

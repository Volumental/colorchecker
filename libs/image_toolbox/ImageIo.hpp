#pragma once

#include <string>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs.hpp>

#include <common/Path.hpp>

#include "Image.hpp"

namespace komb {

bool isImageExtension(std::string extension);

/**
 * Acts like cv::imread.
 * The file is expected to be a gray-scale 16 bit image.
 * Returns an empty matrix (matrix.data == nullptr) iff the file is not found.
 * Aborts on all other errors (corrupt PNG file, unexpected bit depth etc).
 * The values are returned as is, i.e. in range [0, 2^16).
 */
cv::Mat_<uint16_t> readRawDepthPng(const fs::path& file_name);

/// mode is the same as for cv::imread
cv::Mat readCvImage(const fs::path& path, cv::ImreadModes mode = cv::IMREAD_UNCHANGED);
cv::Mat readCvImageOrDie(const fs::path& path, cv::ImreadModes mode = cv::IMREAD_UNCHANGED);

cv::Mat3b readCvImageBgrOrDie(const fs::path& path);

/// params is the same as for cv::imwrite
bool writeCvImage(const fs::path& path, const cv::Mat& mat, const std::vector<int>& params = {});

/// quality = [0, 100], higher is better.
bool writeJpegWithQuality(const fs::path& path, const cv::Mat& mat, int quality);

/// @brief Reads a single channel image and rescales the result from [0,255] to [0,1].
Imagef readGrayImage(const fs::path& file_path);

/// Decode the depth into meters.
cv::Mat1f decodeDepthCvImage(const cv::Mat_<uint16_t>& raw_depth);

/// @brief Reads a depth image in either mm or dmm format and returns the result in m units.
Imagef readDepthImage(const fs::path& file_path);

Imagef readAndAverageGrayImages(const fs::path& directory);

Imageb readImageMask(const fs::path& file_path);
void writeImageMask(const fs::path& file_path, const Imageb& image);

void writeGrayImage(const fs::path& path, const Imagef& image);

/// Load a depth frame from disk, determine unit (mm or dmm) and return as depth in meters.
cv::Mat1f readDepthCvImage(const fs::path& path);

cv::Mat1b readIrCvImage(const fs::path& path);

void writeDepthCvImage(const fs::path& path, const cv::Mat1f& mat);
void writeGrayCvImage(const fs::path& path, const cv::Mat& mat);
void writeColorCvImage(const fs::path& path, const cv::Mat& mat);

void writeNormalizedGrayImage(const fs::path& path, const Imagef& image);
void writeNormalizedGrayImage(const fs::path& path, const Imaged& image);
void writeNormalizedGrayCvImage(const fs::path& path, const cv::Mat1f& mat);

} // namespace komb

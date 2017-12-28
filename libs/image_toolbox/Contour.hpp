#pragma once

#include <tuple>
#include <vector>

#include <opencv2/core/mat.hpp>

namespace komb {

using Contour    = std::vector<cv::Point>;
using Contour2f  = std::vector<cv::Point2f>;
using Contours   = std::vector<Contour>;
using Contours2f = std::vector<Contour2f>;

struct SubpixelRefiner
{
    cv::Mat1f dx;
    cv::Mat1f dy;
    cv::Mat1f gradient_magnitude;
};

SubpixelRefiner makeSubpixelRefiner(const cv::Mat1b& image, int kernel_size);

void computeGradient(const cv::Mat1b& gray, int kernel_size, cv::Mat1f& out_dx, cv::Mat1f& out_dy);

cv::Point2f refineMaximaSubpix(
    const cv::Mat1f& gradient_magnitude,
    const cv::Point2f& gradient,
    const cv::Point2f& point);

Contours2f refineContoursSubpix(const cv::Mat1b& gray, const Contours& contours, int kernel_size);

Contours2f refineContoursSubpix(const SubpixelRefiner& refiner, const Contours& contours);

void refineContourSubpix(Contour2f& io_contour, const SubpixelRefiner& refiner);

std::vector<float> contourCurvature(const Contour2f& contour);

/// Compute the average intensity change orthogonal to the contour segments.
double contourCrossGradient(const cv::Mat1f& dx, const cv::Mat1f& dy, const Contour2f& contour);

/// Compute the fraction of overlap of contour points.
double contourOverlapFraction(const Contour2f& contour_a, const Contour2f& contour_b);

/// Split contour into contiguous segments where
/// consecutive line segments are equidirectional within max_directionality_angle radians
Contours2f splitContourByDirectionality(const Contour2f& in_contour, float angle_threshold);

/// Similar to the above but classified direction crudely as one of eight direction on a unit square
Contours2f splitContourByDiagonality(const Contour2f& contour, float angle_threshold);

/// Remove trim_amount of the length from each end.
std::vector<cv::Point2f> shortenSegment(const std::vector<cv::Point2f>& segment, float trim_amount);

// ----------------------------------------------------------------------------

struct PointPositionComparator
{
    bool operator()(const cv::Point& lhs, const cv::Point& rhs) const
    {
        return std::tie(lhs.y, lhs.x) < std::tie(rhs.y, rhs.x);
    }

    bool operator()(const cv::Point2f& lhs, const cv::Point2f& rhs) const
    {
        return std::tie(lhs.y, lhs.x) < std::tie(rhs.y, rhs.x);
    }
};

} // namespace komb

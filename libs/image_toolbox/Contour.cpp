#include "Contour.hpp"

#include <algorithm>
#include <set>
#include <utility>
#include <vector>

#include <opencv2/opencv.hpp>

#include <common/algorithm/Container.hpp>
#include <common/algorithm/Range.hpp>
#include <common/Logging.hpp>
#include <geometry_toolbox/MetricOpenCv.hpp>

#include "Magnitude.hpp"
#include "Tests.hpp"

namespace komb {

void computeGradient(const cv::Mat1b& gray, int kernel_size, cv::Mat1f& out_dx, cv::Mat1f& out_dy)
{
    // Normalization factor, see cv::getDerivKernels.
    double scale = 1.0 / std::pow(2, kernel_size * 2 - 3);
    cv::Sobel(gray, out_dx, CV_32F, 1, 0, kernel_size, scale);
    cv::Sobel(gray, out_dy, CV_32F, 0, 1, kernel_size, scale);
}

SubpixelRefiner makeSubpixelRefiner(const cv::Mat1b& image, int kernel_size)
{
    SubpixelRefiner refiner;
    computeGradient(image, kernel_size, refiner.dx, refiner.dy);
    refiner.gradient_magnitude = magnitude({refiner.dx, refiner.dy});
    return refiner;
}

cv::Point2f refineMaximaSubpix(
    const cv::Mat1f& gradient_magnitude,
    const cv::Point2f& gradient,
    const cv::Point2f& point)
{
    if (!isInsideImage(point, gradient_magnitude))
    {
        return point;
    }

    float pseudo_norm = std::max(std::abs(gradient.x), std::abs(gradient.y));
    cv::Point2f direction(gradient.x / pseudo_norm, gradient.y / pseudo_norm);

    cv::Point2f p = point;
    while (isInsideImage(p + direction, gradient_magnitude) &&
        gradient_magnitude(p + direction) > gradient_magnitude(p))
    {
        p += direction;
    }
    while (isInsideImage(p - direction, gradient_magnitude) &&
        gradient_magnitude(p - direction) > gradient_magnitude(p))
    {
        p -= direction;
    }

    // Fit a second order polynomial around the pixel with the local maximum.
    // z = a + bx + cx^2
    // a = z(0)

    // z(-1) = a - b + c
    // z(0)  = a
    // z(1)  = a + b + c

    // z(0)  = a
    // (z(1) - z(-1)) / 2 = b
    // (z(-1) + z(1)) / 2 - z(0) = c

    // z' = b + 2cx
    // 0  = b + 2cx
    // x = -b / 2c
    // x = -( (z(1) - z(-1)) / 2 ) / ( 2 * (z(-1) + z(1)) / 2 - z(0) )
    // x = -( (z(1) - z(-1)) / 2 ) / ( z(-1) + z(1) - 2 * z(0) )
    // TODO(Rasmus): Sample points linearly instead of nearest neighbor.
    p         = cv::Point(p);         // Round to nearest integer.
    direction = cv::Point(direction); // Round to nearest integer.
    if (isInsideImage(p + direction, gradient_magnitude) &&
        isInsideImage(p - direction, gradient_magnitude))
    {
        float zm1 = gradient_magnitude(p - direction);
        float z0  = gradient_magnitude(p);
        float z1  = gradient_magnitude(p + direction);
        float x = 0.5f * (zm1 - z1) / (zm1 + z1 - 2 * z0);
        if (std::abs(x) < 1.0f)
        {
            p += x * direction;
        }
    }
    return p;
}

Contours2f refineContoursSubpix(const cv::Mat1b& gray, const Contours& contours, int kernel_size)
{
    return refineContoursSubpix(makeSubpixelRefiner(gray, kernel_size), contours);
}

Contours2f refineContoursSubpix(const SubpixelRefiner& refiner, const Contours& contours)
{
    CHECK(!refiner.gradient_magnitude.empty());
    CHECK_EQ(refiner.dx.size(), refiner.gradient_magnitude.size());
    CHECK_EQ(refiner.dy.size(), refiner.gradient_magnitude.size());

    Contours2f refined_contours(contours.size());
    for (auto i : indices(contours))
    {
        refined_contours[i].resize(contours[i].size());
        cast(contours[i], refined_contours[i]);
    }
    for (auto& contour : refined_contours)
    {
        refineContourSubpix(contour, refiner);
    }
    return refined_contours;
}

void refineContourSubpix(Contour2f& io_contour, const SubpixelRefiner& refiner)
{
    if (io_contour.empty())
    {
        return;
    }

    // Refine by finding the local maxima of gradient magnitude in the gradient direction.
    for (auto& p : io_contour)
    {
        if (isInsideImage(p, refiner.dx))
        {
            cv::Point2f gradient(refiner.dx(p), refiner.dy(p));
            p = refineMaximaSubpix(refiner.gradient_magnitude, gradient, p);
        }
    }

    // Remove points that are too close together.
    Contour2f decimated;
    cv::Point2f prev_point = io_contour.back();
    for (auto& p : io_contour)
    {
        if (distance(p, prev_point) > 0.25f)
        {
            decimated.push_back(p);
            prev_point = p;
        }
    }

    if (decimated.size() >= 3)
    {
        io_contour = std::move(decimated);
    }
}

std::vector<float> contourCurvature(const Contour2f& contour)
{
    if (contour.empty())
    {
        return {};
    }
    const size_t n = contour.size();
    std::vector<float> curvature(n);
    for (size_t i : indices(contour))
    {
        //       c
        //      /
        // a---b
        size_t prev = (i + n - 1) % n;
        size_t next = (i + 1) % n;
        const cv::Point2f& a = contour[prev];
        const cv::Point2f& b = contour[i];
        const cv::Point2f& c = contour[next];
        cv::Point2f dp = (c - a) / 2;
        cv::Point2f ddp = (c - b) - (b - a);
        double norm_dp = norm(dp);
        curvature[i] = std::abs(ddp.y * dp.x - ddp.x * dp.y) / (norm_dp * norm_dp * norm_dp);
    }
    return curvature;
}

double contourCrossGradient(const cv::Mat1f& dx, const cv::Mat1f& dy, const Contour2f& contour)
{
    CHECK_EQ(dx.size(), dy.size());

    double sum    = 0.0;
    double weight = 0.0;
    const size_t n = contour.size();
    for (size_t i : indices(contour))
    {
        //       c
        //      /
        // a---b
        size_t prev = (i + n - 1) % n;
        size_t next = (i + 1) % n;
        const cv::Point2f& a = contour[prev];
        const cv::Point2f& b = contour[i];
        const cv::Point2f& c = contour[next];
        cv::Point2f dp = (c - a) / 2;
        if (isInsideImage(b, dx))
        {
            sum += dp.cross(cv::Point2f(dx(b), dy(b)));
            weight += norm(dp);
        }
    }
    if (weight > 0)
    {
        return sum / weight;
    }
    else
    {
        return 0.0;
    }
}

double contourOverlapFraction(const Contour2f& contour_a, const Contour2f& contour_b)
{
    if (contour_a.empty() || contour_b.empty())
    {
        return 0.0;
    }

    std::set<cv::Point, PointPositionComparator> set_a;
    std::set<cv::Point, PointPositionComparator> set_b;

    set_a.insert(contour_a.begin(), contour_a.end());
    set_b.insert(contour_b.begin(), contour_b.end());

    std::vector<cv::Point> intersection;
    std::set_intersection(set_a.begin(), set_a.end(), set_b.begin(), set_b.end(),
        std::back_inserter(intersection), PointPositionComparator());
    return double(intersection.size()) * 2.0 / (set_a.size() + set_b.size());
}

// ----------------------------------------------------------------------------

Contours2f splitContourByDiagonality(const Contour2f& contour, float angle_threshold)
{
    if (contour.empty())
    {
        return {};
    }

    const float threshold = std::sin(angle_threshold);

    // Extract stretches of horizontal and vertical segments
    Contours2f segments;
    bool active_segment = false;
    cv::Point active_direction;
    const size_t n = contour.size();
    for (size_t i : irange(contour.size() + 1))
    {
        size_t current = i % n;
        size_t next    = (i + 1) % n;
        cv::Point2f a = contour[current];
        cv::Point2f b = contour[next];
        cv::Point2f d = normalized(b - a);
        cv::Point current_direction = cv::Point(d); // Rounded to closest integer.

        float diagonality = std::min(std::abs(d.x), std::abs(d.y));

        if (diagonality < threshold && current_direction != cv::Point(0, 0))
        {
            if (active_segment && current_direction == active_direction)
            {
                segments.back().push_back(b);
            }
            else
            {
                segments.push_back({a, b});
                active_segment = true;
                active_direction = current_direction;
            }
        }
        else
        {
            active_segment = false;
        }
    }

    // Merge first and last segment if they are compatible.
    if (segments.size() >= 2 && active_segment)
    {
        std::copy(segments.front().begin() + 2, segments.front().end(),
            std::back_inserter(segments.back()));
        segments.front() = std::move(segments.back());
        segments.pop_back();
    }
    return segments;
}

// ----------------------------------------------------------------------------

cv::Point2f segmentDir(const Contour2f& segment)
{
    CHECK_GE(segment.size(), 2u);
    return normalized(segment.back() - segment.front());
}

// Would appending the given point to segment keep the direction of the segment?
bool followsDirectionality(
    const Contour2f& segment, const cv::Point2f& new_dir, float dot_threshold)
{
    if (segment.size() < 2)
    {
       return true;
    }

    bool same_dir = segmentDir(segment).dot(new_dir) > dot_threshold;
    return same_dir;
}

bool mergeConsecutiveIfSameDirection(Contours2f& io_segments, float dot_threshold)
{
    bool did_merge = false;

    for (size_t i = 0; i < io_segments.size() && io_segments.size() > 1; )
    {
        auto& segment_a = io_segments[i];
        auto& segment_b  = io_segments[(i + 1) % io_segments.size()];
        if (followsDirectionality(segment_b, segmentDir(segment_a), dot_threshold))
        {
            append(segment_a, segment_b);
            segment_a.swap(segment_b);
            eraseIndex(io_segments, i);
            did_merge = true;
        }
        else
        {
            ++i;
        }
    }

    return did_merge;
}

Contours2f splitContourByDirectionality(const Contour2f& in_contour, float angle_threshold)
{
    if (in_contour.empty())
    {
        return {};
    }

    const float dot_threshold = std::cos(angle_threshold);

    Contours2f out_segments(1, Contour2f{in_contour[0]});

    for (const auto i : irange<size_t>(1, in_contour.size()))
    {
        const auto& point = in_contour[i];
        Contour2f& current_segment = out_segments.back();
        cv::Point2f new_dir = normalized(point - current_segment.back());
        if (followsDirectionality(current_segment, new_dir, dot_threshold))
        {
            current_segment.push_back(point);
        }
        else
        {
            out_segments.push_back({point});
        }
    }

    while (mergeConsecutiveIfSameDirection(out_segments, dot_threshold)) { }

    return out_segments;
}

std::vector<cv::Point2f> shortenSegment(const std::vector<cv::Point2f>& segment, float trim_amount)
{
    if (trim_amount <= 0) { return segment; }

    size_t begin_short = 0;
    size_t end_short   = segment.size();

    float length_removed_front = 0;
    do {
        if (begin_short + 1 >= end_short) { return {}; }
        length_removed_front += norm(segment[begin_short] - segment[begin_short + 1]);
        ++begin_short;
    } while (length_removed_front < trim_amount);

    float length_removed_end = 0;
    do {
        if (begin_short + 1 >= end_short) { return {}; }
        length_removed_end += norm(segment[end_short - 2] - segment[end_short - 1]);
        --end_short;
    } while (length_removed_end < trim_amount);

    return std::vector<cv::Point2f>(&segment[begin_short], &segment[end_short]);
}

} // namespace komb

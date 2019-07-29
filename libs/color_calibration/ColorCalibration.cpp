#include "ColorCalibration.hpp"

#include <algorithm>
#include <functional>
#include <utility>
#include <vector>

#include <opencv2/opencv.hpp>

#include <common/algorithm/Container.hpp>
#include <common/algorithm/Range.hpp>
#include <common/Logging.hpp>
#include <common/Math.hpp>
#include <common/String.hpp>
#include <image_toolbox/Magnitude.hpp>
#include <image_toolbox/Tests.hpp>

namespace komb {

struct FindSquaresRetVal
{
    std::vector<std::vector<cv::Point>> contours;
    std::vector<double> sizes;
};

FindSquaresRetVal findSquares(
    const cv::Mat3b& image, cv::Mat3b& canvas)
{
    CHECK(!image.empty());

    std::vector<std::vector<cv::Point>> contours;
    cv::Mat areas = edgeMagnitude(image) <= 2;
    cv::findContours(areas, contours, cv::RETR_LIST, cv::CHAIN_APPROX_TC89_L1);

    if (!canvas.empty())
    {
        cv::drawContours(canvas, contours, -1, cv::Scalar(255, 255, 255));
    }

    std::vector<std::vector<cv::Point>> square_contours;
    std::vector<double> square_sizes;
    for (const auto& contour : contours)
    {
        std::vector<cv::Point> simple_contour;
        cv::approxPolyDP(contour, simple_contour, 15, true);

        double area = -cv::contourArea(contour, true);
        if (area > 0 && simple_contour.size() == 4)
        {
            std::vector<double> lengths(simple_contour.size());
            for (const auto a : indices(simple_contour))
            {
                auto b = (a + 1) % simple_contour.size();
                lengths[a] = cv::norm(simple_contour[a] - simple_contour[b]);
            }
            double mean_length = cv::mean(lengths)[0];
            bool mean_length_vs_area_ok = (std::abs(mean_length * mean_length - area) < area * 0.1);

            bool even_lengths = komb::all_of(lengths, [&mean_length](float length)
            {
                 return std::abs(length - mean_length) <= mean_length * 0.1f;
            });

            if (mean_length_vs_area_ok && even_lengths)
            {
                square_contours.push_back(simple_contour);
                square_sizes.push_back(mean_length);
            }

            if (!canvas.empty())
            {
                cv::Scalar color =
                    even_lengths && mean_length_vs_area_ok ? cv::Scalar(0, 255, 0) :
                    even_lengths || mean_length_vs_area_ok ? cv::Scalar(0, 127, 255) :
                    cv::Scalar(0, 0, 255);
                cv::polylines(canvas, simple_contour, true, color);
            }
        }
    }
    return {square_contours, square_sizes};
}

cv::Mat3b findColorChecker(
    const cv::Mat3b& image, cv::Mat3b& canvas)
{
    CHECK(!image.empty());

    const FindSquaresRetVal squares = findSquares(image, canvas);

    if (squares.sizes.size() == 0)
    {
        LOG(WARNING) << "Found no squares";
        return cv::Mat();
    }

    auto square_sizes_copy = squares.sizes;
    komb::nth_element(square_sizes_copy, square_sizes_copy.size() / 2);
    double median_square_size = square_sizes_copy[square_sizes_copy.size() / 2];

    VLOG(1) << "Median square size: " << median_square_size;

    std::vector<cv::Point> square_centers;
    cv::Vec2f x_axis;
    cv::Vec2f y_axis;
    for (const auto i : indices(squares.sizes))
    {
        const auto& square_contour = squares.contours[i];
        if (std::abs(squares.sizes[i] - median_square_size) < median_square_size * 0.1)
        {
            cv::Scalar center = cv::mean(square_contour);
            square_centers.push_back(
                cv::Point(komb::roundToInt(center[0]), komb::roundToInt(center[1])));

            for (int a = 0; a < 4; ++a)
            {
                auto b = (a + 1) & 3;
                cv::Point2f vec(square_contour[a] - square_contour[b]);
                if (std::abs(vec.x) > std::abs(vec.y))
                {
                    x_axis += cv::Vec2f(vec.x > 0 ? vec : -vec);
                }
                else
                {
                    y_axis += cv::Vec2f(vec.y > 0 ? vec : -vec);
                }
            }
        }
    }
    VLOG(1) << "Picked " << square_centers.size() << " of " << squares.sizes.size() << " squares.";

    x_axis = cv::normalize(x_axis);
    y_axis = cv::normalize(y_axis);

    cv::Point x_arrow = cv::Vec2i(x_axis * 20);
    cv::Point y_arrow = cv::Vec2i(y_axis * 20);

    cv::Mat1f map_from_image(2, 2);
    map_from_image << x_axis[0], x_axis[1], y_axis[0], y_axis[1];

    std::vector<cv::Point2f> adjusted_centers;
    for (const auto& center : square_centers)
    {
        cv::Mat1f adjusted = map_from_image * (cv::Mat1f(2, 1) << center.x, center.y);
        adjusted_centers.emplace_back(adjusted(0), adjusted(1));
    }

    auto get_x = [](const cv::Point2f& p){ return p.x; };
    auto get_y = [](const cv::Point2f& p){ return p.y; };
    const float min_x = pickSmallest(adjusted_centers, get_x).x;
    const float max_x = pickLargest(adjusted_centers, get_x).x;
    const float min_y = pickSmallest(adjusted_centers, get_y).y;
    const float max_y = pickLargest(adjusted_centers, get_y).y;

    const int num_cols = 6;
    const int num_rows = 4;

    for (auto& adjusted : adjusted_centers)
    {
        adjusted.x = (adjusted.x - min_x) / (max_x - min_x) * (num_cols - 1);
        adjusted.y = (adjusted.y - min_y) / (max_y - min_y) * (num_rows - 1);
    }

    for (const auto i : indices(square_centers))
    {
        const cv::Point& center = square_centers[i];
        const cv::Point2f& adjusted_center = adjusted_centers[i];

        if (!canvas.empty())
        {
            cv::line(canvas, center, center + x_arrow, cv::Scalar(0, 0, 255));
            cv::line(canvas, center, center + y_arrow, cv::Scalar(0, 255, 0));
            cv::putText(canvas, strprintf("(%.0f, %.0f)", adjusted_center.x, adjusted_center.y),
                center - cv::Point(20, 10), cv::FONT_HERSHEY_PLAIN, .75, cv::Scalar(255, 255, 255));
        }
    }

    // Fit a multivariate polynomial to get a function from row,col to image x,y.
    cv::Mat1f AtA(6, 6, 0.f);
    cv::Mat1f AtB(6, 2, 0.f);
    for (const auto i : indices(square_centers))
    {
        float row = std::round(adjusted_centers[i].y);
        float col = std::round(adjusted_centers[i].x);
        cv::Mat1f A_row(1, 6);
        A_row << 1, row, col, row * row, col * col, row * col;
        cv::Mat1f xy(square_centers[i], true);
        AtA += A_row.t() * A_row;
        AtB += A_row.t() * xy.t();
    }
    cv::Mat1f transformation_parameters = AtA.inv() * AtB;
    VLOG(2) << "AtA:\n" << AtA;
    VLOG(2) << "AtB:\n" << AtB;
    VLOG(2) << "Transformation parameters:\n" << transformation_parameters;

    cv::Mat3b ordered_colors(num_rows, num_cols);
    for (int row : irange(num_rows))
    {
        for (int col : irange(num_cols))
        {
            cv::Mat1f A_row(1, 6);
            A_row << 1, row, col, row * row, col * col, row * col;
            cv::Mat1f xy = A_row * transformation_parameters;
            cv::Point center(std::round(xy(0)), std::round(xy(1)));
            if (!isInsideImage(center, image))
            {
                LOG(WARNING) << "Predicted square center is outside the image.";
                return cv::Mat();
            }
            cv::Vec3b color = image.at<cv::Vec3b>(center);
            ordered_colors(row, col) = color;

            cv::Scalar contrast_color = color.dot(color / 255) < 255 ?
                cv::Scalar::all(255) : cv::Scalar::all(0);

            if (!canvas.empty())
            {
                cv::circle(canvas, center, 1, contrast_color, -1);
                cv::circle(canvas, center, 6, contrast_color, 1);
            }
        }
    }
    return ordered_colors;
}

cv::Mat3b bigChecker(const cv::Mat3b& checker)
{
    const int square_size = 50;
    const int padding = 10;
    const int cc = square_size + padding;
    cv::Mat3b big_checker(
        checker.rows * cc + padding,
        checker.cols * cc + padding,
        cv::Vec3b(127, 127, 127));
    cv::Rect square(0, 0, square_size, square_size);
    for (int i = 0; i < checker.rows; ++i)
    {
        for (int j = 0; j < checker.cols; ++j)
        {
            square.x = padding + cc * j;
            square.y = padding + cc * i;
            cv::rectangle(big_checker, square, checker(i, j), -1);
        }
    }
    return big_checker;
}

cv::Matx34f findColorTransformation(
    const cv::Mat3b& camera_checker,
    const cv::Mat3b& reference_checker)
{
    CHECK(!camera_checker.empty());
    CHECK(!reference_checker.empty());
    CHECK_EQ(camera_checker.size(), reference_checker.size());

    int num_colors = camera_checker.total();
    cv::Mat1f AtA(4, 4, 0.f);
    cv::Mat1f AtB(4, 3, 0.f);
    for (const auto i : irange(num_colors))
    {
        cv::Vec3b cam_color = camera_checker(i);
        cv::Vec3b ref_color = reference_checker(i);
        cv::Mat1f A_row(1, 4);
        A_row << cam_color[0], cam_color[1], cam_color[2], 1;
        cv::Mat1f b(cv::Vec3f(ref_color), true);
        AtA += A_row.t() * A_row;
        AtB += A_row.t() * b.t();
    }
    cv::Mat1f transformation_parameters = (AtA.inv() * AtB).t();
    VLOG(4) << "Color transformation computation:";
    VLOG(4) << "AtA:\n" << AtA;
    VLOG(4) << "AtB:\n" << AtB;
    VLOG(4) << "Transformation parameters:\n" << transformation_parameters;
    return transformation_parameters;
}

void applyColorTransformation(
    cv::Mat3b& image, const cv::Matx34f& color_transformation)
{
    auto transform = [&color_transformation](cv::Vec3b cam_color)
    {
        cv::Vec4f A_row(cam_color[0], cam_color[1], cam_color[2], 1);
        cv::Vec3f new_color = color_transformation * A_row;
        return cv::Vec3b(
            cv::saturate_cast<uchar>(new_color(0)),
            cv::saturate_cast<uchar>(new_color(1)),
            cv::saturate_cast<uchar>(new_color(2)));
    };

    int num_pixels = image.total();
    for (int i = 0; i < num_pixels; ++i)
    {
        image(i) = transform(image(i));
    }
}

float medianAbsoluteDeviation(const cv::Mat& a, const cv::Mat& b)
{
    CHECK(!a.empty());
    CHECK(!b.empty());
    CHECK_EQ(a.size(), b.size());
    CHECK_EQ(a.channels(), b.channels());

    cv::Mat1f abs_devs = cv::abs(a.reshape(1) - b.reshape(1));
    int mid = abs_devs.total() / 2;
    std::nth_element(abs_devs.begin(), abs_devs.begin() + mid, abs_devs.end());
    return abs_devs(mid);
}

} // namespace komb

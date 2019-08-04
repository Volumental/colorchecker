#include "ColorCalibration.hpp"

#include <algorithm>
#include <functional>
#include <utility>
#include <vector>

#include <opencv2/opencv.hpp>

#include <common/algorithm/Container.hpp>
#include <common/algorithm/Range.hpp>
#include <common/algorithm/Vector.hpp>
#include <common/Json.hpp>
#include <common/Logging.hpp>
#include <common/Math.hpp>
#include <common/String.hpp>
#include <image_toolbox/Magnitude.hpp>
#include <image_toolbox/Tests.hpp>
#include <image_toolbox/Visualization.hpp>

namespace komb {

namespace {
const cv::Scalar kBrightGreen(0, 255, 0);
const cv::Scalar kDarkGreen(0, 127, 0);
const cv::Scalar kPink(196, 32, 255);
const cv::Scalar kOrange(0, 127, 255);
const cv::Scalar kRed(0, 0, 255);
const cv::Scalar kWhite(255, 255, 255);
} // namespace

std::vector<cv::Point2f> simplifyContour(
    const std::vector<cv::Point>& contour, cv::Mat3b& canvas)
{
    Json contour_json = Json::array();
    for (const auto& p : contour)
    {
        contour_json.push_back(Json::array({p.x, p.y}));
    }
    ERROR_CONTEXT("contour", &contour_json);

    static unsigned int r = 14;

    // The contour is shifted slightly to make the points unique.
    std::vector<cv::Point2f> shifted_contour;
    for (const auto i : indices(contour))
    {
        const cv::Point2f a = contour[i];
        const cv::Point2f b = contour[(i + 1) % contour.size()];
        const cv::Point2f ab = b - a;
        shifted_contour.push_back(a + ab * 0.01f);
    }

    std::vector<cv::Point2f> simple_contour;
    cv::approxPolyDP(shifted_contour, simple_contour, 5, true);

    Json simple_contour_json = Json::array();
    for (const auto& p : simple_contour)
    {
        simple_contour_json.push_back(Json::array({p.x, p.y}));
    }
    ERROR_CONTEXT("simple_contour", &simple_contour_json);

    if (simple_contour.size() < 3)
    {
        return simple_contour;
    }

    // Find last simplified point from contour.
    size_t index_in_contour = shifted_contour.size();
    while (index_in_contour > 0)
    {
        --index_in_contour;
        if (shifted_contour[index_in_contour] == simple_contour.back())
        {
            break;
        }
    }

    // Collect line segments divided by the simplified points.
    size_t points_left_in_contour = shifted_contour.size();
    std::vector<std::vector<cv::Point>> segments(1);
    size_t index_in_simple_contour = 0;
    while (points_left_in_contour > 0)
    {
        // Add to current segment.
        segments.back().push_back(shifted_contour[index_in_contour]);

        if (shifted_contour[index_in_contour] == simple_contour[index_in_simple_contour])
        {
            // Start a new segment.
            segments.push_back({shifted_contour[index_in_contour]});
            ++index_in_simple_contour;
            CHECK_LT(index_in_simple_contour, simple_contour.size());
        }

        index_in_contour = (index_in_contour + 1) % shifted_contour.size();
        --points_left_in_contour;
    }
    CHECK_EQ(segments.size(), simple_contour.size());

    // Fit lines to the segments.
    const std::vector<cv::Point3f> line_coordinates = map(segments,
        [](const std::vector<cv::Point>& segment) -> cv::Point3f
    {
        // Line coordinates are (a, b, c) such that a*x + b*y + c = 0.
        cv::Mat1d AtA(3, 3, 0.f);
        const double k = 1.0e-3; // Arbitrary normalization.
        for (const auto& p : segment)
        {
            cv::Mat1d A_row(1, 3);
            double u = p.x * k;
            double v = p.y * k;
            A_row << u, v, 1;
            AtA += A_row.t() * A_row;
        }
        cv::Mat1d eigenvalues;
        cv::Mat1d eigenvectors;
        cv::eigen(AtA, eigenvalues, eigenvectors);
        cv::Mat1f fitted_line = eigenvectors.row(2);
        VLOG(2) << "AtA:\n" << AtA;
        VLOG(2) << "Fitted line:\n" << fitted_line;
        return cv::Point3f(
            fitted_line(0) * k,
            fitted_line(1) * k,
            fitted_line(2));
    });
    CHECK_EQ(line_coordinates.size(), simple_contour.size());

    // Create new points from the intersection of the lines.
    for (const auto i : indices(line_coordinates))
    {
        const cv::Point3f& line_1 = line_coordinates[i];
        const cv::Point3f& line_2 = line_coordinates[(i + 1) % line_coordinates.size()];
        const cv::Point3f intersection = line_1.cross(line_2);
        if (intersection.z != 0)
        {
            simple_contour[i].x = intersection.x / intersection.z;
            simple_contour[i].y = intersection.y / intersection.z;
        }
    }
    return simple_contour;
}

struct FindSquaresRetVal
{
    std::vector<std::vector<cv::Point2f>> contours;
    std::vector<double> sizes;
};

FindSquaresRetVal findSquares(
    const cv::Mat3b& image, cv::Mat3b& canvas)
{
    CHECK(!image.empty());

    std::vector<std::vector<cv::Point>> contours;
    cv::Mat areas = edgeMagnitude(image) <= 2;
    cv::findContours(areas, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

    if (!canvas.empty())
    {
        cv::drawContours(canvas, contours, -1, kWhite);
    }

    std::vector<std::vector<cv::Point2f>> square_contours;
    std::vector<double> square_sizes;
    for (const auto& contour : contours)
    {
        const double area = -cv::contourArea(contour, true);

        if (area <= 0)
        {
            continue;
        }

        const std::vector<cv::Point2f> simple_contour = simplifyContour(contour, canvas);

        if (area > 0 && simple_contour.size() == 4)
        {
            std::vector<double> lengths(simple_contour.size());
            for (const auto a : indices(simple_contour))
            {
                const auto b = (a + 1) % simple_contour.size();
                lengths[a] = cv::norm(simple_contour[a] - simple_contour[b]);
            }
            const double mean_length = cv::mean(lengths)[0];
            const bool mean_length_vs_area_ok = (std::abs(mean_length * mean_length - area) < area * 0.2);

            const bool even_lengths = komb::all_of(lengths, [&mean_length](float length)
            {
                 return std::abs(length - mean_length) <= mean_length * 0.2f;
            });

            if (mean_length_vs_area_ok && even_lengths)
            {
                square_contours.push_back(simple_contour);
                square_sizes.push_back(mean_length);
            }

            if (!canvas.empty())
            {
                const cv::Scalar color =
                    even_lengths && mean_length_vs_area_ok ? kDarkGreen :
                    !even_lengths && mean_length_vs_area_ok ? kPink :
                    even_lengths && !mean_length_vs_area_ok ? kOrange :
                    kRed;
                polyLinesSubPix(canvas, simple_contour, true, color, 1);
            }
        }
    }
    return {square_contours, square_sizes};
}

size_t getMedianSquareIndex(const FindSquaresRetVal& squares)
{
    std::vector<size_t> square_indices_by_size = makeVector(indices(squares.sizes));
    komb::sort(square_indices_by_size, [&squares](size_t i, size_t j)
    {
        return squares.sizes[i] < squares.sizes[j];
    });
    const size_t median_square_index = square_indices_by_size[squares.sizes.size() / 2];
    return median_square_index;
}

std::vector<size_t> filterSquaresBySize(
    const FindSquaresRetVal& squares,
    float max_fractional_length_difference)
{
    const double median_square_size = squares.sizes[getMedianSquareIndex(squares)];
    VLOG(1) << "Median square size: " << median_square_size;
    std::vector<size_t> median_friendly_squares_indices;
    for (const auto i : indices(squares.sizes))
    {
        const float smaller_size = std::min(squares.sizes[i], median_square_size);
        const float bigger_size = std::max(squares.sizes[i], median_square_size);
        // fractional_length_difference = (bigger_size - smaller_size) / bigger_size;
        if (bigger_size - smaller_size <= bigger_size * max_fractional_length_difference)
        {
            median_friendly_squares_indices.push_back(i);
        }
    }
    VLOG(1) << "Picked " << median_friendly_squares_indices.size()
        << " of " << squares.sizes.size() << " squares.";
    return median_friendly_squares_indices;
}

FindSquaresRetVal pickSquaresByIndices(
    const FindSquaresRetVal& squares,
    const std::vector<size_t>& indices)
{
    FindSquaresRetVal out;
    for (const auto i : indices)
    {
        out.contours.push_back(squares.contours[i]);
        out.sizes.push_back(squares.sizes[i]);
    }
    return out;
}

cv::Matx33f getPerspectiveTransformFromSquare(
    const std::vector<cv::Point2f>& sample_square_contour)
{
    CHECK_EQ(sample_square_contour.size(), 4);
    const cv::Scalar center = cv::mean(sample_square_contour);

    cv::Mat1d AtA(8, 8, 0.f);
    cv::Mat1d AtB(8, 1, 0.f);
    cv::Mat1d A_row(1, 8);
    for (const auto& p : sample_square_contour)
    {
        // TODO(Rasmus): Add normalization for numerical stability.
        const float x = p.x;
        const float y = p.y;
        const float u = p.x < center[0] ? 0 : 1;
        const float v = p.y < center[1] ? 0 : 1;

        // Add constraints for absolute positions.
        A_row << x, y, 1, 0, 0, 0, -x * u, -y * u;
        AtA += A_row.t() * A_row;
        AtB += A_row.t() * u;
        A_row << 0, 0, 0, x, y, 1, -x * v, -y * v;
        AtA += A_row.t() * A_row;
        AtB += A_row.t() * v;
    }

    VLOG(2) << "Single square solve:";
    VLOG(2) << "AtA:\n" << AtA;
    VLOG(2) << "AtB:\n" << AtB;

    cv::Mat M(3, 3, CV_64F), X(8, 1, CV_64F, M.ptr());
    cv::solve(AtA, AtB, X, cv::DECOMP_SVD);
    M.ptr<double>()[8] = 1.;
    VLOG(2) << "Matrix coefficients:\n" << M;
    return M;
}

cv::Matx33f getAveragePerspectiveTransformFromSquares(
    const std::vector<std::vector<cv::Point2f>>& square_contours)
{
    cv::Matx33d image_from_ortho_view_sum = cv::Matx33d::zeros();
    for (const auto& contour : square_contours)
    {
        const cv::Matx33d ortho_view_from_image = getPerspectiveTransformFromSquare(contour);
        image_from_ortho_view_sum += ortho_view_from_image.inv(cv::DECOMP_SVD);
    }
    // Use Singular Value Decomposition for inverses to handle poor conditioned systems.
    return (image_from_ortho_view_sum * (1.0 / image_from_ortho_view_sum(2, 2))).inv(cv::DECOMP_SVD);
}

cv::Point3f getLineCoordinateFromPoints(const cv::Point2f& a, const cv::Point2f& b)
{
    const float dx = b.x - a.x;
    const float dy = b.y - a.y;
    return { dy, -dx, dx * a.y - dy * a.x};
}

cv::Point2f getGeometricSquareCenter(const std::vector<cv::Point2f>& contour)
{
    CHECK_EQ(contour.size(), 4);
    const cv::Point3f line_1 = getLineCoordinateFromPoints(contour[0], contour[2]);
    const cv::Point3f line_2 = getLineCoordinateFromPoints(contour[1], contour[3]);
    const cv::Point3f intersection = line_1.cross(line_2);
    return { intersection.x / intersection.z, intersection.y / intersection.z };
}

cv::Matx33f getPerspectiveTransformFromAllSquares(
    const std::vector<std::vector<cv::Point2f>>& square_contours)
{
    const double k = 1.0e-3; // Arbitrary normalization.

    cv::Mat1d AtA(8, 8, 0.f);
    cv::Mat1d AtB(8, 1, 0.f);

    // Reusable matrices for adding constraints.
    cv::Mat1d A_row(1, 8, 0.0);

    // Add a single constraint for absolute position.
    {
        cv::Point2d sum(0, 0);
        double count = 0.0;
        for (const auto& contour : square_contours)
        {
            for (const auto& p : contour)
            {
                sum += cv::Point2d(p);
                count += 1;
            }
        }
        const cv::Point2d center = sum / count;
        const float x = k * center.x;
        const float y = k * center.y;
        const float u = 0.5f;
        const float v = 0.5f;

        A_row << x, y, 1, 0, 0, 0, -x * u, -y * u;
        AtA += A_row.t() * A_row;
        AtB += A_row.t() * u;
        A_row << 0, 0, 0, x, y, 1, -x * v, -y * v;
        AtA += A_row.t() * A_row;
        AtB += A_row.t() * v;
    }

    // Add relative constraints for the edges of the squares.
    for (const auto i : indices(square_contours))
    {
        const auto& contour = square_contours[i];
        const auto center = getGeometricSquareCenter(contour);
        const float cx = k * center.x;
        const float cy = k * center.y;
        for (const auto& p : contour)
        {
            const float x = k * p.x;
            const float y = k * p.y;
            const float dx = x - cx;
            const float dy = y - cy;
            const float du = dx < 0 ? -0.5 : 0.5;
            const float dv = dy < 0 ? -0.5 : 0.5;

            A_row << dx, dy, 0, 0, 0, 0, -x*du, -y*du;
            AtA += A_row.t() * A_row;
            AtB += A_row.t() * du;

            A_row << 0, 0, 0, dx, dy, 0, -x*dv, -y*dv;
            AtA += A_row.t() * A_row;
            AtB += A_row.t() * dv;
        }
    }

    VLOG(2) << "Multiple square solve:";
    VLOG(2) << "AtA:\n" << AtA;
    VLOG(2) << "AtB:\n" << AtB;

    cv::Mat1d X(8, 1);
    cv::solve(AtA, AtB, X, cv::DECOMP_SVD);
    VLOG(2) << "X:\n" << X;

    const cv::Matx33d ortho_view_from_normalized_image(
        X(0), X(1), X(2),
        X(3), X(4), X(5),
        X(6), X(7), 1.0);
    VLOG(2) << "Matrix coefficients:\n" << ortho_view_from_normalized_image;
    const cv::Matx33d normalized_image_from_image(
        k,   0.0, 0.0,
        0.0, k,   0.0,
        0.0, 0.0, 1.0);
    return ortho_view_from_normalized_image * normalized_image_from_image;
}

// Fit a multivariate polynomial that transforms source to destination.
// Returns a 6x2 matrix M that maps vectors to points like this:
// [1, x, y, x * x, y * y, x * y] * M = [u, v]
cv::Mat1f fit2dPolynomial(
    const std::vector<cv::Point2f>& source,
    const std::vector<cv::Point2f>& destination)
{
    CHECK_EQ(source.size(), destination.size());
    cv::Mat1f AtA(6, 6, 0.f);
    cv::Mat1f AtB(6, 2, 0.f);
    for (const auto i : indices(destination))
    {
        const float x = source[i].x;
        const float y = source[i].y;
        cv::Mat1f A_row(1, 6);
        A_row << 1, x, y, x * x, y * y, x * y;
        cv::Mat1f uv(destination[i], true);
        AtA += A_row.t() * A_row;
        AtB += A_row.t() * uv.t();
    }
    const cv::Mat1f M = AtA.inv() * AtB;
    VLOG(2) << "AtA:\n" << AtA;
    VLOG(2) << "AtB:\n" << AtB;
    VLOG(2) << "Polynomial coefficients:\n" << M;
    return M;
}

cv::Mat3b renderSquaresInBigOrthoView(
    const cv::Mat3b& image,
    const cv::Matx33f& ortho_view_from_image,
    const FindSquaresRetVal& squares_in_ortho_view,
    const std::vector<cv::Point2f>& square_centers_in_image,
    const std::vector<cv::Point2f>& square_centers_in_ortho_view,
    const std::vector<size_t>& median_friendly_squares_indices)
{
    const cv::Size canvas_size(800, 500);
    const cv::Point canvas_center(canvas_size.width / 2, canvas_size.height / 2);
    const int canvas_square_size = 40;
    cv::Mat3b canvas;
    const cv::Matx33f big_ortho_view_from_ortho_view(
        canvas_square_size, 0, canvas_center.x - canvas_square_size / 2,
        0, canvas_square_size, canvas_center.y - canvas_square_size / 2,
        0, 0, 1);
    const cv::Matx33f big_ortho_view_from_image =
        big_ortho_view_from_ortho_view * ortho_view_from_image;
    cv::warpPerspective(image, canvas,
        big_ortho_view_from_image, canvas_size);
    cv::line(canvas,
        cv::Point(canvas_center.x, 0),
        cv::Point(canvas_center.x, canvas_size.height),
        kWhite);
    cv::line(canvas,
        cv::Point(0, canvas_center.y),
        cv::Point(canvas_size.width, canvas_center.y),
        kWhite);

    std::vector<bool> is_square_friendly(squares_in_ortho_view.contours.size(), false);
    for (const auto i : median_friendly_squares_indices)
    {
        is_square_friendly[i] = true;
    }
    for (const auto i : indices(squares_in_ortho_view.contours))
    {
        auto square_contour_in_big_ortho_view =
            squares_in_ortho_view.contours[i];
        cv::perspectiveTransform(
            squares_in_ortho_view.contours[i],
            square_contour_in_big_ortho_view,
            big_ortho_view_from_ortho_view);
        const auto color = is_square_friendly[i] ? kBrightGreen : kDarkGreen;
        polyLinesSubPix(canvas,
            square_contour_in_big_ortho_view, true, color, 1);
    }

    if (!square_centers_in_image.empty())
    {
        const auto square_centers_in_image_in_big_ortho_view = square_centers_in_image;
        cv::perspectiveTransform(
            square_centers_in_image,
            square_centers_in_image_in_big_ortho_view,
            big_ortho_view_from_image);
        for (const auto& center : square_centers_in_image_in_big_ortho_view)
        {
            cv::drawMarker(canvas, center, kWhite, cv::MARKER_SQUARE, 8);
        }
    }

    if (!square_centers_in_ortho_view.empty())
    {
        const auto square_centers_in_ortho_view_in_big_ortho_view = square_centers_in_ortho_view;
        cv::perspectiveTransform(
            square_centers_in_ortho_view,
            square_centers_in_ortho_view_in_big_ortho_view,
            big_ortho_view_from_ortho_view);
        for (const auto& center : square_centers_in_ortho_view_in_big_ortho_view)
        {
            cv::drawMarker(canvas, center, kWhite, cv::MARKER_TILTED_CROSS, 8);
        }
    }

    return canvas;
}

cv::Mat3b findColorChecker(
    const cv::Mat3b& image, cv::Mat3b& canvas)
{
    LOG_SCOPE_F(1, "findColorChecker");
    CHECK(!image.empty());

    const FindSquaresRetVal squares = findSquares(image, canvas);

    if (squares.sizes.size() == 0)
    {
        LOG(WARNING) << "Found no squares";
        return cv::Mat();
    }

    const FindSquaresRetVal median_friendly_squares_in_image =
        pickSquaresByIndices(squares, filterSquaresBySize(squares, 0.5));

    const cv::Matx33f ortho_view_from_image =
        getPerspectiveTransformFromAllSquares(median_friendly_squares_in_image.contours);

    FindSquaresRetVal squares_in_ortho_view;
    squares_in_ortho_view.contours.resize(squares.contours.size());
    squares_in_ortho_view.sizes.resize(squares.sizes.size());
    for (const auto i : indices(squares.contours))
    {
        cv::perspectiveTransform(
            squares.contours[i],
            squares_in_ortho_view.contours[i],
            ortho_view_from_image);
        squares_in_ortho_view.sizes[i] =
            std::sqrt(cv::contourArea(squares_in_ortho_view.contours[i]));
    }

    const std::vector<size_t> median_friendly_squares_indices =
        filterSquaresBySize(squares_in_ortho_view, 0.3);

    if (!canvas.empty())
    {
        for (const auto i : median_friendly_squares_indices)
        {
            const auto& square_contour = squares.contours[i];
            polyLinesSubPix(canvas, square_contour, true, kBrightGreen, 1);
        }
    }

    std::vector<cv::Point2f> square_centers_in_image;
    std::vector<cv::Point2f> square_centers_in_ortho_view;
    for (const auto i : median_friendly_squares_indices)
    {
        const cv::Scalar xy = cv::mean(squares.contours[i]);
        square_centers_in_image.push_back(cv::Point2f(xy[0], xy[1]));
        const cv::Scalar uv = cv::mean(squares_in_ortho_view.contours[i]);
        square_centers_in_ortho_view.push_back(cv::Point2f(uv[0], uv[1]));
    }

    if (!canvas.empty())
    {
        cv::imshow("canvas_big_ortho_view", renderSquaresInBigOrthoView(
            image, ortho_view_from_image,
            squares_in_ortho_view,
            square_centers_in_image,
            square_centers_in_ortho_view,
            median_friendly_squares_indices));
    }

    if (median_friendly_squares_indices.empty())
    {
        LOG(WARNING) << "Found no median friendly squares";
        return cv::Mat();
    }

    auto get_x = [](const cv::Point2f& p){ return p.x; };
    auto get_y = [](const cv::Point2f& p){ return p.y; };
    const float min_x = pickSmallest(square_centers_in_ortho_view, get_x).x;
    const float max_x = pickLargest(square_centers_in_ortho_view, get_x).x;
    const float min_y = pickSmallest(square_centers_in_ortho_view, get_y).y;
    const float max_y = pickLargest(square_centers_in_ortho_view, get_y).y;

    const int num_cols = 6;
    const int num_rows = 4;

    std::vector<cv::Point2f> square_centers_col_row;
    for (const auto& center_in_ortho_view : square_centers_in_ortho_view)
    {
        square_centers_col_row.emplace_back(
            (center_in_ortho_view.x - min_x) / (max_x - min_x) * (num_cols - 1),
            (center_in_ortho_view.y - min_y) / (max_y - min_y) * (num_rows - 1));
    }

    if (!canvas.empty())
    {
        for (const auto i : indices(square_centers_in_image))
        {
            const cv::Point xy = square_centers_in_image[i];
            const auto& uv = square_centers_in_ortho_view[i];
            const auto& col_row = square_centers_col_row[i];
            std::vector<cv::Point2f> arrows_uv = {
                uv,
                uv + cv::Point2f(0.5f, 0.0f),
                uv + cv::Point2f(0.0f, 0.5f),
            };
            std::vector<cv::Point2f> arrows_xy(arrows_uv.size());
            cv::perspectiveTransform(arrows_uv, arrows_xy, ortho_view_from_image.inv(cv::DECOMP_SVD));
            cv::line(canvas, arrows_xy[0], arrows_xy[1], kRed);
            cv::line(canvas, arrows_xy[0], arrows_xy[2], kBrightGreen);
            cv::drawMarker(canvas, xy, kWhite, cv::MARKER_SQUARE, 8);
            cv::drawMarker(canvas, arrows_xy[0], kWhite, cv::MARKER_TILTED_CROSS, 8);
            cv::putText(canvas, strprintf("(%.0f, %.0f)", col_row.x, col_row.y),
                xy - cv::Point(20, 10), cv::FONT_HERSHEY_PLAIN, .75, kWhite);
        }
    }

    // Fit a multivariate polynomial to get a function from row,col to image x,y.
    const std::vector<cv::Point2f> square_row_cols = map(square_centers_col_row,
        [](const cv::Point2f& p)
    {
        return cv::Point2f(std::round(p.y), std::round(p.x));
    });
    const cv::Mat1f polynomial_row_col_terms_to_image_xy =
        fit2dPolynomial(square_row_cols, square_centers_in_image);

    cv::Mat3b ordered_colors(num_rows, num_cols);
    for (int row : irange(num_rows))
    {
        for (int col : irange(num_cols))
        {
            cv::Mat1f A_row(1, 6);
            A_row << 1, row, col, row * row, col * col, row * col;
            cv::Mat1f xy = A_row * polynomial_row_col_terms_to_image_xy;
            cv::Point center(std::round(xy(0)), std::round(xy(1)));
            if (!isInsideImage(center, image))
            {
                LOG(WARNING) << "Predicted square center is outside the image.";
                return cv::Mat();
            }
            cv::Vec3b color = image.at<cv::Vec3b>(center);
            ordered_colors(row, col) = color;

            if (!canvas.empty())
            {
                cv::circle(canvas, center, 1, kWhite, -1);
                cv::circle(canvas, center, 6, kWhite, 1);
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

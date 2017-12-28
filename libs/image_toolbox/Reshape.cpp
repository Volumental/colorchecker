#include "Reshape.hpp"

#include <algorithm>
#include <limits>
#include <vector>

#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/opencv.hpp>

namespace komb {

cv::Mat reshapeTiles(const cv::Mat& src, int cell_width, int cell_height, int tiling_hint,
    int border, const cv::Scalar& color)
{
    CHECK_GT(cell_width, 0);
    CHECK_GT(cell_height, 0);

    CHECK_EQ(src.cols % cell_width, 0);
    CHECK_EQ(src.rows % cell_height, 0);

    int src_cell_cols = src.cols / cell_width;
    int src_cell_rows = src.rows / cell_height;
    int num_tiles = src_cell_cols * src_cell_rows;

    // Decide shape of output
    int tiling_rows;
    int tiling_cols;
    if (tiling_hint == 0)
    {
        tiling_cols = ceil(sqrt(num_tiles));
        tiling_rows = (num_tiles + tiling_cols - 1) / tiling_cols;
    }
    else if (tiling_hint > 0)
    {
        tiling_cols = tiling_hint;
        tiling_rows = (num_tiles + tiling_cols - 1) / tiling_cols;
    }
    else  // tiling_hint < 0
    {
        tiling_rows = -tiling_hint;
        tiling_cols = (num_tiles + tiling_rows - 1) / tiling_rows;
    }

    CHECK_GE(tiling_cols * tiling_rows, src_cell_cols * src_cell_rows);

    cv::Mat dst(cell_height * tiling_rows + border * (tiling_rows + 1),
        cell_width * tiling_cols + border * (tiling_cols + 1), src.type());
    dst.setTo(color);
    CHECK_GE(dst.cols * dst.rows, src.cols * src.rows);

    for (int i = 0; i < num_tiles; ++i)
    {
        int src_x = i % src_cell_cols;
        int src_y = i / src_cell_cols;
        int dst_x = i % tiling_cols;
        int dst_y = i / tiling_cols;

        cv::Rect src_rect(src_x * cell_width, src_y * cell_height, cell_width, cell_height);
        cv::Rect dst_rect(border + dst_x * (cell_width + border),
            border + dst_y * (cell_height + border), cell_width, cell_height);

        cv::Mat(src, src_rect).copyTo(cv::Mat(dst, dst_rect));
    }
    return dst;
}

cv::Mat reshapeAndTile(const cv::Mat& src, int cell_width, int cell_height, int tiling_hint,
    int border, const cv::Scalar& color)
{
    return reshapeTiles(src.reshape(1, src.rows * cell_height), cell_width, cell_height,
        tiling_hint, border, color);
}

}  // namespace komb

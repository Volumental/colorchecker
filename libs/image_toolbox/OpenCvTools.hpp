#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include <opencv2/core/mat.hpp>
#include <Eigen/Core>

#include <common/algorithm/Container.hpp>
#include <common/ArrayPointer.hpp>
#include <common/Logging.hpp>
#include <geometry_toolbox/Types.hpp>

#include "Image.hpp"

namespace komb {

namespace image {

template<typename T>
size_t width(const cv::Mat_<T>& image)
{
    return static_cast<size_t>(image.cols);
}

template<typename T>
size_t height(const cv::Mat_<T>& image)
{
    return static_cast<size_t>(image.rows);
}

template<typename T>
size_t size(const cv::Mat_<T>& image)
{
    return static_cast<size_t>(image.total());
}

template<typename T>
const T* data(const cv::Mat_<T>& image)
{
    CHECK(image.isContinuous());
    return image.template ptr<const T>();
}

template<typename T>
T* data(cv::Mat_<T>& image)
{
    CHECK(image.isContinuous());
    return image.template ptr<T>();
}

template<typename T>
std::array<size_t, 2> dimensions(const cv::Mat_<T>& image)
{
    return {static_cast<size_t>(image.cols), static_cast<size_t>(image.rows)};
}

inline size_t width(const cv::Mat& image)
{
    return static_cast<size_t>(image.cols);
}

inline size_t height(const cv::Mat& image)
{
    return static_cast<size_t>(image.rows);
}

inline size_t size(const cv::Mat& image)
{
    return static_cast<size_t>(image.total());
}

inline std::array<size_t, 2> dimensions(const cv::Mat& image)
{
    return {static_cast<size_t>(image.cols), static_cast<size_t>(image.rows)};
}

template<typename Image>
int widthInt(const Image& i)
{
    return static_cast<int>(image::width(i));
}

template<typename Image>
int heightInt(const Image& i)
{
    return static_cast<int>(image::height(i));
}

} // namespace image

template<typename T, int R>
cv::Mat_<T> vectorOfVectorToMat(const AlignedVector<Eigen::Matrix<T, R, 1>>& v)
{
    size_t num_vecs = v.size();
    cv::Mat_<T> out_mat = cv::Mat_<T>::ones(num_vecs, R);

    for (size_t i = 0; i < num_vecs; ++i)
    {
        const Eigen::Matrix<T, R, 1>& vec = v[i];
        for (size_t j = 0; j < R; ++j)
        {
            out_mat(i, j) = vec(j);
        }
    }
    return out_mat;
}

template<typename Tmat, typename Teig, int R>
AlignedVector<Eigen::Matrix<Teig, R, 1>> matToVectorOfVector(const cv::Mat& mat)
{
    size_t num_points = static_cast<size_t>(mat.rows);
    AlignedVector<Eigen::Matrix<Teig, R, 1>> vec_of_vecs_eig(num_points);

    for (size_t i = 0; i < num_points; ++i)
    {
        for (size_t j = 0; j < R; ++j)
        {
            vec_of_vecs_eig[i](j) = Teig(mat.at<Tmat>(i, j));
        }
    }
    return vec_of_vecs_eig;
}

template<typename T>
ArrayPointer<T, 2> makeArrayPointer(cv::Mat_<T>& mat)
{
    using namespace image;
    return ArrayPointer<T, 2>(data(mat), width(mat), height(mat));
}

template<typename T>
ArrayPointer<const T, 2> makeArrayPointer(const cv::Mat_<T>& mat)
{
    using namespace image;
    return ArrayPointer<const T, 2>(data(mat), width(mat), height(mat));
}

template<typename T>
ArrayPointer<const T, 2> makeConstArrayPointer(const cv::Mat_<T>& mat)
{
    using namespace image;
    return ArrayPointer<const T, 2>(data(mat), width(mat), height(mat));
}

template<typename T>
cv::Mat_<T> makeCvMatPointer(ArrayPointer<T, 2> array)
{
    using namespace image;
    return cv::Mat_<T>(heightInt(array), widthInt(array), data(array));
}

template<typename T>
const cv::Mat_<T> makeCvMatConstPointer(ArrayPointer<const T, 2> array)
{
    const auto height = image::heightInt(array);
    const auto width  = image::widthInt(array);
    const auto data   = const_cast<T*>(array.data());
    return cv::Mat_<T>(height, width, data);
}

template<typename T>
cv::Mat_<T> makeCvMatPointer(Image<T>& i)
{
    using namespace image;
    return cv::Mat_<T>(heightInt(i), widthInt(i), data(i));
}

template<typename T>
const cv::Mat_<T> makeCvMatConstPointer(const Image<T>& i)
{
    const auto height = image::heightInt(i);
    const auto width  = image::widthInt(i);
    const auto data   = const_cast<T*>(i.data());
    return cv::Mat_<T>(height, width, data);
}

template <typename ImageIn>
auto makeCvMat(const ImageIn& in)
{
    using value_type  = typename ImageIn::value_type;
    const auto width  = image::widthInt(in);
    const auto height = image::heightInt(in);
    auto out          = cv::Mat_<value_type>(height, width);
    copy(in, out);
    return out;
}

template <typename ImageIn, typename UnaryOperation>
auto makeCvMat(const ImageIn& in, UnaryOperation op)
{
    const auto width     = image::widthInt(in);
    const auto height    = image::heightInt(in);
    auto out             = cv::Mat_<decltype(op(*in.begin()))>(height, width);
    transform(in, out, op);
    return out;
}

template <typename ImageIn1, typename ImageIn2, typename BinaryOperation>
auto makeCvMat(const ImageIn1& in1, const ImageIn2& in2, BinaryOperation op)
{
    CHECK_EQ(image::dimensions(in1), image::dimensions(in2));
    using value_type_out = decltype(op(*in1.begin(), *in2.begin()));

    const auto width  = image::widthInt(in1);
    const auto height = image::heightInt(in1);

    auto image_out = cv::Mat_<value_type_out>(height, width);
    transform(in1, in2, image_out, op);
    return image_out;
}

template <typename T, typename UnaryOperation>
auto map(const cv::Mat_<T>& in, UnaryOperation op)
{
    return makeCvMat(in, op);
}

template <typename T1, typename T2, typename BinaryOperation>
auto map(const cv::Mat_<T1>& in1, const cv::Mat_<T2>& in2, BinaryOperation op)
{
    return makeCvMat(in1, in2, op);
}

// TODO(mabur): figure out why this overload is needed. Should work without it.
template <typename T>
Image<T> makeImage(const cv::Mat_<T>& in)
{
    return Image<T>(image::width(in), image::height(in), image::data(in));
}

inline Imagef makeImageFloat(const cv::Mat& in)
{
    CHECK_EQ(in.type(), CV_32FC1);
    CHECK_F(in.isContinuous());
    return Imagef(image::width(in), image::height(in), reinterpret_cast<float*>(in.data));
}

template <typename InType, typename UnaryOperation>
auto mapImageFromCv(const cv::Mat_<InType>& in, UnaryOperation op)
{
    using value_type_out = decltype(op(*in.begin()));
    const auto width  = image::width(in);
    const auto height = image::height(in);
    auto out = Image<value_type_out>(width, height);
    transform(in, out, op);
    return out;
}

Imageb maskFromCvMat(const cv::Mat& cv_image);
Imageb imageMaskFromCv(const cv::Mat1b& cv_mask);
cv::Mat1b cvMaskFromImage(const Imageb& image_mask);

inline cv::Point2f makeCvPoint2f(const Vector2f& p)
{
    return {p.x(), p.y()};
}

inline cv::Point3f makeCvPoint3f(const Vector3f& p)
{
    return {p.x(), p.y(), p.z()};
}

inline Vector2f makeEigenVector2f(const cv::Point2f& p)
{
    return {p.x, p.y};
}

inline Vector3f makeEigenVector3f(const cv::Point3f& p)
{
    return {p.x, p.y, p.z};
}

} // namespace komb

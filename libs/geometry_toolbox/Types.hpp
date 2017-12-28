#pragma once

#include <vector>

#include <Eigen/Geometry>

namespace komb {

using Eigen::Vector2i;
using Eigen::Vector2f;
using Eigen::Vector2d;
using Eigen::Vector3i;
using Eigen::Vector3f;
using Eigen::Vector3d;
using Eigen::Vector4i;
using Eigen::Vector4f;
using Eigen::Vector4d;

using Eigen::Matrix2i;
using Eigen::Matrix2f;
using Eigen::Matrix2d;
using Eigen::Matrix3i;
using Eigen::Matrix3f;
using Eigen::Matrix3d;
using Eigen::Matrix4i;
using Eigen::Matrix4f;
using Eigen::Matrix4d;

using Eigen::Affine2f;
using Eigen::Affine2d;
using Eigen::Affine3f;
using Eigen::Affine3d;

template<typename T> using VectorX = Eigen::Matrix<T, Eigen::Dynamic, 1>;
template<typename T> using MatrixX = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>;
template<typename T> using Vector2 = Eigen::Matrix<T, 2, 1>;
template<typename T> using Vector3 = Eigen::Matrix<T, 3, 1>;
template<typename T> using Vector4 = Eigen::Matrix<T, 4, 1>;
template<typename T> using Matrix2 = Eigen::Matrix<T, 2, 2>;
template<typename T> using Matrix3 = Eigen::Matrix<T, 3, 3>;
template<typename T> using Matrix4 = Eigen::Matrix<T, 4, 4>;
template<typename T> using Affine2 = Eigen::Transform<T, 2, Eigen::Affine>;
template<typename T> using Affine3 = Eigen::Transform<T, 3, Eigen::Affine>;

template<typename T, int Rows>
using Vector = Eigen::Matrix<T, Rows, 1>;

template<typename T>
using AlignedVector = std::vector<T, Eigen::aligned_allocator<T>>;

template<typename T, int Rows>
using VectorOfVector = AlignedVector<Eigen::Matrix<T, Rows, 1>>;

template<typename T> using VectorOfVector2 = AlignedVector<Vector2<T>>;
template<typename T> using VectorOfVector3 = AlignedVector<Vector3<T>>;
template<typename T> using VectorOfVector4 = AlignedVector<Vector4<T>>;
template<typename T> using VectorOfAffine2 = AlignedVector<Affine2<T>>;
template<typename T> using VectorOfAffine3 = AlignedVector<Affine3<T>>;
template<typename T> using VectorOfMatrix2 = AlignedVector<Matrix2<T>>;
template<typename T> using VectorOfMatrix3 = AlignedVector<Matrix3<T>>;
template<typename T> using VectorOfMatrix4 = AlignedVector<Matrix4<T>>;

using VectorOfVector2i = VectorOfVector2<int>;
using VectorOfVector2f = VectorOfVector2<float>;
using VectorOfVector2d = VectorOfVector2<double>;
using VectorOfVector3i = VectorOfVector3<int>;
using VectorOfVector3f = VectorOfVector3<float>;
using VectorOfVector3d = VectorOfVector3<double>;
using VectorOfVector4i = VectorOfVector4<int>;
using VectorOfVector4f = VectorOfVector4<float>;
using VectorOfVector4d = VectorOfVector4<double>;

using VectorOfAffine2f = VectorOfAffine2<float>;
using VectorOfAffine2d = VectorOfAffine2<double>;
using VectorOfAffine3f = VectorOfAffine3<float>;
using VectorOfAffine3d = VectorOfAffine3<double>;

using VectorOfMatrix2f = VectorOfMatrix2<float>;
using VectorOfMatrix2d = VectorOfMatrix2<double>;
using VectorOfMatrix3f = VectorOfMatrix3<float>;
using VectorOfMatrix3d = VectorOfMatrix3<double>;
using VectorOfMatrix4f = VectorOfMatrix4<float>;
using VectorOfMatrix4d = VectorOfMatrix4<double>;

} // namespace komb

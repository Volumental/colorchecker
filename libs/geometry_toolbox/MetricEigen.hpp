#pragma once

#include <Eigen/Geometry>

namespace komb {

template<typename T, int N>
T norm(const Eigen::Matrix<T, N, 1>& p)
{
    return p.norm();
}

template<typename T, int N>
T distance(const Eigen::Matrix<T, N, 1>& a, const Eigen::Matrix<T, N, 1>& b)
{
    return (a - b).norm();
}

template<typename T, int N>
T squaredNorm(const Eigen::Matrix<T, N, 1>& p)
{
    return p.squaredNorm(p);
}

template<typename T, int N>
T squaredDistance(const Eigen::Matrix<T, N, 1>& a, const Eigen::Matrix<T, N, 1>& b)
{
    return (a - b).squaredNorm();
}

template<typename T, int N>
void normalize(Eigen::Matrix<T, N, 1>& io_p)
{
    if (io_p != Eigen::Matrix<T, N, 1>(0, 0))
    {
        io_p /= norm(io_p);
    }
}

template<typename T, int N>
Eigen::Matrix<T, N, 1> normalized(const Eigen::Matrix<T, N, 1>& p)
{
    if (p == Eigen::Matrix<T, N, 1>(0, 0))
    {
        return p;
    }
    return p / norm(p);
}

} // namespace komb

// Copyright (c) 2021, Justin Kim

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#include <Sapphire/compute/ActivationOps.hpp>
#include <Sapphire/compute/dense/cuda/Activation.cuh>
#include <Sapphire/compute/dense/naive/NaiveBasic.hpp>
#include <Sapphire/compute/dense/cuda/Basic.cuh>

namespace Sapphire::Compute
{
void SoftMax(TensorData& y, const TensorData& x)
{
    assert(y.Mode() == x.Mode());
    assert(y.GetDevice() == x.GetDevice());
    const auto device = y.GetDevice();
    const auto N = y.Cols();
    const auto paddedN = y.PaddedHostColSize;
    const auto unitSize = y.TensorShape.Cols();
    const auto totalSize = y.TensorShape.Size();
    const auto totalSizeWithPadding = (totalSize / N) * paddedN;

    if (y.Mode() == DeviceType::Cuda)
    {
        cudaSetDevice(device.GetID());
        Dense::Cuda::SoftMax(y.GetMutableDenseCuda(), x.GetDenseCuda(),
                             totalSize,
                             unitSize);
    }
    else
    {
        Dense::Naive::Softmax(y.GetMutableDenseHost(), x.GetDenseHost(),
                              totalSizeWithPadding, unitSize, paddedN);
    }
}

void LeakyReLU(TensorData& y, const TensorData& x, float a)
{
    assert(y.Mode() == x.Mode());
    assert(y.GetDevice() == x.GetDevice());
    const auto device = y.GetDevice();
    const auto N = y.Cols();
    const auto paddedN = y.PaddedHostColSize;
    const auto totalSize = y.TensorShape.Size();
    const auto totalSizeWithPadding = (totalSize / N) * paddedN;

    if (y.Mode() == DeviceType::Cuda)
    {
        cudaSetDevice(device.GetID());
        Dense::Cuda::LeakyReLU(y.GetMutableDenseCuda(), x.GetDenseCuda(), a,
                               totalSize);
    }
    else
    {
        Dense::Naive::LeakyReLU(y.GetMutableDenseHost(), x.GetDenseHost(), a,
                                totalSizeWithPadding, N, paddedN);
    }
}

void ReLU(TensorData& y, const TensorData& x)
{
    assert(y.Mode() == x.Mode());
    assert(y.GetDevice() == x.GetDevice());
    const auto device = y.GetDevice();
    const auto N = y.Cols();
    const auto paddedN = y.PaddedHostColSize;
    const auto totalSize = y.TensorShape.Size();
    const auto totalSizeWithPadding = (totalSize / N) * paddedN;

    if (y.Mode() == DeviceType::Cuda)
    {
        cudaSetDevice(device.GetID());
        Dense::Cuda::ReLU(y.GetMutableDenseCuda(), x.GetDenseCuda(), totalSize);
    }
    else
    {
        Dense::Naive::ReLU(y.GetMutableDenseHost(), x.GetDenseHost(),
                           totalSizeWithPadding, N, paddedN);
    }
}

void ReLUBackward(TensorData& dx, const TensorData& dy, const TensorData& x)
{
    assert(dx.Mode() == dy.Mode() && dx.Mode() == x.Mode());
    assert(dx.GetDevice() == dy.GetDevice() &&
        dx.GetDevice() == x.GetDevice());
    const auto device = dx.GetDevice();
    const auto paddedColSize = dx.PaddedHostColSize;
    const auto totalSize = dx.TensorShape.Size();

    if (dx.Mode() == DeviceType::Cuda)
    {
        cudaSetDevice(device.GetID());
        Dense::Cuda::ReLUBackward(dx.GetMutableDenseCuda(), dy.GetDenseCuda(),
                                  x.GetDenseCuda(), totalSize);
    }
    else
    {
        Dense::Naive::ReLUBackward(dx.GetMutableDenseHost(), dy.GetDenseHost(),
                                   x.GetDenseHost(), totalSize, dx.Cols(),
                                   paddedColSize);
    }
}

void LeakyReLUBackward(TensorData& dx, const TensorData& dy,
                       const TensorData& x, float a)
{
    const auto device = dx.GetDevice();
    const auto totalSize = dx.TensorShape.Size();

    if (dx.Mode() == DeviceType::Cuda)
    {
        cudaSetDevice(device.GetID());
        Dense::Cuda::LeakyReLUBackward(dx.GetMutableDenseCuda(),
                                       dy.GetDenseCuda(),
                                       x.GetDenseCuda(),
                                       a, totalSize);
    }
    else
    {
        throw std::runtime_error(
            "Compute::LeakyReLUBackward - Host not implemented");
    }
}
}

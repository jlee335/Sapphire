// Copyright (c) 2021, Justin Kim

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#include <Sapphire/compute/TrigonometricOps.hpp>
#include <Sapphire/compute/dense/cuda/Trigonometric.cuh>
#include <Sapphire/compute/dense/naive/NaiveBasic.hpp>

namespace Sapphire::Compute
{
void Cos(TensorData& y, const TensorData& x)
{
    const auto device = y.GetDevice();
    const auto N = y.Cols();
    const auto paddedN = y.PaddedHostColSize;
    const auto totalSize = y.TensorShape.Size() * y.BatchSize;
    const auto totalSizeWithPadding = (totalSize / N) * paddedN;

    if (device.Type() == DeviceType::CUDA)
    {
        Dense::Cuda::Cos(y.GetMutableDenseCuda(), x.GetDenseCuda(), totalSize);
    }
    else
    {
        Dense::Naive::cos(y.GetMutableDenseHost(), x.GetDenseHost(),
                          totalSizeWithPadding);
    }
}

void Sin(TensorData& y, const TensorData& x)
{
    const auto device = y.GetDevice();
    const auto N = y.Cols();
    const auto paddedN = y.PaddedHostColSize;
    const auto totalSize = y.TensorShape.Size() * y.BatchSize;
    const auto totalSizeWithPadding = (totalSize / N) * paddedN;

    if (device.Type() == DeviceType::CUDA)
    {
        Dense::Cuda::Sin(y.GetMutableDenseCuda(), x.GetDenseCuda(), totalSize);
    }
    else
    {
        Dense::Naive::sin(y.GetMutableDenseHost(), x.GetDenseHost(),
                          totalSizeWithPadding);
    }
}

void Tan(TensorData& y, const TensorData& x)
{
    const auto device = y.GetDevice();
    const auto N = y.Cols();
    const auto paddedN = y.PaddedHostColSize;
    const auto totalSize = y.TensorShape.Size() * y.BatchSize;
    const auto totalSizeWithPadding = (totalSize / N) * paddedN;

    if (device.Type() == DeviceType::CUDA)
    {
        Dense::Cuda::Tan(y.GetMutableDenseCuda(), x.GetDenseCuda(), totalSize);
    }
    else
    {
        Dense::Naive::tan(y.GetMutableDenseHost(), x.GetDenseHost(),
                          totalSizeWithPadding);
    }
}

void Cosh(TensorData& y, const TensorData& x)
{
    const auto device = y.GetDevice();
    const auto N = y.Cols();
    const auto paddedN = y.PaddedHostColSize;
    const auto totalSize = y.TensorShape.Size() * y.BatchSize;
    const auto totalSizeWithPadding = (totalSize / N) * paddedN;

    if (device.Type() == DeviceType::CUDA)
    {
        Dense::Cuda::Cosh(y.GetMutableDenseCuda(), x.GetDenseCuda(), totalSize);
    }
    else
    {
        Dense::Naive::cosh(y.GetMutableDenseHost(), x.GetDenseHost(),
                           totalSizeWithPadding);
    }
}

void Sinh(TensorData& y, const TensorData& x)
{
    const auto device = y.GetDevice();
    const auto N = y.Cols();
    const auto paddedN = y.PaddedHostColSize;
    const auto totalSize = y.TensorShape.Size() * y.BatchSize;
    const auto totalSizeWithPadding = (totalSize / N) * paddedN;

    if (device.Type() == DeviceType::CUDA)
    {
        Dense::Cuda::Sinh(y.GetMutableDenseCuda(), x.GetDenseCuda(), totalSize);
    }
    else
    {
        Dense::Naive::sinh(y.GetMutableDenseHost(), x.GetDenseHost(),
                           totalSizeWithPadding);
    }
}

void Tanh(TensorData& y, const TensorData& x)
{
    const auto device = y.GetDevice();
    const auto N = y.Cols();
    const auto paddedN = y.PaddedHostColSize;
    const auto totalSize = y.TensorShape.Size() * y.BatchSize;
    const auto totalSizeWithPadding = (totalSize / N) * paddedN;

    if (device.Type() == DeviceType::CUDA)
    {
        Dense::Cuda::Tanh(y.GetMutableDenseCuda(), x.GetDenseCuda(), totalSize);
    }
    else
    {
        Dense::Naive::tanh(y.GetMutableDenseHost(), x.GetDenseHost(),
                           totalSizeWithPadding);
    }
}

void ArcCos(TensorData& y, const TensorData& x)
{
    const auto device = y.GetDevice();
    const auto totalSize = y.TensorShape.Size() * y.BatchSize;

    if (device.Type() == DeviceType::CUDA)
    {
        Dense::Cuda::ArcCos(y.GetMutableDenseCuda(), x.GetDenseCuda(),
                            totalSize);
    }
    else
    {
        throw std::runtime_error("Compute::ArcCos - Host not implemented");
    }
}

void Arcsin(TensorData& y, const TensorData& x)
{
    const auto device = y.GetDevice();
    const auto totalSize = y.TensorShape.Size() * y.BatchSize;

    if (device.Type() == DeviceType::CUDA)
    {
        Dense::Cuda::ArcSin(y.GetMutableDenseCuda(), x.GetDenseCuda(),
                            totalSize);
    }
    else
    {
        throw std::runtime_error("Compute::ArcSin - Host not implemented");
    }
}


void ArcTan(TensorData& y, const TensorData& x)
{
    const auto device = y.GetDevice();
    const auto totalSize = y.TensorShape.Size() * y.BatchSize;

    if (device.Type() == DeviceType::CUDA)
    {
        Dense::Cuda::ArcTan(y.GetMutableDenseCuda(), x.GetDenseCuda(),
                            totalSize);
    }
    else
    {
        throw std::runtime_error("Compute::ArcTan - Host not implemented");
    }
}

void ArcCosh(TensorData& y, const TensorData& x)
{
    const auto device = y.GetDevice();
    const auto totalSize = y.TensorShape.Size() * y.BatchSize;
    if (device.Type() == DeviceType::CUDA)
    {
        Dense::Cuda::ArcCosh(y.GetMutableDenseCuda(), x.GetDenseCuda(),
                             totalSize);
    }
    else
    {
        throw std::runtime_error("Compute::ArcCosh - Host not implemented");
    }
}

void ArcSinh(TensorData& y, const TensorData& x)
{
    const auto device = y.GetDevice();
    const auto totalSize = y.TensorShape.Size() * y.BatchSize;

    if (device.Type() == DeviceType::CUDA)
    {
        Dense::Cuda::ArcSinh(y.GetMutableDenseCuda(), x.GetDenseCuda(),
                             totalSize);
    }
    else
    {
        throw std::runtime_error("Compute::ArcSinh - Host not implemented");
    }
}

void ArcTanh(TensorData& y, const TensorData& x)
{
    const auto device = y.GetDevice();
    const auto totalSize = y.TensorShape.Size() * y.BatchSize;

    if (device.Type() == DeviceType::CUDA)
    {
        Dense::Cuda::ArcTanh(y.GetMutableDenseCuda(), x.GetDenseCuda(),
                             totalSize);
    }
    else
    {
        throw std::runtime_error("Compute::ArcTanh - Host not implemented");
    }
}

void CosBackward(TensorData& dx, const TensorData& dy, const TensorData& x)
{
    const auto device = dx.GetDevice();
    const auto totalSize = dx.TensorShape.Size() * dx.BatchSize;

    if (device.Type() == DeviceType::CUDA)
    {
        Dense::Cuda::CosBackward(dx.GetMutableDenseCuda(), dy.GetDenseCuda(),
                                 x.GetDenseCuda(), totalSize);
    }
    else
    {
        throw std::runtime_error("Compute::CosBackward - Host not implemented");
    }
}

void SinBackward(TensorData& dx, const TensorData& dy, const TensorData& x)
{
    const auto device = dx.GetDevice();
    const auto totalSize = dx.TensorShape.Size() * dx.BatchSize;

    if (device.Type() == DeviceType::CUDA)
    {
        Dense::Cuda::SinBackward(dx.GetMutableDenseCuda(), dy.GetDenseCuda(),
                                 x.GetDenseCuda(), totalSize);
    }
    else
    {
        throw std::runtime_error("Compute::SinBackward - Host not implemented");
    }
}

void TanBackward(TensorData& dx, const TensorData& dy, const TensorData& x)
{
    const auto device = dx.GetDevice();
    const auto totalSize = dx.TensorShape.Size() * dx.BatchSize;

    if (device.Type() == DeviceType::CUDA)
    {
        Dense::Cuda::TanBackward(dx.GetMutableDenseCuda(), dy.GetDenseCuda(),
                                 x.GetDenseCuda(), totalSize);
    }
    else
    {
        throw std::runtime_error("Compute::TanBackward - Host not implemented");
    }
}

void CoshBackward(TensorData& dx, const TensorData& dy, const TensorData& x)
{
    const auto device = dx.GetDevice();
    const auto totalSize = dx.TensorShape.Size() * dx.BatchSize;

    if (device.Type() == DeviceType::CUDA)
    {
        Dense::Cuda::CoshBackward(dx.GetMutableDenseCuda(), dy.GetDenseCuda(),
                                  x.GetDenseCuda(), totalSize);
    }
    else
    {
        throw std::runtime_error(
            "Compute::CoshBackward - Host not implemented");
    }
}

void SinhBackward(TensorData& dx, const TensorData& dy, const TensorData& x)
{
    const auto device = dx.GetDevice();
    const auto totalSize = dx.TensorShape.Size() * dx.BatchSize;

    if (device.Type() == DeviceType::CUDA)
    {
        Dense::Cuda::SinhBackward(dx.GetMutableDenseCuda(), dy.GetDenseCuda(),
                                  x.GetDenseCuda(), totalSize);
    }
    else
    {
        throw std::runtime_error(
            "Compute::SinhBackward - Host not implemented");
    }
}

void TanhBackward(TensorData& dx, const TensorData& dy, const TensorData& x)
{
    const auto device = dx.GetDevice();
    const auto totalSize = dx.TensorShape.Size() * dx.BatchSize;

    if (device.Type() == DeviceType::CUDA)
    {
        Dense::Cuda::TanhBackward(dx.GetMutableDenseCuda(), dy.GetDenseCuda(),
                                  x.GetDenseCuda(), totalSize);
    }
    else
    {
        throw std::runtime_error(
            "Compute::TanhBackward - Host not implemented");
    }
}

void ArcCosBackward(TensorData& dx, const TensorData& dy, const TensorData& x)
{
    const auto device = dx.GetDevice();
    const auto totalSize = dx.TensorShape.Size() * dx.BatchSize;

    if (device.Type() == DeviceType::CUDA)
    {
        Dense::Cuda::ArcCosBackward(dx.GetMutableDenseCuda(), dy.GetDenseCuda(),
                                    x.GetDenseCuda(), totalSize);
    }
    else
    {
        throw std::runtime_error(
            "Compute::ArcCosBackward - Host not implemented");
    }
}

void ArcSinBackward(TensorData& dx, const TensorData& dy, const TensorData& x)
{
    const auto device = dx.GetDevice();
    const auto totalSize = dx.TensorShape.Size() * dx.BatchSize;

    if (device.Type() == DeviceType::CUDA)
    {
        Dense::Cuda::ArcSinBackward(dx.GetMutableDenseCuda(), dy.GetDenseCuda(),
                                    x.GetDenseCuda(), totalSize);
    }
    else
    {
        throw std::runtime_error(
            "Compute::ArcSinBackward - Host not implemented");
    }
}

void ArcTanBackward(TensorData& dx, const TensorData& dy, const TensorData& x)
{
    const auto device = dx.GetDevice();
    const auto totalSize = dx.TensorShape.Size() * dx.BatchSize;

    if (device.Type() == DeviceType::CUDA)
    {
        Dense::Cuda::ArcTanBackward(dx.GetMutableDenseCuda(), dy.GetDenseCuda(),
                                    x.GetDenseCuda(), totalSize);
    }
    else
    {
        throw std::runtime_error(
            "Compute::ArcTanBackward- Host not implemented");
    }
}

void ArcCoshBackward(TensorData& dx, const TensorData& dy, const TensorData& x)
{
    const auto device = dx.GetDevice();
    const auto totalSize = dx.TensorShape.Size() * dx.BatchSize;

    if (device.Type() == DeviceType::CUDA)
    {
        Dense::Cuda::ArcCoshBackward(dx.GetMutableDenseCuda(),
                                     dy.GetDenseCuda(),
                                     x.GetDenseCuda(), totalSize);
    }
    else
    {
        throw std::runtime_error(
            "Compute::ArcCoshBackward - Host not implemented");
    }
}

void ArcSinhBackward(TensorData& dx, const TensorData& dy, const TensorData& x)
{
    const auto device = dx.GetDevice();
    const auto totalSize = dx.TensorShape.Size() * dx.BatchSize;

    if (device.Type() == DeviceType::CUDA)
    {
        Dense::Cuda::ArcSinhBackward(dx.GetMutableDenseCuda(),
                                     dy.GetDenseCuda(),
                                     x.GetDenseCuda(), totalSize);
    }
    else
    {
        throw std::runtime_error(
            "Compute::ArcSinhBackward - Host not implemented");
    }
}

void ArcTanhBackward(TensorData& dx, const TensorData& dy, const TensorData& x)
{
    const auto device = dx.GetDevice();
    const auto totalSize = dx.TensorShape.Size() * dx.BatchSize;

    if (device.Type() == DeviceType::CUDA)
    {
        Dense::Cuda::ArcTanhBackward(dx.GetMutableDenseCuda(),
                                     dy.GetDenseCuda(),
                                     x.GetDenseCuda(), totalSize);
    }
    else
    {
        throw std::runtime_error(
            "Compute::ArcTanhBackward - Host not implemented");
    }
}
}

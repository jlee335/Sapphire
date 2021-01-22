// Copyright (c) 2020, Jaewoo Kim

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#ifndef MOTUTAPU_UTIL_TENSORDATA_HPP
#define MOTUTAPU_UTIL_TENSORDATA_HPP

#include <Motutapu/tensor/TensorDataDecl.hpp>
#include <Motutapu/compute/cuda/Memory.cuh>

namespace Motutapu::Util
{
template <typename T>
TensorData<T>* TensorData<T>::CreateTensorData(const Shape& shape,
                                               const Device& device,
                                               Type type, unsigned batchSize)
{
    auto* tensorData = new TensorData<T>(shape, type, device);
    if (device.Type() == DeviceType::CUDA)
    {
        Compute::Cuda::CudaSetDevice(device.GetID());
        tensorData->m_allocateGpu(batchSize);
        tensorData->m_allocateCpu(batchSize);
    }
    return tensorData;
}

template <typename T>
bool TensorData<T>::DestroyTensorData(TensorData<T>* tensorData)
{
    auto isSuccess = true;

    tensorData->m_freeCpu();

    if (tensorData->m_device.Type() == DeviceType::CUDA)
    {
        isSuccess &= tensorData->m_freeGpu();
    }

    delete tensorData;

    return isSuccess;
}

template <typename T>
void TensorData<T>::m_freeCpu() const
{
    if (m_type == Type::Sparse)
    {
        delete[] SparseMatHost;
    }
    else
    {
        delete[] DenseMatHost;
    }
}

template <typename T>
bool TensorData<T>::m_freeGpu()
{
    bool isSuccess = true;

    isSuccess &= Compute::Cuda::CudaSetDevice(m_device.GetID());

    if (m_type == Type::Sparse)
    {
        isSuccess &= Compute::Cuda::CudaFree<T>(SparseMatCuda);
    }
    else
    {
        isSuccess &= Compute::Cuda::CudaFree<T>(DenseMatCuda);
    }

    return isSuccess;
}

template <typename T>
void TensorData<T>::m_allocateCpu(unsigned int batchSize)
{
    const auto colSize = TensorShape.At(0);
    const auto rowSize = TensorShape.Dim() > 1 ? TensorShape.At(1) : 0;

    const auto padUnitSize = 32 / sizeof(T);

    const auto paddedColSize = colSize % padUnitSize == 0
                                   ? colSize
                                   : colSize / padUnitSize * padUnitSize +
                                     padUnitSize;

    const auto paddedRowSize = rowSize % padUnitSize == 0
                                   ? rowSize
                                   : rowSize / padUnitSize * padUnitSize +
                                     padUnitSize;

    if (m_type == Type::Sparse)
    {
        throw std::runtime_error("m_allocate - Sparse not implemented");
    }
    else
    {
        DenseTotalLength = batchSize * paddedRowSize * paddedColSize;
        DenseMatHost = new T[DenseTotalLength];
    }
}


template <typename T>
bool TensorData<T>::m_allocateCuda(unsigned int batchSize)
{
    const auto colSize = TensorShape.At(0);
    const auto rowSize = TensorShape.Dim() > 1 ? TensorShape.At(1) : 0;

    const auto padUnitSize = 32 / sizeof(T);

    const auto paddedColSize =
        colSize % padUnitSize == 0
            ? colSize
            : colSize / padUnitSize * padUnitSize + padUnitSize;

    const auto paddedRowSize =
        rowSize % padUnitSize == 0
            ? rowSize
            : rowSize / padUnitSize * padUnitSize + padUnitSize;

    auto isSuccess = true;

    if (m_device.Type() == DeviceType::CUDA)
    {
        isSuccess &= Compute::Cuda::CudaSetDevice(m_device.GetID());
        if (m_type == Type::Sparse)
        {
            throw std::runtime_error("m_allocate - Sparse not implemented");
        }
        else
        {
            isSuccess &= Compute::Cuda::CudaMalloc<T>(
                &DenseMatCuda, batchSize * paddedRowSize * paddedColSize);
        }
    }
    else
    {
        std::runtime_error("m_allocateCuda - Tensor Data type is not CUDA");
    }

    return isSuccess;
}

template <typename T>
void TensorData<T>::DenseToSparse(TensorData<T>* tensorData)
{
    throw std::exception("DenseToSparse not implemented");
}

template <typename T>
void TensorData<T>::SparseToDense(TensorData<T>* tensorData)
{
    throw std::exception("SparseToDense not implemented");
}

template <typename T>
bool TensorData<T>::CopyTensorData(TensorData<T>* dest,
                                   const TensorData<T>* src)
{
    if (src->GetDevice() != dest->GetDevice())
    {
        throw std::invalid_argument("Device mismatch while copying tensorData");
    }

    if (dest->TensorShape != src->TensorShape)
    {
        throw std::invalid_argument("Shape mismatch while copying tensorData");
    }

    if (dest->GetType() != src->GetType())
    {
        throw std::invalid_argument("Type mismatch while copying tensorData");
    }

    if (src->IsBusy || dest->IsBusy)
        return false;

    const bool sparse = src->GetType() == Type::Sparse ? true : false;
    bool success = true;
    auto device = src->GetDevice();

    src->IsBusy.exchange(true, std::memory_order_acquire);
    dest->IsBusy.exchange(true, std::memory_order_acquire);

    const Shape shape = dest->TensorShape;

    if (device.Type() == DeviceType::CPU)
    {
        if (sparse)
        {
            throw std::exception("CopyTensorData - sparse not implemented");
        }
        else
        {
            std::memcpy(dest->DenseMatHost, src->DenseMatHost,
                        src->DenseTotalLength * sizeof(T));
            dest->DenseTotalLength = src->DenseTotalLength;
        }
    }

    if (device.Type() == DeviceType::CUDA)
    {
        success &= Compute::Cuda::CudaSetDevice(device.GetID());

        if (sparse)
        {
            throw std::exception("CopyTensorData - sparse not implemented");
        }
        else
        {
            MemcpyGpuToGpu(static_cast<void**>(dest->DenseMatCuda),
                           static_cast<void**>(src->DenseMatCuda),
                           src->DenseTotalLength * sizeof(T));
            dest->DenseTotalLength = src->DenseTotalLength;
        }
    }

    src->IsBusy.exchange(true, std::memory_order_release);
    dest->IsBusy.exchange(true, std::memory_order_release);
    return success;
}

template <typename T>
bool TensorData<T>::ChangeDevice(TensorData<T>* tensorData, Device device)
{
    auto currentDevice = tensorData->GetDevice();
    if (currentDevice == device)
    {
        return false;
    }

    if (currentDevice.Type() == DeviceType::CPU && device.Type() ==
        DeviceType::CUDA)
    {
        tensorData->m_allocateCuda(tensorData->BatchSize);
        TensorData<T>::CopyHostToGpu(tensorData);
    }

    if (currentDevice.Type() == DeviceType::CUDA && device.Type() ==
        DeviceType::CPU)
    {
        TensorData<T>::CopyGpuToHost(tensorData);
    }

    if (currentDevice.Type() == DeviceType::CUDA &&
        device.Type() == DeviceType::CUDA)
    {
        TensorData<T>::CopyGpuToHost(tensorData);
        TensorData<T>::CopyHostToGpu(tensorData);
    }

    tensorData->m_device = device;

    return true;
}

template <typename T>
void TensorData<T>::CopyHostToGpu(TensorData<T>* tensorData)
{
    if (tensorData->GetDevice().Type() != DeviceType::CUDA)
    {
        throw std::invalid_argument(
            "CopyHostToGpu - Given tensor data is not GPU tensor");
    }

    if (tensorData->GetType() == Type::Sparse)
    {
        throw std::exception("Sparse matrix not implemented");
    }
    else
    {
        Compute::Cuda::CudaSetDevice(tensorData->m_device.GetID());
        MemcpyHostToGpu(tensorData->DenseMatCuda, tensorData->DenseMatHost,
                        tensorData->BatchSize);
    }
}

template <typename T>
void TensorData<T>::CopyGpuToHost(TensorData<T>* tensorData)
{
    if (tensorData->GetDevice().Type() != DeviceType::CUDA)
    {
        throw std::invalid_argument(
            "CopyHostToGpu - Given tensor data is not GPU tensor");
    }

    if (tensorData->GetType() == Type::Sparse)
    {
        throw std::exception("Sparse matrix not implemented");
    }
    else
    {
        Compute::Cuda::CudaSetDevice(tensorData->m_device.GetID());
        MemcpyGpuToHost(tensorData->DenseMatHost, tensorData->DenseMatCuda,
                        tensorData->BatchSize);
    }
}


template <typename T>
unsigned long TensorData<T>::m_convertDenseToSparse(
    SparseMatrix<T>* sparse, const T* dense,
    Shape shape, unsigned long paddedRowSize,
    Device device)
{
    throw std::exception("m_convertDenseToSparse not implemented");
}

template <typename T>
unsigned long TensorData<T>::m_convertSparseToDense(
    SparseMatrix<T>* sparse, const T* dense,
    Shape shape, unsigned long paddedRowSize,
    Device device)
{
    throw std::exception("m_convertSparseToDense not implemented");
}

template <typename T>
TensorData<T>::TensorData(Shape shape, Type type, Device device)
    : TensorShape(std::move(shape)),
      m_type(type),
      m_device(std::move(device))
{
}
} // namespace Motutapu::Util

#endif
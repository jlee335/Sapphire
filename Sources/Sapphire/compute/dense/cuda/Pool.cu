// Copyright (c) 2021, Justin Kim

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#include <Sapphire/compute/dense/cuda/Pool.cuh>
#include <Sapphire/util/ResourceManager.hpp>
#include <cudnn.h>

namespace Sapphire::Compute::Dense::Cuda
{
__host__ void CreateCudnnPoolMetaData(CudnnPool2DMetaData* metaData,
                                      Shape4D xShape, int windowHeight,
                                      int windowWidth, int strideRow,
                                      int strideCol, int rowPadding,
                                      int columnPadding,
                                      cudnnPoolingMode_t mode,
                                      cudnnNanPropagation_t nanPropagation)
{
    Shape4D outputShape = { 0, 0, 0, 0 };
    checkCuDNN(cudnnCreatePoolingDescriptor(&metaData->PoolDesc));
    checkCuDNN(cudnnCreateTensorDescriptor(&metaData->xDesc));
    checkCuDNN(cudnnCreateTensorDescriptor(&metaData->yDesc));
    checkCuDNN(cudnnSetTensor4dDescriptor(
        metaData->xDesc, CUDNN_TENSOR_NCHW, CUDNN_DATA_FLOAT, xShape.N,
        xShape.Channels, xShape.Height, xShape.Width));
    checkCuDNN(cudnnSetPooling2dDescriptor(
        metaData->PoolDesc, mode, nanPropagation, windowHeight, windowWidth,
        rowPadding, columnPadding, strideRow, strideCol));
    checkCuDNN(cudnnGetPooling2dForwardOutputDim(
        metaData->PoolDesc, metaData->xDesc, &outputShape.N,
        &outputShape.Channels, &outputShape.Height, &outputShape.Width));
    checkCuDNN(cudnnSetTensor4dDescriptor(
        metaData->yDesc, CUDNN_TENSOR_NCHW, CUDNN_DATA_FLOAT, outputShape.N,
        outputShape.Channels, outputShape.Height, outputShape.Width));
}

__host__ void CudnnPoolForward2d(CudnnPool2DMetaData* metaData, float* y,
                                 float* x,
                                 float* alpha, float* beta, int deviceId)
{
    cudnnHandle_t* handle = Util::ResourceManager::GetCudnnHandle(
        deviceId, std::this_thread::get_id());
    checkCuDNN(cudnnPoolingForward(*handle, metaData->PoolDesc, alpha,
                                   metaData->xDesc, x, beta, metaData->yDesc,
                                   y));
}

__host__ void CudnnPoolBackward2d(CudnnPool2DMetaData* metaData, float* y,
                                  float* dy,
                                  float* x, float* dx, float* alpha,
                                  float* beta, int deviceId)
{
    cudnnHandle_t* handle = Util::ResourceManager::GetCudnnHandle(
        deviceId, std::this_thread::get_id());
    checkCuDNN(cudnnPoolingBackward(
        *handle, metaData->PoolDesc, alpha, metaData->yDesc, y,
        metaData->dyDesc, dy, metaData->xDesc, x, beta, metaData->dxDesc, dx));
}

__host__ void PoolForward2d(float* y, float* x, Shape4D xShape,
                            int windowHeight, int windowWidth, int strideRow,
                            int strideCol, int rowPadding, int columnPadding,
                            cudnnPoolingMode_t mode,
                            cudnnNanPropagation_t nanPropagation, int deviceId)
{
    const PoolConfig poolConfig = { xShape, windowHeight, windowWidth,
                                    strideRow, strideCol, rowPadding,
                                    columnPadding };
    if (!Util::ResourceManager::HasPoolConfig(poolConfig))
    {
        auto* metaData = new CudnnPool2DMetaData();
        CreateCudnnPoolMetaData(metaData, xShape, windowHeight, windowWidth,
                                strideRow, strideCol, rowPadding, columnPadding,
                                mode, nanPropagation);

        Util::ResourceManager::AddCudnnPool2DMetaData(poolConfig, metaData);
    }

    float alpha = 1.0f, beta = 0.0f;
    auto* metaData = Util::ResourceManager::GetCudnnPoolMetaData(poolConfig);
    CudnnPoolForward2d(metaData, y, x, &alpha, &beta, deviceId);
}

__host__ void PoolBackward2d(float* y, float* dy, float* x, float* dx,
                             Shape4D xShape,
                             int windowHeight,
                             int windowWidth, int strideRow, int strideCol,
                             int rowPadding,
                             int columnPadding, int deviceId)
{
    const PoolConfig poolConfig = { xShape, windowHeight, windowWidth,
                                    strideRow, strideCol, rowPadding,
                                    columnPadding };

    if (!Util::ResourceManager::HasPoolConfig(poolConfig))
    {
        throw std::runtime_error(
            "Compute::Dense::Cuda::PoolBackward2d - CudnnPool2DMetaData was not "
            "found");
    }
    float alpha = 1.0f, beta = 0.0f;
    auto* metaData = Util::ResourceManager::GetCudnnPoolMetaData(poolConfig);
    CudnnPoolBackward2d(metaData, y, dy, x, dx, &alpha, &beta, deviceId);
}
}

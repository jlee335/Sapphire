// Copyright (c) 2021, Justin Kim

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#ifndef MOTUTAPU_UTIL_TENSORDATA_DECL_HPP
#define MOTUTAPU_UTIL_TENSORDATA_DECL_HPP

#include <Motutapu/util/Device.hpp>
#include <Motutapu/util/SparseMatrix.hpp>
#include <Motutapu/tensor/Shape.hpp>

namespace Motutapu::Util
{
class TensorData
{
public:
    TensorData() = default;
    TensorData(Shape shape, Type type, Device device, unsigned int batchSize);


    TensorData(const TensorData& tensorData) = default;
    TensorData& operator=(const TensorData& tensorData) = default;

    ~TensorData();

    unsigned long DenseTotalLength = 0;
    unsigned long SparseTotalLength = 0;
    unsigned long PaddedColumnSize = 0;
    unsigned long PaddedRowSize = 0;
    unsigned long BatchSize = 0;

    float* DenseMatHost = nullptr;
    float* DenseMatCuda = nullptr;

    SparseMatrix* SparseMatHost = nullptr;
    SparseMatrix* SparseMatCuda = nullptr;
    Shape TensorShape;

    //! Gets device descriptor (Sparse or Dense)
    //! \return : device descriptor
    [[nodiscard]] const Device& GetDevice() const
    {
        return m_device;
    }

    //! Gets type of the data (Sparse of Dense)
    //! \return : Type of the data
    [[nodiscard]] Type GetType() const
    {
        return m_type;
    }

    [[nodiscard]] Shape GetShape() const
    {
        return TensorShape;
    }

    //! Helper static functions
    //! These helper functions are used to control the tensorData from the
    //! operation units

    //! Converts tensor data from dense to sparse
    static void DenseToSparse(TensorData tensorData);
    //! Converts tensor data  from sparse to dense
    static void SparseToDense(TensorData tensorData);

    //! Deep copies tensor data from src to dest
    //! Type of dest and src must be the same
    static bool CopyTensorData(TensorData dest,
                               const TensorData src);

    //! Changes device of the tensor
    //! Transfers data to target device from current device
    //! immediately returns false if change device is requested to same device
    //! \param tensorData : tensorData object to change device
    //! \param device : new device to set
    static bool ChangeDevice(TensorData tensorData, Device device);

    //! Copies data on the Host to Gpu
    //! Only available for CUDA tensors
    static void CopyHostToGpu(TensorData tensorData);

    //! Copies data on the Host to Gpu
    //! Only available for CUDA tensors
    static void CopyGpuToHost(TensorData tensorData);

private:


    static unsigned long m_convertDenseToSparse(SparseMatrix* sparse,
                                                const float* dense, Shape shape,
                                                unsigned long paddedRowSize,
                                                Device device);

    static unsigned long m_convertSparseToDense(SparseMatrix* sparse,
                                                const float* dense, Shape shape,
                                                unsigned long paddedRowSize,
                                                Device device);
    //! Allocates data on the CPU with given batchSize
    void m_allocateCpu(unsigned int batchSize);

    //! Allocates data on the GPU with given batchSize
    bool m_allocateCuda(unsigned int batchSize);

    //! Free space allocated on CPU memory
    void m_freeCpu() const;

    //! Free space allocated on GPU memory
    bool m_freeGpu();


    Type m_type = Type::Dense;
    Device m_device;
};
}


#endif
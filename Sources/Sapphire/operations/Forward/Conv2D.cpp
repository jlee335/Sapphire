// Copyright (c) 2021, Justin Kim

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#include <Sapphire/operations/Forward/Conv2D.hpp>
#include <Sapphire/compute/ConvolutionOps.hpp>
#include <Sapphire/operations/Backward/Conv2DBackward.hpp>
#include <Sapphire/util/UnitUtils.hpp>
#include <Sapphire/compute/BasicOps.hpp>

namespace Sapphire::NN
{
Conv2D::Conv2D(int inChannels, int outChannels, std::pair<int, int> inputSize,
               std::pair<int, int> kernelSize,
               std::pair<int, int> stride, std::pair<int, int> padSize,
               std::pair<int, int> dilation, bool useBias,
               Util::SharedPtr<Optimizer::Optimizer> optimizer,
               std::unique_ptr<Initialize::Initializer> kernelInitializer,
               std::unique_ptr<Initialize::Initializer> biasInitializer,
               CudaDevice device, bool isSparse)
    : m_inputChannels(inChannels),
      m_outputChannels(outChannels),
      m_inputSize(inputSize),
      m_kernelSize(kernelSize),
      m_stride(stride),
      m_padSize(padSize),
      m_dilation(dilation),
      m_useBias(useBias),
      m_device(std::move(device)),
      m_isSparse(isSparse),
      m_optimizer(std::move(optimizer))
{
    const Type type = m_isSparse ? Type::Sparse : Type::Dense;

    if (m_isSparse)
        throw std::invalid_argument(
            "NN::Linear - Sparse version not implemented");

    auto [dilationRows, dilationCols] = dilation;
    auto [inputRows, inputCols] = inputSize;
    auto [rowPadding, colPadding] = padSize;
    auto [kernelRows, kernelCols] = kernelSize;
    auto [strideRows, strideCols] = stride;

    m_yRows =
        (inputRows + 2 * rowPadding - dilationRows * (kernelRows - 1) - 1) /
        strideRows + 1;
    m_yCols =
        (inputCols + 2 * colPadding - dilationCols * (kernelCols - 1) - 1) /
        strideCols + 1;

    auto kernel = TensorUtil::TensorData(
        Shape({ static_cast<unsigned int>(outChannels),
                static_cast<unsigned int>(inChannels),
                static_cast<unsigned int>(kernelRows),
                static_cast<unsigned int>(kernelCols) }), type,
        m_device);
    kernelInitializer->operator()(kernel);
    kernel.ToCuda();
    m_trainableDataMap["kernel"] = std::move(kernel);

    if (useBias)
    {
        auto bias =
            TensorUtil::TensorData(
                Shape({ static_cast<unsigned int>(outChannels),
                        static_cast<unsigned int>(m_yRows),
                        static_cast<unsigned int>(m_yCols) }),
                type, m_device);
        biasInitializer->operator()(bias);
        bias.ToCuda();
        m_trainableDataMap["bias"] = std::move(bias);
    }
}

Tensor Conv2D::operator()(Tensor& tensor)
{
    auto mode = tensor.Mode();
    auto& model = ModelManager::GetCurrentModel();

    auto& xDesc = model.GetDescriptor(tensor.TensorDescriptorKey());
    const auto yKey = m_registerOutputTensor(xDesc);
    auto& yDesc = model.GetDescriptor(yKey);
    yDesc.SetMode(mode);

    auto [dilationRows, dilationCols] = m_dilation;
    auto [inputRows, inputCols] = m_inputSize;
    auto [rowPadding, colPadding] = m_padSize;
    auto [kernelRows, kernelCols] = m_kernelSize;
    auto [strideRows, strideCols] = m_stride;

    auto x = xDesc.GetForwardData();
    auto dx = xDesc.GetBackwardData();
    auto y = yDesc.GetForwardData();
    auto dy = yDesc.GetBackwardData();

    auto kernel = m_trainableDataMap.at("kernel");
    kernel.SetMode(mode);

    Util::ChangeTensorDataDimension(4, x, dx, y, dy);

    Compute::Initialize::Zeros(y);
    Compute::Conv2DForward(y, x, kernel, strideRows, strideCols, dilationRows,
                           dilationCols, rowPadding, colPadding);

    if (m_useBias)
    {
        auto bias = m_trainableDataMap.at("bias");
        bias.SetMode(mode);
        Compute::Add(y, y, bias);
        auto backPropWrapper = Util::SharedPtr<BackProp::Conv2DBackProp>::Make(
            dx, dy, kernel, bias, x, m_stride, m_dilation, m_padSize,
            m_optimizer);
        SaveHistory(backPropWrapper, std::make_tuple(&xDesc),
                    std::make_tuple(&yDesc));
    }
    else
    {
        auto backPropWrapper = Util::SharedPtr<BackProp::Conv2DBackProp>::Make(
            dx, dy, kernel, x, m_stride, m_dilation, m_padSize, m_optimizer);
        SaveHistory(backPropWrapper, std::make_tuple(&xDesc),
                    std::make_tuple(&yDesc));
    }
    return Tensor(yKey);
}
}
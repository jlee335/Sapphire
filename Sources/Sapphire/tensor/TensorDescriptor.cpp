// Copyright (c) 2021, Justin Kim

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#include <Sapphire/tensor/TensorDescriptor.hpp>
#include <Sapphire/operations/Initializers/Initialize.hpp>
#include <algorithm>

namespace Sapphire::TensorUtil
{
TensorDescriptor::TensorDescriptor(const Shape& shape, Type type,
                                   const CudaDevice& device,
                                   int key, bool preserve)
    : m_forwardData(shape, type, device, key, preserve),
      m_backwardData(shape, type, device, key, preserve),
      m_key(key),
      m_trainable(false)
{
}

TensorDescriptor::TensorDescriptor(TensorDescriptor&& tensorData) noexcept
    : m_forwardData(std::move(tensorData.m_forwardData)),
      m_backwardData(std::move(tensorData.m_backwardData)),
      m_key(tensorData.m_key),
      m_batchSize(tensorData.m_batchSize),
      m_trainable(tensorData.m_trainable),
      m_history(std::move(tensorData.m_history))
{
}

TensorDescriptor& TensorDescriptor::operator=(
    TensorDescriptor&& tensorDesc) noexcept
{
    m_forwardData = tensorDesc.m_forwardData;
    m_backwardData = tensorDesc.m_backwardData;
    m_key = tensorDesc.m_key;
    m_batchSize = tensorDesc.m_batchSize;
    m_trainable = tensorDesc.m_trainable;
    m_history = std::move(tensorDesc.m_history);
    return *this;
}

TensorData TensorDescriptor::GetForwardData() const
{
    return m_forwardData;
}

TensorData TensorDescriptor::GetBackwardData() const
{
    return m_backwardData;
}

unsigned int TensorDescriptor::GetBatchSize() const
{
    return m_batchSize;
}

Shape TensorDescriptor::GetShape() const
{
    return m_forwardData.GetShape();
}

CudaDevice TensorDescriptor::GetDevice() const
{
    if (Mode() == DeviceType::Cuda)
        return m_forwardData.GetDevice();
    return CudaDevice();
}

CudaDevice TensorDescriptor::GetCudaDevice() const
{
    return m_forwardData.GetDevice();
}

Type TensorDescriptor::GetType() const
{
    return m_forwardData.GetType();
}

void TensorDescriptor::Reshape(Shape shape)
{
    m_forwardData.Reshape(shape);
    m_backwardData.Reshape(shape);
}

void TensorDescriptor::ToCuda()
{
    m_forwardData.ToCuda();
    m_backwardData.ToCuda();
}


void TensorDescriptor::ToHost()
{
    m_forwardData.ToHost();
    m_backwardData.ToHost();
}


DeviceType TensorDescriptor::Mode() const
{
    return m_forwardData.Mode();
}

void TensorDescriptor::SetMode(DeviceType deviceType)
{
    m_forwardData.SetMode(deviceType);
    m_backwardData.SetMode(deviceType);
}

void TensorDescriptor::InitGradient()
{
    Initialize::Zeros zeroInitializer;
    zeroInitializer(m_backwardData);
}

void TensorDescriptor::AppendOutputHistory(
    int backPropWrapperKey,
    int location)
{
    m_history.emplace_back(History(backPropWrapperKey, location));
}

void TensorDescriptor::AppendOperandHistory(int tensorDescKey)
{
    if (m_history.empty() || m_history.back().IsOutput)
    {
        History history;
        history.AddOperand(tensorDescKey);
        m_history.emplace_back(std::move(history));
        return;
    }

    m_history.back().AddOperand(tensorDescKey);
}

void TensorDescriptor::RemoveOperand(int tensorDescKey)
{
    if (m_history.empty() || m_history.back().IsOutput)
    {
        throw std::runtime_error(
            "RemoveOperand - Last history was empty or last history was output");
    }

    auto& history = m_history.back();
    history.RemoveOperand(tensorDescKey);
}

void TensorDescriptor::PopIfOperandHistory()
{
    if (!m_history.empty() && !m_history.back().IsOutput)
        m_history.pop_back();
}

void TensorDescriptor::PopOutputHistory()
{
    if (!m_history.empty() && m_history.back().IsOutput)
        m_history.pop_back();
    else
        throw std::runtime_error(
            "TensorDescriptor::PopOutputHistory - the last history was not output or history was empty");
}

bool TensorDescriptor::IsBackPropReady() const

{
    if (m_history.empty())
        return false;
    if (m_history.back().IsOutput)
        return true;

    if (const auto& lastHistory = m_history.back();
        lastHistory.GradientInputTensorKeyList.empty())
        return true;

    return false;
}
} // namespace Sapphire::TensorUtil

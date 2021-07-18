// Copyright (c) 2021, Justin Kim

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#include <Sapphire/Model.hpp>

namespace Sapphire
{
Model::Model(std::string name)
    : m_name(std::move(name))
{
}

int Model::RegisterUnitDataWrapper(UnitDataWrapper& unitDataWrapper)
{
    const int unitKey = m_unitPool.Counter++;
    m_unitPool.UnitWrapperMap[unitKey] = unitDataWrapper;

    return unitKey;
}

int Model::RegisterTensorDescriptor(const Shape& shape, Type type,
                                    const Device& device,
                                    unsigned int batchSize,
                                    bool isTrainable)
{
    const int tensorDescKey = m_tensorDescriptorPool.Counter++;
    TensorUtil::TensorDescriptor tensorDesc(shape, type, device, batchSize,
                                            tensorDescKey);
    if (isTrainable)
    {
        tensorDesc.BackwardData = TensorUtil::TensorData(
            shape, type, device, batchSize, tensorDescKey);
    }

    m_tensorDescriptorPool.TensorDescMap[tensorDescKey] = std::move(tensorDesc);

    return tensorDescKey;
}

void Model::m_autoGrad(int tensorKey)
{
    if (auto& descriptor = GetDescriptor(tensorKey);
        descriptor.IsBackPropReady())
    {
        descriptor.PopIfOperandHistory();
        const auto& wrapper = descriptor.GetBackPropWrapper();
        const auto outputTensorDataVector = wrapper->
            GetOutputTensorDataVector();

        const bool isInvoked = wrapper->InvokeBackProp(descriptor.BackwardData);
        descriptor.PopHistory(); //! Pop output history

        if (isInvoked)
            for (auto& tensorData : outputTensorDataVector)
            {
                GetDescriptor(tensorData.GetDescriptorKey())
                    .RemoveGradientInput(tensorKey);
                m_autoGrad(tensorData.GetDescriptorKey());
            }
    }
}

UnitDataWrapper& Model::GetUnitDataWrapper(int key)
{
    return m_unitPool.UnitWrapperMap.at(key);
}

TensorUtil::TensorDescriptor& Model::GetDescriptor(int descKey)
{
    return m_tensorDescriptorPool.TensorDescMap.at(descKey);
}

std::string ModelManager::m_currentModel;

std::unordered_map<std::string, Model> ModelManager::m_modelMap;

Model& ModelManager::GetModel(const std::string& modelName)
{
    return m_modelMap.at(modelName);
}

Model& ModelManager::GetCurrentModel()
{
    return m_modelMap.at(m_currentModel);
}

void ModelManager::SetCurrentModel(const std::string& modelName)
{
    if (m_modelMap.find(modelName) == m_modelMap.end())
        throw std::invalid_argument(
            "ModelManager::SetCurrentModel - Given model name is not "
            "registered");
    m_currentModel = modelName;
}

void ModelManager::AddModel(const std::string& modelName)
{
    m_modelMap.emplace(modelName, Model(modelName));
}
} // namespace Sapphire
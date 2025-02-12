// Copyright (c) 2021, Justin Kim

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#include <ModelTest/SimpleLinearModel.hpp>
#include <Sapphire/Model.hpp>
#include <Sapphire/operations/Forward/Linear.hpp>
#include <Sapphire/operations/Forward/ReLU.hpp>
#include <Sapphire/operations/Loss/MSE.hpp>
#include <Sapphire/operations/optimizers/SGD.hpp>
#include <Sapphire/util/ResourceManager.hpp>
#include <iostream>

namespace Sapphire::Test
{
void SimpleLinearModel(std::vector<float> xData, std::vector<float> labelData,
                       int inputSize, int outputSize, float learningRate,
                       int batchSize, int epochs, bool hostMode)
{
    ModelManager::AddModel("SimpleLinearModel");
    ModelManager::SetCurrentModel("SimpleLinearModel");

    const CudaDevice gpu(0, "cuda0");

    NN::Linear linear(inputSize, outputSize,
                      new Optimizer::SGD(learningRate), gpu);

    Tensor weight(Shape({ inputSize, outputSize }), gpu, Type::Dense, true);
    Tensor weight1(Shape({ outputSize, outputSize }), gpu, Type::Dense, true);

    Tensor bias(Shape({ 1, outputSize }), gpu, Type::Dense, true);
    Tensor bias1(Shape({ 1, outputSize }), gpu, Type::Dense, true);
    Initialize::Initialize(weight,
                           std::make_unique<Initialize::Normal>(0.0f, 0.01f));
    Initialize::Initialize(weight1,
                           std::make_unique<Initialize::Normal>(0.0f, 0.01f));
    Initialize::Initialize(bias,
                           std::make_unique<Initialize::Normal>(0.0f, 0.01f));
    Initialize::Initialize(bias1,
                           std::make_unique<Initialize::Normal>(0.0f, 0.01f));

    if (hostMode)
    {
        weight.ToHost();
        weight1.ToHost();
        bias.ToHost();
        bias1.ToHost();
    }

    Tensor x(Shape({ batchSize, 1, inputSize }), gpu, Type::Dense, true);
    Tensor label(Shape({ batchSize, 1, outputSize }), gpu, Type::Dense, true);

    if (hostMode)
    {
        x.ToHost();
        label.ToHost();
    }

    x.LoadData(xData);
    label.LoadData(labelData);
    for (int i = 0; i < epochs; ++i)
    {
        auto y = linear(x, weight, bias);
        y = NN::ReLU(y);
        y = NN::ReLU(linear(y, weight1, bias1));
        const auto loss = NN::Loss::MSE(y, label);
        if (i % 20 == 0)
        {
            const auto lossData = loss.GetDataCopy();
            std::cout << "epoch: " << i << " loss : " << lossData[0]
                << std::endl;
        }
        //ModelManager::CurModel().InitGradient();
        ModelManager::CurModel().BackProp(loss);
        ModelManager::CurModel().Clear();
        if (i % 10 == 0)
            Util::ResourceManager::Clean();
    }
    Util::ResourceManager::ClearAll();
}
}

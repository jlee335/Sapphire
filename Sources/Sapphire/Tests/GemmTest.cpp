// Copyright (c) 2021, Justin Kim

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#include <Sapphire/Tests/GemmTest.hpp>
#include <Sapphire/compute/BasicOps.hpp>
#include <Sapphire/compute/Initialize.hpp>
#include <Sapphire/tensor/Shape.hpp>
#include <Sapphire/tensor/TensorData.hpp>
#include <Sapphire/util/Device.hpp>
#include <Sapphire/util/ResourceManager.hpp>
#include <atomic>
#include <cmath>
#include <iostream>
#include <random>
#include "doctest.h"

namespace Sapphire::Test
{
void Gemm1()
{
    for (int j = 0; j < 10; j++)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distribution(1, 100);

        const unsigned int M = distribution(gen);
        const unsigned int N = distribution(gen);
        const unsigned int K = distribution(gen);
        const unsigned int batchSize = distribution(gen) % 30 + 1;

        const Shape shapeA({ batchSize, M, K });
        const Shape shapeB({ batchSize, K, N });
        const Shape shapeC({ batchSize, M, N });
        const Shape shapeOut({ batchSize, M, N });

        std::cout << "M : " << M << " N: " << N << " K: " << K
            << " batchSize : " << batchSize << std::endl;

        const Device cuda(0, "device0");
        const Device host("host");

        TensorUtil::TensorData A(shapeA, Type::Dense, host);
        TensorUtil::TensorData B(shapeB, Type::Dense, host);
        TensorUtil::TensorData C(shapeC, Type::Dense, host);
        TensorUtil::TensorData Out(shapeOut, Type::Dense, host);

        Compute::Initialize::Normal(A, 10, 5);
        Compute::Initialize::Normal(B, 10, 5);
        Compute::Initialize::Normal(C, 10, 5);
        Compute::Initialize::Zeros(Out);

        Compute::Gemm(Out, A, B, C);

        auto* cpuGemmResult = new float[Out.DenseTotalLengthHost];

        for (long i = 0; i < static_cast<long>(Out.DenseTotalLengthHost); ++i)
        {
            cpuGemmResult[i] = Out.GetMutableDenseHost()[i];
        }

        Compute::Initialize::Zeros(Out);

        A.SendTo(cuda);
        B.SendTo(cuda);
        C.SendTo(cuda);
        Out.SendTo(cuda);

        Compute::Gemm(Out, A, B, C);

        Out.SendTo(host);

        std::atomic<float> largestError = 0.0f;

#pragma omp parallel for default(none) schedule(static) \
    shared(Out, cpuGemmResult, largestError)
        for (long i = 0; i < static_cast<long>(Out.DenseTotalLengthHost); ++i)
        {
            auto error = std::abs(
                cpuGemmResult[i] - Out.GetMutableDenseHost()[i]);
            if (largestError < error)
                largestError = error;
            CHECK(error <= 2.0f);
        }

        std::cout << "Largest error : " << largestError << std::endl;
        delete[] cpuGemmResult;
    }

    Util::ResourceManager::ClearAll();
}

void Gemm2()
{
    for (int j = 0; j < 10; j++)
    {
        std::random_device
            rd; // Will be used to obtain a seed for the random number engine
        std::mt19937 gen(
            rd()); // Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution<> distrib(8, 16);

        const unsigned int M = distrib(gen);
        const unsigned int N = distrib(gen);
        const unsigned int K = distrib(gen);
        const unsigned int batchSize = distrib(gen) % 3 + 1;

        std::cout << "M : " << M << " N: " << N << " K: " << K
            << " batchSize : " << batchSize << std::endl;

        const Shape shapeA({ batchSize, M, K });
        const Shape shapeB({ batchSize, K, N });
        const Shape shapeC({ batchSize, M, N });
        const Shape shapeOut({ batchSize, M, N });

        const Device cuda(0, "device0");
        const Device host("host");

        TensorUtil::TensorData A(shapeA, Type::Dense, host);
        TensorUtil::TensorData B(shapeB, Type::Dense, host);
        TensorUtil::TensorData C(shapeC, Type::Dense, host);
        TensorUtil::TensorData out(shapeOut, Type::Dense, host);

        Compute::Initialize::Normal(A, 10, 5);
        Compute::Initialize::Normal(B, 10, 5);
        Compute::Initialize::Normal(C, 10, 5);
        Compute::Initialize::Zeros(out);

        A.SendTo(cuda);
        B.SendTo(cuda);
        C.SendTo(cuda);
        out.SendTo(cuda);

        Compute::Initialize::Zeros(out);
        Compute::Gemm(out, A, B, C);

        A.SendTo(host);
        B.SendTo(host);
        C.SendTo(host);
        out.SendTo(host);

        auto* cudaGemmResult = new float[out.DenseTotalLengthHost];

#pragma omp parallel for default(shared) schedule(static)
        for (long i = 0; i < static_cast<long>(out.DenseTotalLengthHost); ++i)
        {
            cudaGemmResult[i] = out.GetMutableDenseHost()[i];
        }

        Compute::Initialize::Zeros(out);
        Compute::Gemm(out, A, B, C);

        std::atomic largestError = 0.0f;

#pragma omp parallel for default(shared) schedule(static)
        for (long i = 0; i < static_cast<long>(out.DenseTotalLengthHost); ++i)
        {
            auto error = std::abs(
                cudaGemmResult[i] - out.GetMutableDenseHost()[i]);
            if (largestError < error)
                largestError = error;

            CHECK(error <= std::abs(out.GetMutableDenseHost()[i] / 100.0f));
        }

        std::cout << "Largest error : " << largestError << std::endl;
        delete[] cudaGemmResult;
    }
    Util::ResourceManager::ClearAll();
}

void GemmBroadcast()
{
    for (int j = 0; j < 10; j++)
    {
        std::random_device
            rd; // Will be used to obtain a seed for the random number engine
        std::mt19937 gen(
            rd()); // Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution<> distrib(1, 16);

        const unsigned int M = distrib(gen);
        const unsigned int N = distrib(gen);
        const unsigned int K = distrib(gen);
        const unsigned int batchSize = distrib(gen) % 3 + 1;

        std::cout << "M : " << M << " N: " << N << " K: " << K
            << " batchSize : " << batchSize << std::endl;

        const Shape shapeA({ 1, M, K });
        const Shape shapeB({ batchSize, K, N });
        const Shape shapeC({ 1, M, N });
        const Shape shapeOut({ batchSize, M, N });

        const Device cuda(0, "device0");
        const Device host("host");

        TensorUtil::TensorData A(shapeA, Type::Dense, host);
        TensorUtil::TensorData B(shapeB, Type::Dense, host);
        TensorUtil::TensorData C(shapeC, Type::Dense, host);
        TensorUtil::TensorData out(shapeOut, Type::Dense, host);

        Compute::Initialize::Normal(A, 10, 1);
        Compute::Initialize::Normal(B, 10, 1);
        Compute::Initialize::Normal(C, 10, 1);
        Compute::Initialize::Zeros(out);

        A.SendTo(cuda);
        B.SendTo(cuda);
        C.SendTo(cuda);
        out.SendTo(cuda);

        Compute::Gemm(out, A, B, C);

        A.SendTo(host);
        B.SendTo(host);
        C.SendTo(host);
        out.SendTo(host);

        auto* cudaGemmResult = new float[out.DenseTotalLengthHost];

#pragma omp parallel for default(shared) schedule(static)
        for (long i = 0; i < static_cast<long>(out.DenseTotalLengthHost); ++i)
        {
            cudaGemmResult[i] = out.GetMutableDenseHost()[i];
        }

        Compute::Initialize::Zeros(out);
        Compute::Gemm(out, A, B, C);

        std::atomic<float> largestError = 0.0f;

#pragma omp parallel for default(shared) schedule(static)
        for (long i = 0; i < static_cast<long>(out.DenseTotalLengthHost); ++i)
        {
            auto error = std::abs(
                cudaGemmResult[i] - out.GetMutableDenseHost()[i]);
            if (largestError < error)
                largestError = error;

            CHECK(error <= std::abs(out.GetMutableDenseHost()[i] / 100.0f));
        }

        std::cout << "Largest error : " << largestError << std::endl;

        delete[] cudaGemmResult;
    }
    Util::ResourceManager::ClearAll();
}

void GemmBroadcastOnOutput()
{
    for (int j = 0; j < 30; j++)
    {
        std::random_device
            rd; // Will be used to obtain a seed for the random number engine
        std::mt19937 gen(
            rd()); // Standard mersenne_twister_engine seeded with rd()
        std::uniform_int_distribution<> distrib(1, 16);

        const unsigned int M = distrib(gen);
        const unsigned int N = distrib(gen);
        const unsigned int K = distrib(gen);
        const unsigned int batchSize = distrib(gen) % 3 + 1;

        std::cout << "M : " << M << " N: " << N << " K: " << K
            << " batchSize : " << batchSize << std::endl;

        const Shape shapeA({ M, K });
        const Shape shapeB({ batchSize, K, N });
        const Shape shapeC({ M, N });
        const Shape shapeOut({ M, N });

        const Device cuda(0, "device0");
        const Device host("host");

        TensorUtil::TensorData A(shapeA, Type::Dense, host);
        TensorUtil::TensorData B(shapeB, Type::Dense, host);
        TensorUtil::TensorData out(shapeOut, Type::Dense, host);

        Compute::Initialize::Normal(A, 10, 1);
        Compute::Initialize::Normal(B, 10, 1);

        A.SendTo(cuda);
        B.SendTo(cuda);
        out.SendTo(cuda);

        Compute::Initialize::Zeros(out);

        Compute::Gemm(out, A, B, out);

        A.SendTo(host);
        B.SendTo(host);
        out.SendTo(host);

        auto* cudaGemmResult = new float[out.DenseTotalLengthHost];

#pragma omp parallel for default(shared) schedule(static)
        for (long i = 0; i < static_cast<long>(out.DenseTotalLengthHost); ++i)
        {
            cudaGemmResult[i] = out.GetMutableDenseHost()[i];
        }

        Compute::Initialize::Zeros(out);
        Compute::Gemm(out, A, B, out);

        std::atomic largestError = 0.0f;

#pragma omp parallel for default(shared) schedule(static)
        for (long i = 0; i < static_cast<long>(out.DenseTotalLengthHost); ++i)
        {
            auto error = std::abs(
                cudaGemmResult[i] - out.GetMutableDenseHost()[i]);
            if (largestError < error)
                largestError = error;

            CHECK(error <= std::abs(out.GetMutableDenseHost()[i] / 100.0f));
        }

        std::cout << "Largest error : " << largestError << std::endl;
        delete[] cudaGemmResult;
    }
    Util::ResourceManager::ClearAll();
}
} // namespace Sapphire::Test

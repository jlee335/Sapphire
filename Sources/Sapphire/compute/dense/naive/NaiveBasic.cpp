// Copyright (c) 2021, Jaewoo Kim

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#include <Sapphire/compute/dense/naive/NaiveBasic.hpp>
#include <cmath>

namespace Sapphire::Compute::Dense::Naive
{
void Add(unsigned int totalSize, float* output, const float* inputA,
         const float* inputB, unsigned int inputStride, bool broadcastInputA,
         bool broadcastInputB)
{
    unsigned int leftOverA = broadcastInputA ? inputStride : totalSize;
    unsigned int leftOverB = broadcastInputB ? inputStride : totalSize;

    for (unsigned int i = 0; i < totalSize; i++)
    {
        output[i] = inputA[i % leftOverA] + inputB[i % leftOverB];
    }
}

void Sub(unsigned int totalSize, float* output, const float* inputA,
         const float* inputB, unsigned int inputStride, bool broadcastInputA,
         bool broadcastInputB)
{
    unsigned int leftOverA = broadcastInputA ? inputStride : totalSize;
    unsigned int leftOverB = broadcastInputB ? inputStride : totalSize;

    for (unsigned int i = 0; i < totalSize; i++)
    {
        output[i] = inputA[i % leftOverA] - inputB[i % leftOverB];
    }
}

void Dot(unsigned int totalSize, float* output, const float* inputA,
         const float* inputB, unsigned int inputStride, bool broadcastInputA,
         bool broadcastInputB)
{
    unsigned int leftOverA = broadcastInputA ? inputStride : totalSize;
    unsigned int leftOverB = broadcastInputB ? inputStride : totalSize;

    for (unsigned int i = 0; i < totalSize; i++)
    {
        output[i] = inputA[i % leftOverA] * inputB[i % leftOverB];
    }
}

void Scale(float* output, const float* input, const float scaleFactor,
           unsigned int totalSize, unsigned colSize, unsigned padSize)
{
    for (unsigned int ii = 0; ii < totalSize; ii += padSize)
        for (unsigned int i = ii; i < ii + colSize; i++)
        {
            output[i] = input[i] * scaleFactor;
        }
}

void Transpose(float* output, const float* input, unsigned int inputRows,
               unsigned int paddedInputRows, unsigned int inputCols,
               unsigned int paddedInputCols, unsigned int batchSize,
               bool broadcast)
{
    const auto leftOver = broadcast
                              ? inputRows * paddedInputCols
                              : batchSize * inputRows * paddedInputCols;
    for (unsigned int batchIdx = 0; batchIdx < batchSize; batchIdx++)
        for (unsigned int i = 0; i < inputRows; i++)
            for (unsigned int j = 0; j < inputCols; j++)
            {
                float* outputOffset =
                    output + batchIdx * inputCols * paddedInputRows;
                const float* inputOffset =
                    input + batchIdx * inputRows * paddedInputCols;
                outputOffset[j * paddedInputRows + i] =
                    inputOffset[(i * paddedInputCols + j) % leftOver];
            }
}

void Pow(float* output, const float* input, const float exponent,
         unsigned int totalSize, unsigned colSize, unsigned padSize)
{
    for (unsigned int ii = 0; ii < totalSize; ii += padSize)
        for (unsigned int i = ii; i < ii + colSize; i++)
        {
            output[i] = std::pow(input[i], exponent);
        }
}

void cos(float* output, const float* input, unsigned int totalSize,
         unsigned colSize, unsigned
         padSize)
{
    for (unsigned int ii = 0; ii < totalSize; ii += padSize)
        for (unsigned int i = ii; i < ii + colSize; i++)
        {
            output[i] = std::cos(input[i]);
        }
}

void sin(float* output, const float* input, unsigned int totalSize,
         unsigned colSize, unsigned
         padSize)
{
    for (unsigned int ii = 0; ii < totalSize; ii += padSize)
        for (unsigned int i = ii; i < ii + colSize; i++)
        {
            output[i] = std::sin(input[i]);
        }
}

void tan(float* output, const float* input, unsigned int totalSize,
         unsigned colSize, unsigned
         padSize)
{
    for (unsigned int ii = 0; ii < totalSize; ii += padSize)
        for (unsigned int i = ii; i < ii + colSize; i++)
        {
            output[i] = std::tan(input[i]);
        }
}

void cosh(float* output, const float* input, unsigned int totalSize,
          unsigned colSize, unsigned
          padSize)
{
    for (unsigned int ii = 0; ii < totalSize; ii += padSize)
        for (unsigned int i = ii; i < ii + colSize; i++)
        {
            output[i] = std::cosh(input[i]);
        }
}

void sinh(float* output, const float* input, unsigned int totalSize,
          unsigned colSize, unsigned
          padSize)
{
    for (unsigned int ii = 0; ii < totalSize; ii += padSize)
        for (unsigned int i = ii; i < ii + colSize; i++)
        {
            output[i] = std::sinh(input[i]);
        }
}

void tanh(float* output, const float* input, unsigned int totalSize,
          unsigned colSize, unsigned
          padSize)
{
    for (unsigned int ii = 0; ii < totalSize; ii += padSize)
        for (unsigned int i = ii; i < ii + colSize; i++)
        {
            output[i] = std::tanh(input[i]);
        }
}

void log(float* output, const float* input, unsigned int totalSize,
         unsigned colSize, unsigned
         padSize)
{
    for (unsigned int ii = 0; ii < totalSize; ii += padSize)
        for (unsigned int i = ii; i < ii + colSize; i++)
        {
            output[i] = std::log(input[i]);
        }
}

void log10(float* output, const float* input, unsigned int totalSize,
           unsigned colSize, unsigned
           padSize)
{
    for (unsigned int ii = 0; ii < totalSize; ii += padSize)
        for (unsigned int i = ii; i < ii + colSize; i++)
        {
            output[i] = std::log10(input[i]);
        }
}

void ReLU(float* output, const float* input, unsigned int totalSize,
          unsigned colSize, unsigned
          padSize)
{
    for (unsigned int ii = 0; ii < totalSize; ii += padSize)
        for (unsigned int i = ii; i < ii + colSize; i++)
        {
            output[i] = input[i] > 0 ? input[i] : 0;
        }
}

void ReLUBackward(float* dx, const float* dy, const float* x,
                  unsigned int totalSize, unsigned colSize, unsigned
                  padSize)
{
    for (unsigned int ii = 0; ii < totalSize; ii += padSize)
        for (unsigned int i = ii; i < ii + colSize; i++)
        {
            dx[i] = x[i] > 0.0f ? dy[i] : 0.0f;
        }
}

void LeakyReLU(float* output, const float* input, float a,
               unsigned int totalSize, unsigned colSize, unsigned padSize)
{
    for (unsigned int ii = 0; ii < totalSize; ii += padSize)
        for (unsigned int i = ii; i < ii + colSize; i++)
        {
            output[i] = input[i] > 0 ? input[i] : a * input[i];
        }
}

void LeakyReLUBackward(float* output, const float* input, float a,
                       unsigned int totalSize, unsigned colSize,
                       unsigned padSize)
{
    for (unsigned int ii = 0; ii < totalSize; ii += padSize)
        for (unsigned int i = ii; i < ii + colSize; i++)
        {
            output[i] = input[i] > 0 ? 1 : a;
        }
}

void Inverse(float* output, const float* input, unsigned int totalSize,
             unsigned colSize, unsigned
             padSize)
{
    for (unsigned int ii = 0; ii < totalSize; ii += padSize)
        for (unsigned int i = ii; i < ii + colSize; i++)
        {
            output[i] = 1.0f / input[i];
        }
}

void Mean(float* y, const float* x,
          unsigned ySize, unsigned int unitSize, unsigned stride,
          unsigned yCols, unsigned yPadSize, unsigned xCols, unsigned xPadSize)
{
    for (unsigned int unitId = 0; unitId < ySize; unitId++)
    {
        const auto yIdx = (unitId / yCols) * yPadSize + unitId % yCols;
        const auto outerId = unitId / stride;
        const auto innerId = unitId % stride;

        for (unsigned int i = 0; i < unitSize; i++)
        {
            const auto xIdx =
                unitSize * stride * outerId + i * stride + innerId;
            y[yIdx] += x[(xIdx / xCols) * xPadSize + xIdx % xCols];
        }
        y[yIdx] /= static_cast<float>(unitSize);
    }
}

void MeanBackward(float* dx, const float* x, const float* dy,
                  unsigned int ySize, unsigned int unitSize,
                  unsigned int stride,
                  unsigned int yCols, unsigned int yPadSize,
                  unsigned xCols, unsigned xPadSize)
{
    for (unsigned int unitId = 0; unitId < ySize; unitId++)
    {
        const auto yIdx = (unitId / yCols) * yPadSize + unitId % yCols;
        const auto outerId = unitId / stride;
        const auto innerId = unitId % stride;

        for (unsigned int i = 0; i < unitSize; i++)
        {
            const auto xIdx =
                unitSize * stride * outerId + i * stride + innerId;
            dx[(xIdx / xCols) * xPadSize + xIdx % xCols] += dy[yIdx] / static_cast<float>(unitSize);
        }
    }
}

void Softmax(float* output, const float* input, unsigned int paddedTotalSize,
             unsigned int unitSize, unsigned int paddedUnitSize)
{
    const auto batchSize = paddedTotalSize / paddedUnitSize;

    for (unsigned int batchIdx = 0; batchIdx < batchSize; ++batchIdx)
    {
        float sum = 0;
        for (unsigned int i = 0; i < unitSize; ++i)
            sum += std::exp(input[paddedUnitSize * batchIdx + i]);

        for (unsigned int i = 0; i < unitSize; ++i)
            output[paddedUnitSize * batchIdx + i] =
                std::exp(input[paddedUnitSize * batchIdx + i]) / sum;
    }
}

void SoftmaxBackward(float* dx, const float* dy, const float* x,
                     unsigned int totalSize, unsigned int unitSize,
                     unsigned int padSize)
{
    const auto batchSize = totalSize / padSize;

    for (unsigned int batchIdx = 0; batchIdx < batchSize; ++batchIdx)
    {
        float sum = 0;
        for (unsigned int unitIdx = 0; unitIdx < unitSize; ++unitIdx)
            for (unsigned int i = 0; i < unitSize; ++i)
            {
                if (i == unitIdx)
                {
                    sum += dy[padSize * batchIdx + i] *
                    (x[padSize * batchIdx + i] *
                     (1 - x[padSize * batchIdx + i]));
                }
                else
                {
                    sum += dy[padSize * batchIdx + i] *
                    (-x[padSize * batchIdx + unitIdx] *
                     x[padSize * batchIdx + i]);
                }
            }
    }
}
} // namespace Sapphire::Compute::Naive::Dense

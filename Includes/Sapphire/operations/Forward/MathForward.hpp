// Copyright (c) 2021, Justin Kim

// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#ifndef SAPPHIRE_FUNCTIONAL_MATHFORWARD_HPP
#define SAPPHIRE_FUNCTIONAL_MATHFORWARD_HPP

#include <Sapphire/tensor/Tensor.hpp>

namespace Sapphire::NN::Functional
{
[[maybe_unused]]
Tensor MulOp(const Tensor& inputA, const Tensor& inputB);

[[maybe_unused]]
Tensor AddOp(const Tensor& inputA, const Tensor& inputB);

[[maybe_unused]]
Tensor MeanOp(const Tensor& input, int dim);
}

#endif

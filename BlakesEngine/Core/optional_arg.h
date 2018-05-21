#pragma once
#include <optional>
#include <type_traits>

// This is to avoid copying large structures with std::optional
template <typename T>
using optional_arg = std::optional<std::reference_wrapper<T>>;
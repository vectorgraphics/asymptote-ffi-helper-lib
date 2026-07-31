#pragma once
#include <optional>

namespace AsyFfiHelpers::Common
{

template<typename T>
T* pointerOrNull(std::optional<T>& obj)
{
    return obj.has_value() ? &obj.value() : nullptr;
}

template<typename T>
T const* pointerOrNullConst(std::optional<T> const& obj)
{
    return obj.has_value() ? &obj.value() : nullptr;
}

}

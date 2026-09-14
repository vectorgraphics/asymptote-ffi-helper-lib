#pragma once
#include <asyffi.h>
#include "asyffihelpers/itemFnStructs.h"

namespace AsyFfiHelpers::Item
{

/** Set Asymptote items for basic types (int64_t, double, bool) or pointers */
template<typename T>
void setItem(IAsyItem* item, T const& value)
{
    ItemFunctions<T>::setItem(item, value);
}

/** Gets Asymptote items for basic types (int64_t, double, bool) or pointers */
template<typename T>
T getItem(IAsyItem const* item)
{
    return ItemFunctions<T>::getItem(item);
}

} // namespace AsyFfiHelpers::Item

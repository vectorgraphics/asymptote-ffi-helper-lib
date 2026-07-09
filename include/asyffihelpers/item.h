#pragma once
#include <asyffi.h>

namespace AsyFfiHelpers::Item
{

template<typename T>
void setItem(IAsyItem* item, T const& value) = delete;

template<>
void setItem<int64_t>(IAsyItem* item, int64_t const& value);

template<>
void setItem<double>(IAsyItem* item, double const& value);

template<>
void setItem<bool>(IAsyItem* item, bool const& value);

template<typename T>
void setItemPtr(IAsyItem* item, T* ptr)
{
    item->setRawPointer(static_cast<void*>(ptr));
}

template<typename T>
T getItem(IAsyItem const* item) = delete;

template<>
int64_t getItem<int64_t>(IAsyItem const* item);

template<>
double getItem<double>(IAsyItem const* item);

template<>
bool getItem<bool>(IAsyItem const* item);

template<typename T>
T* getItemPtr(IAsyItem const* item)
{
    return static_cast<T*>(item->asRawPointer());
}

} // namespace AsyFfiHelpers::Item

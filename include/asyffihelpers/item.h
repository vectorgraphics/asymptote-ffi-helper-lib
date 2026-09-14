#pragma once
#include <asyffi.h>
#include <type_traits>

namespace AsyFfiHelpers::Item
{

template<typename T>
    requires std::is_fundamental_v<T>
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

template<typename TPTr>
    requires std::is_pointer_v<TPTr>
void setItem(IAsyItem* item, TPTr ptr)
{
    item->setRawPointer(static_cast<void*>(ptr));
}

template<typename T>
    requires std::is_fundamental_v<T>
T getItem(IAsyItem const* item) = delete;

template<typename TPtr>
    requires std::is_pointer_v<TPtr>
TPtr getItem(IAsyItem const* item)
{
    return static_cast<TPtr>(item->asRawPointer());
}

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

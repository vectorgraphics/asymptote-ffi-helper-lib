#pragma once
#include <cstdint>
#include <asyffi.h>

namespace AsyFfiHelpers::Item
{
template<typename T>
class ItemFunctions
{
};

template<>
class ItemFunctions<int64_t>
{
public:
    static int64_t getItem(IAsyItem* item);

    static void setItem(IAsyItem* item, int64_t value);
};

template<>
class ItemFunctions<bool>
{
public:
    static bool getItem(IAsyItem* item);

    static void setItem(IAsyItem* item, bool value);
};

template<>
class ItemFunctions<double>
{
public:
    static double getItem(IAsyItem* item);

    static void setItem(IAsyItem* item, double value);
};

template<typename T>
class ItemFunctions<T*>
{
public:
    static T* getItem(IAsyItem* item)
    {
        return static_cast<T*>(item->asRawPointer());
    }

    static void setItem(IAsyItem* item, T* const value)
    {
        item->setRawPointer(static_cast<void*>(value));
    }
};

template<typename T>
class ItemWrapper
{
public:
    ItemWrapper(IAsyItem* baseItem): backingItem(baseItem) {}

    operator T()
    {
        return ItemFunctions<T>::getItem(backingItem);
    }

    ItemWrapper& operator=(T const& other)
    {
        ItemFunctions<T>::setItem(backingItem, other);
        return *this;
    }
private:
    IAsyItem* backingItem;
};
} // namespace AsyFfiHelper::Item

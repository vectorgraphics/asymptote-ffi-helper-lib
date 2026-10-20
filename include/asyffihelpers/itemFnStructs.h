#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>
#include <vector>

#include <asyffi.h>

namespace AsyFfiHelpers::Item
{
/**
 * Maps a C++ type to functions that read/write it from/to an {@link IAsyItem}.
 *
 * Every supported specialization provides the context-aware pair used by
 * {@link AsyFfi::toItem}/{@link AsyFfi::fromItem}:
 *   static T    getItem(IAsyContext* ctx, IAsyItem const* item);
 *   static void setItem(IAsyContext* ctx, IAsyItem* item, T const& value);
 *
 * The scalar specializations additionally keep a context-free pair (used by the
 * pre-existing helpers in item.h/args.h/array.h). Types that Asymptote must
 * allocate (std::string, std::vector<T>) only offer the context-aware pair,
 * since they need the {@link IAsyContext} (the host) to allocate Asy-side
 * storage.
 *
 * Specializing {@link ItemFunctions} for a new type is the single extension
 * point shared by value conversion (this header) and, later, typed proxies.
 */
template<typename T>
class ItemFunctions
{
    static_assert(
            std::false_type::value,
            "AsyFfiHelpers::Item::ItemFunctions: no value conversion defined for "
            "this C++ type. Supported: int64_t, int, bool, double, std::string, "
            "std::vector<T>."
    );
};

template<>
class ItemFunctions<int64_t>
{
public:
    static int64_t getItem(IAsyItem* item);

    static void setItem(IAsyItem* item, int64_t value);

    // Context-aware (used by AsyFfi::toItem/fromItem); context unused for scalars.
    static int64_t getItem(IAsyContext* ctx, IAsyItem const* item);

    static void setItem(IAsyContext* ctx, IAsyItem* item, int64_t value);
};

// Convenience: Asy Int is 64-bit, but int is accepted and mapped onto it.
template<>
class ItemFunctions<int>
{
public:
    static int getItem(IAsyContext* ctx, IAsyItem const* item);

    static void setItem(IAsyContext* ctx, IAsyItem* item, int value);
};

template<>
class ItemFunctions<bool>
{
public:
    static bool getItem(IAsyItem* item);

    static void setItem(IAsyItem* item, bool value);

    static bool getItem(IAsyContext* ctx, IAsyItem const* item);

    static void setItem(IAsyContext* ctx, IAsyItem* item, bool value);
};

template<>
class ItemFunctions<double>
{
public:
    static double getItem(IAsyItem* item);

    static void setItem(IAsyItem* item, double value);

    static double getItem(IAsyContext* ctx, IAsyItem const* item);

    static void setItem(IAsyContext* ctx, IAsyItem* item, double value);
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

// std::string: Asy must allocate the backing string, hence context-aware only.
template<>
class ItemFunctions<std::string>
{
public:
    static std::string getItem(IAsyContext* ctx, IAsyItem const* item);

    static void setItem(IAsyContext* ctx, IAsyItem* item, std::string const& value);
};

// std::vector<T>: Asy must allocate the backing array; each element is
// converted recursively via ItemFunctions<T>.
template<typename T>
class ItemFunctions<std::vector<T>>
{
public:
    static std::vector<T> getItem(IAsyContext* ctx, IAsyItem const* item)
    {
        IAsyArray* arr = static_cast<IAsyArray*>(item->asRawPointer());
        size_t const n = arr->getSize();
        std::vector<T> result;
        result.reserve(n);
        for (size_t i = 0; i < n; ++i)
        {
            result.push_back(ItemFunctions<T>::getItem(ctx, arr->getItem(i)));
        }
        return result;
    }

    static void setItem(IAsyContext* ctx, IAsyItem* item, std::vector<T> const& value)
    {
        // createNewArray(0) starts empty; pushAndReturnBlankItem then grows the
        // array to exactly value.size() elements.
        IAsyArray* arr = ctx->createNewArray(0);
        for (auto const& elem : value)
        {
            ItemFunctions<T>::setItem(ctx, arr->pushAndReturnBlankItem(), elem);
        }
        item->setRawPointer(arr);
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

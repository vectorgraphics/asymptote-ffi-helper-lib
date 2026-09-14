#pragma once
#include <asyffi.h>
#include <asyffihelpers/itemFnStructs.h>

namespace AsyFfiHelpers::Array
{

/** Wrapper class for an Asymptote array. This class supports [] indexing */
template<typename T>
class ArrayWrapper
{
public:
    ArrayWrapper(IAsyArray* array) : backingArray(array)
    {
    }

    /** Gets a specified item at index i. */
    Item::ItemWrapper<T> operator[](size_t const& i)
    {
        return Item::ItemWrapper<T>(backingArray->getItem(i));
    }

    /** Gets the size of the array. */
    [[nodiscard]]
    size_t size() const
    {
        return backingArray->getSize();
    }

    /** Whether the array is empty. */
    [[nodiscard]]
    bool isEmpty() const
    {
        return backingArray->getSize() > 0;
    }

    /** Returns the underlying pointer to the Asymptote array object. */
    [[nodiscard]]
    IAsyArray* getBackingArray() const
    {
        return backingArray;
    }

    operator IAsyArray*() const
    {
        return backingArray;
    }

private:
    IAsyArray* backingArray;
};

} // namespace AsyFfiHelpers::Array

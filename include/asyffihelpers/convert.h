
#pragma once

#include <asyffi.h>
#include <asyffihelpers/itemFnStructs.h>

/**
 * @file convert.h
 * @brief Convert C++ values across the FFI boundary via {@link IAsyItem}.
 *
 * {@link AsyFfi::toItem} writes a C++ value into an Asymptote item and
 * {@link AsyFfi::fromItem} reads a C++ value back out of one. Both are thin
 * dispatchers over the per-type specializations in
 * {@link AsyFfiHelpers::Item::ItemFunctions<T>} (mirroring it), so a single
 * extension point handles scalars, strings, and arrays -- and later, typed
 * proxies.
 */
namespace AsyFfi
{

/**
 * Writes the C++ value into an Asymptote item.
 *
 * The {@link IAsyContext} (the Asymptote host) is required for types Asymptote
 * must allocate (std::string, std::vector<T>); it is ignored for scalars.
 *
 * @tparam T one of: double, int64_t, int, bool, std::string, std::vector<T>.
 * @param ctx the Asymptote host context (used to allocate Asy-side storage).
 * @param item the item to write into.
 * @param value the C++ value to write.
 */
template<typename T>
void toItem(IAsyContext* ctx, IAsyItem* item, T const& value)
{
    AsyFfiHelpers::Item::ItemFunctions<T>::setItem(ctx, item, value);
}

/**
 * Reads a C++ value out of an Asymptote item.
 *
 * @tparam T one of: double, int64_t, int, bool, std::string, std::vector<T>.
 * @param ctx the Asymptote host context (used to read Asy-side storage).
 * @param item the item to read from.
 * @return the C++ value held by the item.
 */
template<typename T>
[[nodiscard]]
T fromItem(IAsyContext* ctx, IAsyItem const* item)
{
    return AsyFfiHelpers::Item::ItemFunctions<T>::getItem(ctx, item);
}

} // namespace AsyFfi


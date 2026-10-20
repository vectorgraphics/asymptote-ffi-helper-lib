
#pragma once

#include <asyffi.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>
#include <vector>

namespace AsyFfi
{

namespace detail
{

/**
 * Base for {@link TypeInfoOf} specializations that carry only a base type
 * (i.e. no {@code extraData}).
 *
 * The produced {@link Asy::TypeInfo} is a {@code static constexpr} (hence
 * inline, static-storage) member, so its address is stable for the lifetime of
 * the process. This matters because {@link Asy::TypeInfo} for arrays holds a
 * pointer (into {@code extraData.arrayTypeInfo.typeOfItem}) to another
 * TypeInfo, and Asymptote reads that metadata at registration time.
 *
 * @tparam Base the {@link Asy::BaseTypes} this type corresponds to.
 */
template<Asy::BaseTypes Base>
struct Basic
{
    static constexpr Asy::TypeInfo value{.baseType = Base, .extraData = {}};
};

} // namespace detail

/**
 * @brief Maps a C++ type to the corresponding {@link Asy::TypeInfo}.
 *
 * {@code TypeInfoOf<T>::value} is a {@code static constexpr}
 * {@link Asy::TypeInfo} describing T's Asymptote type.
 *
 * Supported types:
 * - {@code void}          -> {@link Asy::BaseTypes::Void}
 * - {@code double}        -> {@link Asy::BaseTypes::Real}
 * - {@code int64_t}       -> {@link Asy::BaseTypes::Integer} (Asy {@code Int}; preferred)
 * - {@code int}           -> {@link Asy::BaseTypes::Integer} (convenience)
 * - {@code bool}          -> {@link Asy::BaseTypes::Boolean}
 * - {@code std::string}   -> {@link Asy::BaseTypes::Str}
 * - {@code std::vector<T>}(T supported) -> {@link Asy::BaseTypes::ArrayType} (1-D array of T)
 *
 * For {@code std::vector<T>}, {@code value.baseType} is
 * {@link Asy::BaseTypes::ArrayType}, {@code value.extraData.arrayTypeInfo.dimension} is
 * {@code 1}, and {@code value.extraData.arrayTypeInfo.typeOfItem} points to
 * {@code TypeInfoOf<T>::value} (also in static storage), so the pointer remains
 * valid for the lifetime of the process.
 *
 * @note Use {@code int64_t} rather than {@code int} to match Asymptote's
 * 64-bit {@code Int}; {@code int} is accepted only as a convenience.
 *
 * @note This trait is deliberately open to extension: proxy-type
 * specializations (e.g. {@code asy::pair}, {@code asy::path}, ...) can be added
 * later by specializing {@link TypeInfoOf} for those types.
 */
template<typename T, typename = void>
struct TypeInfoOf
{
    static_assert(
        std::false_type::value,
        "AsyFfi::TypeInfoOf: no Asymptote type mapping for this C++ type. "
        "Supported: void, double, int64_t, int, bool, std::string, std::vector<T>."
    );
};

template<>
struct TypeInfoOf<void> : detail::Basic<Asy::BaseTypes::Void>
{
};

template<>
struct TypeInfoOf<double> : detail::Basic<Asy::BaseTypes::Real>
{
};

template<>
struct TypeInfoOf<int64_t> : detail::Basic<Asy::BaseTypes::Integer>
{
};

// Convenience alias: Asy Int is 64-bit, so int64_t is the canonical mapping.
// int is also mapped to Integer for convenience, but int64_t is preferred.
template<>
struct TypeInfoOf<int> : detail::Basic<Asy::BaseTypes::Integer>
{
};

template<>
struct TypeInfoOf<bool> : detail::Basic<Asy::BaseTypes::Boolean>
{
};

template<>
struct TypeInfoOf<std::string> : detail::Basic<Asy::BaseTypes::Str>
{
};

template<typename T>
struct TypeInfoOf<std::vector<T>>
{
    // 1-D array of T. extraData.arrayTypeInfo.typeOfItem points to the element
    // TypeInfo, which is stored in static (constexpr) storage so the pointer is
    // valid for the lifetime of the process.
    static constexpr Asy::TypeInfo value{
        .baseType = Asy::BaseTypes::ArrayType,
        .extraData = {
            .arrayTypeInfo = {.typeOfItem = &TypeInfoOf<T>::value, .dimension = 1}
        }
    };
};

/**
 * @brief Returns the {@link Asy::TypeInfo} for the C++ type T.
 *
 * The returned {@link Asy::TypeInfo} is a copy of {@code TypeInfoOf<T>::value}.
 * For array types the copy's {@code extraData.arrayTypeInfo.typeOfItem} still
 * points to {@code TypeInfoOf<T>::value}, which lives in static storage and is
 * therefore valid for the lifetime of the process.
 *
 * @tparam T a supported C++ type (see {@link TypeInfoOf}).
 */
template<typename T>
[[nodiscard]]
constexpr Asy::TypeInfo makeTypeInfo()
{
    return TypeInfoOf<T>::value;
}

} // namespace AsyFfi



#pragma once

#include <asyffi.h>

#include <cstdint>
#include <type_traits>

/**
 * @file values.h
 * @brief Typed proxy types for structural Asy values (pair, triple, path, ...).
 *
 * These proxies sit between raw `IAsy*` pointers and the user's C++ code, so
 * the common "build a path" / "return a pair" cases read like plain C++.
 *
 * Each proxy:
 * - carries the data (by value for tuples, by pointer for opaque handles);
 * - has a {@link TypeInfoOf} specialization (Phase 1 trait);
 * - has an {@link ItemFunctions} specialization (Phase 2 conversion);
 * - participates in {@link AsyFfi::toItem} / {@link AsyFfi::fromItem}.
 */
namespace AsyFfi
{

// ============================================================================
// pair
// ============================================================================

/**
 * Proxy for Asymptote's `pair` type (a 2D point / complex number).
 *
 * Holds two `double` components by value so a lambda can write:
 * @code
 * return asy::pair{(a.x + b.x) / 2, (a.y + b.y) / 2};
 * @endcode
 */
struct pair
{
    double x{};
    double y{};

    // Default-constructible and brace-initializable.
    constexpr pair() = default;
    constexpr pair(double x_, double y_): x(x_), y(y_) {}

    // Natural operators (kept minimal).
    constexpr pair operator+(pair const& o) const { return {x + o.x, y + o.y}; }
    constexpr pair operator-(pair const& o) const { return {x - o.x, y - o.y}; }
    pair operator*(double s) const { return {x * s, y * s}; }
    friend pair operator*(double s, pair const& p) { return {p.x * s, p.y * s}; }
};

// ============================================================================
// triple
// ============================================================================

/**
 * Proxy for Asymptote's `triple` type (a 3D point).
 */
struct triple
{
    double x{};
    double y{};
    double z{};

    constexpr triple() = default;
    constexpr triple(double x_, double y_, double z_): x(x_), y(y_), z(z_) {}

    constexpr triple operator+(triple const& o) const
    {
        return {x + o.x, y + o.y, z + o.z};
    }
    constexpr triple operator-(triple const& o) const
    {
        return {x - o.x, y - o.y, z - o.z};
    }
    triple operator*(double s) const { return {x * s, y * s, z * s}; }
    friend triple operator*(double s, triple const& p) { return {p.x * s, p.y * s, p.z * s}; }
};

// ============================================================================
// pointer-backed proxies (path, path3, picture, pen, transform)
// ============================================================================

/**
 * Proxy for Asymptote's `transform` type.
 * Wraps a non-owning `IAsyTransform*`.
 */
struct transform
{
    IAsyTransform* ptr{};

    constexpr transform() = default;
    constexpr explicit transform(IAsyTransform* p): ptr(p) {}

    // Apply transform to a pair: (u,v) -> (x + xx*u + xy*v, y + yx*u + yy*v)
    pair operator*(pair const& p) const
    {
        if (ptr == nullptr)
            return p;
        double const x  = ptr->getIndexedValue(0);
        double const y  = ptr->getIndexedValue(1);
        double const xx = ptr->getIndexedValue(2);
        double const xy = ptr->getIndexedValue(3);
        double const yx = ptr->getIndexedValue(4);
        double const yy = ptr->getIndexedValue(5);
        return {x + xx * p.x + xy * p.y, y + yx * p.x + yy * p.y};
    }
};

/**
 * Proxy for Asymptote's `path` type (2D path).
 * Wraps a non-owning `IAsyPath*`.
 */
struct path
{
    IAsyPath* ptr{};

    constexpr path() = default;
    constexpr explicit path(IAsyPath* p): ptr(p) {}
};

/**
 * Proxy for Asymptote's `path3` type (3D path).
 * Wraps a non-owning `IAsyPath3*`.
 */
struct path3
{
    IAsyPath3* ptr{};

    constexpr path3() = default;
    constexpr explicit path3(IAsyPath3* p): ptr(p) {}
};

/**
 * Proxy for Asymptote's `picture` type (internally a `frame`).
 * Wraps a non-owning `IAsyPicture*`.
 */
struct picture
{
    IAsyPicture* ptr{};

    constexpr picture() = default;
    constexpr explicit picture(IAsyPicture* p): ptr(p) {}
};

/**
 * Proxy for Asymptote's `pen` type.
 * Wraps a non-owning `IAsyPen*`.
 */
struct pen
{
    IAsyPen* ptr{};

    constexpr pen() = default;
    constexpr explicit pen(IAsyPen* p): ptr(p) {}
};

/**
 * Proxy for a user-defined Asymptote struct (record) *instance*.
 *
 * Wraps a non-owning {@link IAsyVarFrame*} -- the frame that holds one instance
 * of a struct. Unlike the other proxies, {@code record} has no
 * {@link TypeInfoOf} specialization: the Asymptote type of a struct is runtime
 * data (a specific {@link IAsyRecord*}) and cannot be a {@code static constexpr}
 * {@link Asy::TypeInfo}. Struct-typed signatures therefore still use the
 * escape-hatch {@link AsyFfi::registerFunction} overload; this proxy provides
 * only the {@link ItemFunctions} value conversion so {@code AsyFfi::toItem}/
 * {@code AsyFfi::fromItem} work on struct instances.
 *
 * Inside a struct-returning or struct-argument function, obtain a {@code record}
 * from an {@link IAsyItem} and reach the frame via {@code ptr}:
 * @code
 * AsyFfi::record r = AsyFfi::fromItem<AsyFfi::record>(ctx, item);
 * mr.set(ctx, r.ptr, "x", 10);   // ManagedRecord::set on the frame
 * @endcode
 */
struct record
{
    IAsyVarFrame* ptr{};

    constexpr record() = default;
    constexpr explicit record(IAsyVarFrame* p): ptr(p) {}
};

// ============================================================================
// TypeInfoOf specializations (Phase 1)
// ============================================================================

template<>
struct TypeInfoOf<pair>
{
    static constexpr Asy::TypeInfo value{.baseType = Asy::BaseTypes::Pair, .extraData = {}};
};

template<>
struct TypeInfoOf<triple>
{
    static constexpr Asy::TypeInfo value{.baseType = Asy::BaseTypes::Triple, .extraData = {}};
};

template<>
struct TypeInfoOf<transform>
{
    static constexpr Asy::TypeInfo value{.baseType = Asy::BaseTypes::Transform, .extraData = {}};
};

template<>
struct TypeInfoOf<path>
{
    static constexpr Asy::TypeInfo value{.baseType = Asy::BaseTypes::Path, .extraData = {}};
};

template<>
struct TypeInfoOf<path3>
{
    static constexpr Asy::TypeInfo value{.baseType = Asy::BaseTypes::Path3, .extraData = {}};
};

template<>
struct TypeInfoOf<picture>
{
    static constexpr Asy::TypeInfo value{.baseType = Asy::BaseTypes::Picture, .extraData = {}};
};

template<>
struct TypeInfoOf<pen>
{
    static constexpr Asy::TypeInfo value{.baseType = Asy::BaseTypes::Pen, .extraData = {}};
};

// ============================================================================
// ItemFunctions specializations (Phase 2)
// ============================================================================

// ItemFunctions is declared in itemFnStructs.h (already included via
// asyffihelper.h -> args.h). The #pragma once makes this a no-op but ensures
// the primary template is visible here.
#include <asyffihelpers/itemFnStructs.h>

} // namespace AsyFfi

namespace AsyFfiHelpers::Item
{

// --- pair ---

template<>
class ItemFunctions<AsyFfi::pair>
{
public:
    static AsyFfi::pair getItem(IAsyContext* ctx, IAsyItem const* item)
    {
        IAsyTuple* t = static_cast<IAsyTuple*>(item->asRawPointer());
        return AsyFfi::pair{t->getIndexedValue(0), t->getIndexedValue(1)};
    }

    static void setItem(IAsyContext* ctx, IAsyItem* item, AsyFfi::pair const& value)
    {
        IAsyTuple* t = ctx->createPair(value.x, value.y);
        item->setRawPointer(t);
    }
};

// --- triple ---

template<>
class ItemFunctions<AsyFfi::triple>
{
public:
    static AsyFfi::triple getItem(IAsyContext* ctx, IAsyItem const* item)
    {
        IAsyTuple* t = static_cast<IAsyTuple*>(item->asRawPointer());
        return AsyFfi::triple{t->getIndexedValue(0), t->getIndexedValue(1), t->getIndexedValue(2)};
    }

    static void setItem(IAsyContext* ctx, IAsyItem* item, AsyFfi::triple const& value)
    {
        IAsyTuple* t = ctx->createTriple(value.x, value.y, value.z);
        item->setRawPointer(t);
    }
};

// --- transform ---

template<>
class ItemFunctions<AsyFfi::transform>
{
public:
    static AsyFfi::transform getItem(IAsyContext*, IAsyItem const* item)
    {
        return AsyFfi::transform(static_cast<IAsyTransform*>(item->asRawPointer()));
    }

    static void setItem(IAsyContext*, IAsyItem* item, AsyFfi::transform const& value)
    {
        item->setRawPointer(value.ptr);
    }
};

// --- path ---

template<>
class ItemFunctions<AsyFfi::path>
{
public:
    static AsyFfi::path getItem(IAsyContext*, IAsyItem const* item)
    {
        return AsyFfi::path(static_cast<IAsyPath*>(item->asRawPointer()));
    }

    static void setItem(IAsyContext*, IAsyItem* item, AsyFfi::path const& value)
    {
        item->setRawPointer(value.ptr);
    }
};

// --- path3 ---

template<>
class ItemFunctions<AsyFfi::path3>
{
public:
    static AsyFfi::path3 getItem(IAsyContext*, IAsyItem const* item)
    {
        return AsyFfi::path3(static_cast<IAsyPath3*>(item->asRawPointer()));
    }

    static void setItem(IAsyContext*, IAsyItem* item, AsyFfi::path3 const& value)
    {
        item->setRawPointer(value.ptr);
    }
};

// --- picture ---

template<>
class ItemFunctions<AsyFfi::picture>
{
public:
    static AsyFfi::picture getItem(IAsyContext*, IAsyItem const* item)
    {
        return AsyFfi::picture(static_cast<IAsyPicture*>(item->asRawPointer()));
    }

    static void setItem(IAsyContext*, IAsyItem* item, AsyFfi::picture const& value)
    {
        item->setRawPointer(value.ptr);
    }
};

// --- pen ---

template<>
class ItemFunctions<AsyFfi::pen>
{
public:
    static AsyFfi::pen getItem(IAsyContext*, IAsyItem const* item)
    {
        return AsyFfi::pen(static_cast<IAsyPen*>(item->asRawPointer()));
    }

    static void setItem(IAsyContext*, IAsyItem* item, AsyFfi::pen const& value)
    {
        item->setRawPointer(value.ptr);
    }
};

// --- record (user-defined struct instance) ---
//
// A struct instance is stored in an item as a raw pointer to its
// IAsyVarFrame, so the conversion is a plain pointer round-trip (no Asy
// allocation, context unused).

template<>
class ItemFunctions<AsyFfi::record>
{
public:
    static AsyFfi::record getItem(IAsyContext*, IAsyItem const* item)
    {
        return AsyFfi::record(static_cast<IAsyVarFrame*>(item->asRawPointer()));
    }

    static void setItem(IAsyContext*, IAsyItem* item, AsyFfi::record const& value)
    {
        item->setRawPointer(value.ptr);
    }
};

} // namespace AsyFfiHelpers::Item


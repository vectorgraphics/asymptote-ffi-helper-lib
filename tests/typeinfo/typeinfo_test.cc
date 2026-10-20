
/**
 * Unit test for the AsyFfi::TypeInfoOf<T> type-mapping trait.
 *
 * The assertions are written so that as many as possible are checked at compile
 * time (via static_assert against the constexpr TypeInfoOf<T>::value), and the
 * remainder are double-checked at runtime in main(). main() returns 0 on
 * success and non-zero on failure so that ctest reports a failure.
 */

#include <asyffihelper.h>

#include <cstdint>
#include <string>
#include <type_traits>
#include <vector>

// ---------------------------------------------------------------------------
// Compile-time checks (constexpr TypeInfoOf<T>::value)
// ---------------------------------------------------------------------------

static_assert(
    AsyFfi::TypeInfoOf<void>::value.baseType == Asy::BaseTypes::Void,
    "void must map to Asy::BaseTypes::Void"
);
static_assert(
    AsyFfi::TypeInfoOf<double>::value.baseType == Asy::BaseTypes::Real,
    "double must map to Asy::BaseTypes::Real"
);
static_assert(
    AsyFfi::TypeInfoOf<int64_t>::value.baseType == Asy::BaseTypes::Integer,
    "int64_t must map to Asy::BaseTypes::Integer (Asy Int)"
);
static_assert(
    AsyFfi::TypeInfoOf<int>::value.baseType == Asy::BaseTypes::Integer,
    "int must map to Asy::BaseTypes::Integer (convenience)"
);
static_assert(
    AsyFfi::TypeInfoOf<bool>::value.baseType == Asy::BaseTypes::Boolean,
    "bool must map to Asy::BaseTypes::Boolean"
);
static_assert(
    AsyFfi::TypeInfoOf<std::string>::value.baseType == Asy::BaseTypes::Str,
    "std::string must map to Asy::BaseTypes::Str"
);

// std::vector<double> -> 1-D Asy array of Real, with the element pointer
// pointing into static storage (TypeInfoOf<double>::value).
static_assert(
    AsyFfi::TypeInfoOf<std::vector<double>>::value.baseType
    == Asy::BaseTypes::ArrayType,
    "std::vector<double> must map to Asy::BaseTypes::ArrayType"
);
static_assert(
    AsyFfi::TypeInfoOf<std::vector<double>>::value.extraData.arrayTypeInfo.dimension == 1,
    "std::vector<double> must be a 1-D array"
);
static_assert(
    AsyFfi::TypeInfoOf<std::vector<double>>::value.extraData.arrayTypeInfo.typeOfItem
    == &AsyFfi::TypeInfoOf<double>::value,
    "element TypeInfo pointer must be the static TypeInfoOf<double>::value"
);

// makeTypeInfo must agree with TypeInfoOf::value at compile time.
static_assert(
    AsyFfi::makeTypeInfo<double>().baseType == Asy::BaseTypes::Real,
    "makeTypeInfo<double>() must be Real"
);
static_assert(
    AsyFfi::makeTypeInfo<std::vector<double>>().baseType == Asy::BaseTypes::ArrayType,
    "makeTypeInfo<std::vector<double>>() must be ArrayType"
);

// ---------------------------------------------------------------------------
// Runtime checks (defensive; the above already catch most regressions)
// ---------------------------------------------------------------------------

namespace
{

int failures = 0;

template<typename T, Asy::BaseTypes Expected>
void expectBase()
{
    if (AsyFfi::makeTypeInfo<T>().baseType != Expected)
    {
        ++failures;
    }
}

} // namespace

int main()
{
    expectBase<void, Asy::BaseTypes::Void>();
    expectBase<double, Asy::BaseTypes::Real>();
    expectBase<int64_t, Asy::BaseTypes::Integer>();
    expectBase<int, Asy::BaseTypes::Integer>();
    expectBase<bool, Asy::BaseTypes::Boolean>();
    expectBase<std::string, Asy::BaseTypes::Str>();

    // std::vector<double>
    const auto vec = AsyFfi::makeTypeInfo<std::vector<double>>();
    if (vec.baseType != Asy::BaseTypes::ArrayType)
    {
        ++failures;
    }
    if (vec.extraData.arrayTypeInfo.dimension != 1)
    {
        ++failures;
    }
    if (
        vec.extraData.arrayTypeInfo.typeOfItem == nullptr
        || vec.extraData.arrayTypeInfo.typeOfItem->baseType != Asy::BaseTypes::Real
    )
    {
        ++failures;
    }

    return failures == 0 ? 0 : 1;
}


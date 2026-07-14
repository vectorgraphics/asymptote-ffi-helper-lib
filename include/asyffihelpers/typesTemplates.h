#pragma once
#include "asyffi.h"
#include <cstdint>

#define ASYFFI_TYPE_TEMPLATE_DECLARE_BASIC_TYPE(TYPE_NAME) \
    template<> \
    struct Basic<TYPE_NAME> \
    { \
        static Asy::TypeInfo const value; \
    }

namespace AsyFfiHelpers::TypeObjects
{

// basic types
template<typename T>
struct Basic
{
};

ASYFFI_TYPE_TEMPLATE_DECLARE_BASIC_TYPE(int64_t);
ASYFFI_TYPE_TEMPLATE_DECLARE_BASIC_TYPE(double);
ASYFFI_TYPE_TEMPLATE_DECLARE_BASIC_TYPE(bool);
ASYFFI_TYPE_TEMPLATE_DECLARE_BASIC_TYPE(IAsyPen);
ASYFFI_TYPE_TEMPLATE_DECLARE_BASIC_TYPE(IAsyTransform);
ASYFFI_TYPE_TEMPLATE_DECLARE_BASIC_TYPE(IAsyPath);
ASYFFI_TYPE_TEMPLATE_DECLARE_BASIC_TYPE(IAsyPath3);

struct String
{
    static Asy::TypeInfo const value;
};

// declarations of tuples (pairs, triples, transform)
template<size_t tupleSize>
struct Tuple
{
};

template<>
struct Tuple<2>
{
    static Asy::TypeInfo const value;
};

template<>
struct Tuple<3>
{
    static Asy::TypeInfo const value;
};

template<>
struct Tuple<6>
{
    static Asy::TypeInfo const value;
};

// array construction
template<typename TypeObject, size_t dimension = 1>
struct Array
{
private:
    static inline Asy::TypeInfo const baseType = TypeObject::value;

public:
    static inline Asy::TypeInfo const value = {
        .baseType = Asy::BaseTypes::ArrayType,
        .extraData = {.arrayTypeInfo = {.typeOfItem = &baseType, .dimension = dimension}}
    };
};

} // namespace AsyFfiHelpers::TypeObjects

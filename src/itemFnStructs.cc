
#include "asyffihelpers/itemFnStructs.h"

namespace AsyFfiHelpers::Item
{
int64_t ItemFunctions<int64_t>::getItem(IAsyItem* item)
{
    return item->asInt64();
}
void ItemFunctions<int64_t>::setItem(IAsyItem* item, int64_t const value)
{
    item->setInt64Value(value);
}
bool ItemFunctions<bool>::getItem(IAsyItem* item)
{
    return item->asBoolean();
}
void ItemFunctions<bool>::setItem(IAsyItem* item, bool const value)
{
    item->setBooleanValue(value);
}
double ItemFunctions<double>::getItem(IAsyItem* item)
{
    return item->asDouble();
}
void ItemFunctions<double>::setItem(IAsyItem* item, double const value)
{
    item->setDoubleValue(value);
}

int64_t ItemFunctions<int64_t>::getItem(IAsyContext*, IAsyItem const* item)
{
    return item->asInt64();
}
void ItemFunctions<int64_t>::setItem(IAsyContext*, IAsyItem* item, int64_t const value)
{
    item->setInt64Value(value);
}
int ItemFunctions<int>::getItem(IAsyContext*, IAsyItem const* item)
{
    return static_cast<int>(item->asInt64());
}
void ItemFunctions<int>::setItem(IAsyContext*, IAsyItem* item, int const value)
{
    item->setInt64Value(value);
}
bool ItemFunctions<bool>::getItem(IAsyContext*, IAsyItem const* item)
{
    return item->asBoolean();
}
void ItemFunctions<bool>::setItem(IAsyContext*, IAsyItem* item, bool const value)
{
    item->setBooleanValue(value);
}
double ItemFunctions<double>::getItem(IAsyContext*, IAsyItem const* item)
{
    return item->asDouble();
}
void ItemFunctions<double>::setItem(IAsyContext*, IAsyItem* item, double const value)
{
    item->setDoubleValue(value);
}
std::string ItemFunctions<std::string>::getItem(IAsyContext* ctx, IAsyItem const* item)
{
    // An Asymptote string is an opaque void* handle (the THAsyString typedef).
    void* const str = item->asRawPointer();
    size_t const len = ctx->getStringLength(str);
    std::string result(len, '\0');
    if (len > 0)
    {
        // copyString writes the len characters plus the null terminator.
        ctx->copyString(str, result.data(), len + 1);
    }
    return result;
}
void ItemFunctions<std::string>::setItem(IAsyContext* ctx, IAsyItem* item, std::string const& value)
{
    void* const str = ctx->createNewAsyStringSized(value.data(), value.size());
    item->setRawPointer(str);
}
} // namespace AsyFfiHelper::Item


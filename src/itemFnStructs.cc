#include "asyffihelpers/itemFnStructs.h"

namespace AsyFfiHelpers::Item
{
int64_t ItemFunctions<long long>::getItem(IAsyItem* item)
{
    return item->asInt64();
}
void ItemFunctions<long long>::setItem(IAsyItem* item, int64_t const value)
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
} // namespace AsyFfiHelper::Item

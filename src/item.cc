#include <asyffihelpers/item.h>

namespace AsyFfiHelpers::Item
{
template<>
void setItem<int64_t>(IAsyItem* item, int64_t const& value)
{
    item->setInt64Value(value);
}

template<>
void setItem<double>(IAsyItem* item, double const& value)
{
    item->setDoubleValue(value);
}

template<>
void setItem<bool>(IAsyItem* item, bool const& value)
{
    item->setBooleanValue(value);
}

template<>
int64_t getItem<int64_t>(IAsyItem const* item)
{
    return item->asInt64();
}

template<>
double getItem<double>(IAsyItem const* item)
{
    return item->asDouble();
}

template<>
bool getItem<bool>(IAsyItem const* item)
{
    return item->asBoolean();
}

} // namespace AsyFfiHelpers::Item

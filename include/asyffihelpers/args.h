#pragma once

#include "asyffi.h"
#include <tuple>
#include "asyffihelpers/itemFnStructs.h"

namespace AsyFfiHelpers::Args
{

/**
 * Unpacks arguments into tuples of objects (int64_t, double, bool) or
 * pointers to Asymptote objects
 */
template<typename... ArgumentTypes>
std::tuple<ArgumentTypes...> argsToTuple(IAsyArgs* args)
{
    size_t index = 0;

    return {Item::ItemFunctions<ArgumentTypes>::getItem(args->getNumberedArg(index++))...};
}

} // namespace AsyFfiHelpers::Args

#pragma once

#include <asyffihelpers/contextFuncs.h>
#include <asyffihelpers/args.h>

/** Add this macro to declare AsyContextHelper with name ctxHelper in a plugin function*/
#define ASYFFI_CONTEXT_HELPER AsyFfiHelpers::Context::AsyContextHelper ctxHelper(context);

/**
 * Shorthand for container.size(), container.data()
 */
#define STD_CONTAINER_SIZE_AND_DATA(container) container.size(), container.data()

/** Shorthand for 0, nullptr */
#define EMPTY_STD_CONTAINER_SIZE_AND_DATA 0, nullptr


/** */
#define UNPACK_ARGS(...) AsyFfiHelpers::Args::argsToTuple<__VA_ARGS__>(args);

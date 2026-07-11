#pragma once

/** Add this macro to declare AsyContextHelper with name ctxHelper in a plugin function*/
#define ASYFFI_CONTEXT_HELPER AsyFfiHelpers::Item::AsyContextHelper ctxHelper(context);

/**
 * Shorthand for container.size(), container.data()
 */
#define STD_CONTAINER_SIZE_AND_DATA(container) container.size(), container.data()

/** Shorthand for 0, nullptr */
#define EMPTY_STD_CONTAINER_SIZE_AND_DATA 0, nullptr

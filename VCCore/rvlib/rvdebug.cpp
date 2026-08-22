#include "rvconfig.h"
#include "utl/abilities/Loggable.h"
#include <type_traits>

#ifndef NDEBUG

namespace retro::vault::debug {

/* The flag descriptor tables, generated from the X-macro lists in rvdebug.h.
 * Each entry wraps its flag in a pair of captureless lambdas, so that flags
 * of different types can be read and written through a common interface.
 */

#define LOG_FLAG_ENTRY(name, dflt, help) \
    { #name, help, false, \
      []() -> long { return (long)name; }, \
      [](long value) { name = LogLevel(value); } },

#define DEBUG_FLAG_ENTRY(type, name, dflt, help) \
    { #name, help, std::is_same_v<type, bool>, \
      []() -> long { return (long)name; }, \
      [](long value) { name = (type)value; } },

const std::vector<FlagInfo> logFlags = { RV_LOG_FLAGS(LOG_FLAG_ENTRY) };
const std::vector<FlagInfo> debugFlags = { RV_DEBUG_FLAGS(DEBUG_FLAG_ENTRY) };

#undef LOG_FLAG_ENTRY
#undef DEBUG_FLAG_ENTRY

}

#endif

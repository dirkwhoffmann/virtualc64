#include "rvconfig.h"
#include "utl/abilities/Loggable.h"

#define STR(x) #x
#define XSTR(x) STR(x)

#define DEBUG_CHANNEL(name, description) \
utl::LogChannel name = \
  ::retro::vault::name ? \
    utl::Loggable::subscribe(XSTR(name), std::optional<long>(7), description) : \
    utl::Loggable::subscribe(XSTR(name), std::optional<long>(std::nullopt), description);

namespace retro::vault::channel {

// Register the default channels
DEBUG_CHANNEL(NULLDEV,          "Message sink");
DEBUG_CHANNEL(STDERR,           "Standard error");

// Register a logging channel for each debug flag

// File systems
DEBUG_CHANNEL(FS_DEBUG,         "File systems");

// Images
DEBUG_CHANNEL(IMG_DEBUG,        "Disk images");

}

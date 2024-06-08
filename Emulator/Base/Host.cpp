// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// This FILE is dual-licensed. You are free to choose between:
//
//     - The GNU General Public License v3 (or any later version)
//     - The Mozilla Public License v2
//
// SPDX-License-Identifier: GPL-3.0-or-later OR MPL-2.0
// -----------------------------------------------------------------------------

#include "config.h"
#include "Host.h"
#include "Emulator.h"
#include "IOUtils.h"

namespace vc64 {

i64
Host::getOption(Option option) const
{
    switch (option) {

        case OPT_HOST_REFRESH_RATE:     return i64(config.refreshRate);
        case OPT_HOST_SAMPLE_RATE:      return i64(config.sampleRate);
        case OPT_HOST_FRAMEBUF_WIDTH:   return i64(config.frameBufferWidth);
        case OPT_HOST_FRAMEBUF_HEIGHT:  return i64(config.frameBufferHeight);

        default:
            fatalError;
    }
}

void
Host::checkOption(Option opt, i64 value)
{
    switch (opt) {

        case OPT_HOST_REFRESH_RATE:
        case OPT_HOST_SAMPLE_RATE:
        case OPT_HOST_FRAMEBUF_WIDTH:
        case OPT_HOST_FRAMEBUF_HEIGHT:
            return;

        default:
            throw(ERROR_OPT_UNSUPPORTED);
    }
}

void
Host::setOption(Option opt, i64 value)
{
    switch (opt) {

        case OPT_HOST_REFRESH_RATE:

            config.refreshRate = isize(value);
            return;

        case OPT_HOST_SAMPLE_RATE:

            config.sampleRate = isize(value);
            return;

        case OPT_HOST_FRAMEBUF_WIDTH:

            config.frameBufferWidth = isize(value);
            return;

        case OPT_HOST_FRAMEBUF_HEIGHT:

            config.frameBufferHeight = isize(value);
            return;

        default:
            fatalError;
    }
}

void
Host::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::Config) {

        dumpConfig(os);
    }
}

fs::path
Host::tmp() const
{
    SYNCHRONIZED

    static fs::path base;

    if (base.empty()) {

        // Use /tmp as default folder for temporary files
        base = "/tmp";

        // Open a file to see if we have write permissions
        std::ofstream logfile(base / "virtualc64.log");

        // If /tmp is not accessible, use a different directory
        if (!logfile.is_open()) {

            base = fs::temp_directory_path();
            logfile.open(base / "virtualc64.log");

            if (!logfile.is_open()) {

                throw Error(ERROR_DIR_NOT_FOUND);
            }
        }

        logfile.close();
        fs::remove(base / "virtualc64.log");
    }

    return base;
}

fs::path
Host::tmp(const string &name, bool unique) const
{
    auto base = tmp();
    auto result = base / name;

    // Make the file name unique if requested
    if (unique) result = fs::path(util::makeUniquePath(result.string()));

    return result;
}


}

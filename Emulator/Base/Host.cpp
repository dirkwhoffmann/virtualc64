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

        case OPT_HOST_REFRESH_RATE:     return i64(refreshRate);
        case OPT_HOST_SAMPLE_RATE:      return i64(sampleRate);
        case OPT_HOST_FRAMEBUF_WIDTH:   return i64(frameBufferWidth);
        case OPT_HOST_FRAMEBUF_HEIGHT:  return i64(frameBufferHeight);

        default:
            fatalError;
    }
}

void
Host::setOption(Option option, i64 value)
{
    switch (option) {

        case OPT_HOST_REFRESH_RATE:

            refreshRate = double(value);
            return;

        case OPT_HOST_SAMPLE_RATE:

            sampleRate = double(value);
            return;

        case OPT_HOST_FRAMEBUF_WIDTH:

            frameBufferWidth = isize(value);
            return;

        case OPT_HOST_FRAMEBUF_HEIGHT:

            frameBufferHeight = isize(value);
            return;

        default:
            fatalError;
    }
}

void
Host::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::State) {

        os << tab("Audio sample rate");
        os << flt(sampleRate) << " Hz" << std::endl;
        os << tab("Monitor refresh rate");
        os << flt(refreshRate) << " Hz" << std::endl;
        os << tab("Frame buffer size");
        os << dec(frameBufferWidth) << " x ";
        os << dec(frameBufferHeight) << " Texels" << std::endl;
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

                throw VC64Error(ERROR_DIR_NOT_FOUND);
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

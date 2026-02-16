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
#include "utl/io/Files.h"
#include "utl/support/Strings.h"
#include <unordered_set>

namespace vc64 {

i64
Host::getOption(Opt option) const
{
    switch (option) {

        case Opt::HOST_REFRESH_RATE:     return i64(config.refreshRate);
        case Opt::HOST_SAMPLE_RATE:      return i64(config.sampleRate);
        case Opt::HOST_FRAMEBUF_WIDTH:   return i64(config.frameBufferWidth);
        case Opt::HOST_FRAMEBUF_HEIGHT:  return i64(config.frameBufferHeight);

        default:
            fatalError;
    }
}

void
Host::checkOption(Opt opt, i64 value)
{
    switch (opt) {

        case Opt::HOST_REFRESH_RATE:
        case Opt::HOST_SAMPLE_RATE:
        case Opt::HOST_FRAMEBUF_WIDTH:
        case Opt::HOST_FRAMEBUF_HEIGHT:
            return;

        default:
            throw AppError(Fault::OPT_UNSUPPORTED);
    }
}

void
Host::setOption(Opt opt, i64 value)
{
    switch (opt) {

        case Opt::HOST_REFRESH_RATE:

            config.refreshRate = isize(value);
            return;

        case Opt::HOST_SAMPLE_RATE:

            config.sampleRate = isize(value);
            audioPort.setSampleRate(double(value));
            return;

        case Opt::HOST_FRAMEBUF_WIDTH:

            config.frameBufferWidth = isize(value);
            return;

        case Opt::HOST_FRAMEBUF_HEIGHT:

            config.frameBufferHeight = isize(value);
            return;

        default:
            fatalError;
    }
}

void
Host::resetConfigItems(const class Defaults &defaults, isize objid)
{
    // Don't reset any host properties
}

void
Host::_dump(Category category, std::ostream &os) const
{
    using namespace utl;

    if (category == Category::Config) {

        os << tab("Search path");
        os << searchPath << std::endl;

        dumpConfig(os);
    }
}

fs::path
Host::sanitize(const string &filename)
{
    auto toUtf8 = [&](u8 c) {
        
        if (c < 0x80) {
            return string(1, c);
        } else {
            return string(1, char( 0xC0 | (c >> 6))) + char(0x80 | (c & 0x3F));
        }
    };
    
    auto toHex = [&](u8 c) {
        
        std::ostringstream ss;
        ss << '%' << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << (int)c;
        return ss.str();
    };
    
    auto shouldEscape = [&](u8 c, isize i) {
        
        // Unhide hidden files
        if (c == '.' && i == 0) return true;
        
        // Escape the lower ASCII range
        if (c < 23) return true;
        
        // Escape special characters
        if (c == '<' || c == '>' || c == ':' || c == '"' || c == '\\') return true;
        if (c == '/' || c == '>' || c == '?' || c == '*') return true;
        
        // Don't escape everything else
        return false;
    };

    auto isReserved = [&](const string& name) {
        
        static const std::unordered_set<std::string> reserved {
            "CON", "PRN", "AUX", "NUL",
            "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
            "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"
        };
        
        return reserved.count(utl::uppercased(name)) > 0;
    };

    string result;
    
    // Convert characters one by one
    for (usize i = 0; i < filename.length(); i++) {

        auto u = u8(filename[i]);
        
        if (u > 127) {
            result += toUtf8(u);
        } else if (shouldEscape(u, i)) {
            result += toHex(u);
        } else {
            result += char(u);
        }
    }
    
    // Avoid reserved Windows names
    if (isReserved(result)) result = "__" + result;

    /*
    if (filename != result) {
        printf("sanitize: %s -> %s\n", filename.c_str(), result.c_str());
    }
    */
    
    return fs::path(result);
}

string
Host::unsanitize(const fs::path &filename)
{
    const auto &s = filename.string();
    const auto len = isize(s.length());
    
    auto isUtf8 = [&](isize i) {
        
        if (i + 2 >= len) return false;
        return u8(s[i]) >= 0xC0 && (u8(s[i + 1]) & 0xC0) == 0x80;
    };
    
    auto fromUtf8 = [&](isize i) {
        
        return (u8)((((u8)s[i] & 0x1F) << 6) | ((u8)s[i + 1] & 0x3F));
    };
    
    auto isHexDigit = [&](char c) {
        
        return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');
    };
    
    auto isHex = [&](isize i) {
        
        if (i + 3 >= len) return false;
        return s[i] == '%' && isHexDigit(s[i + 1]) && isHexDigit(s[i + 1]);
    };

    auto fromHex = [&](isize i) {
        
        return std::stoi(s.substr(i + 1, 2), nullptr, 16);
    };
    
    auto isReserved = [&]() {
        
        static const std::unordered_set<std::string> reserved {
            "CON", "PRN", "AUX", "NUL",
            "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
            "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"
        };
        
        if (s.rfind("__", 0) != 0) return false;
        return reserved.count(utl::uppercased(s.substr(2))) > 0;
    };

    std::string result;

    if (isReserved()) {

        // Restore the original reserved word
        result = s.substr(2);
        
    } else {
        
        // Convert characters one by one
        for (isize i = 0; i < len; i++) {
            
            if (isUtf8(i)) {
                result += (char)fromUtf8(i); i += 1;
            } else if (isHex(i)) {
                result += (char)fromHex(i); i += 2;
            } else {
                result += s[i];
            }
        }
    }
    
    /*
    if (filename.string() != result) {
        printf("unsanitize: %s -> %s\n", filename.string().c_str(), result.c_str());
    }
    */
    
    return result;
}

void
Host::setSearchPath(const fs::path &path)
{
    SYNCHRONIZED
    
    searchPath = path;
}

fs::path
Host::makeAbsolute(const fs::path &path) const
{
    SYNCHRONIZED

    printf("makeAbsolute: %s %s\n", path.string().c_str(), searchPath.string().c_str());
    return path.is_absolute() ? path : searchPath / path;
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

                throw IOError(IOError::DIR_NOT_FOUND);
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
    if (unique) result = fs::path(utl::makeUniquePath(result));

    return result;
}

}

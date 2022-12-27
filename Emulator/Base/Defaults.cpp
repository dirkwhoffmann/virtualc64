// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "Defaults.h"
#include "C64.h"
#include "StringUtils.h"

namespace vc64 {

Defaults::Defaults()
{
    setFallback(OPT_FPS_MODE, FPS_NATIVE);
    setFallback(OPT_FPS, 60);

    setFallback(OPT_POWER_GRID, GRID_STABLE_50HZ);

    setFallback(OPT_CIA_REVISION, MOS_6526);
    setFallback(OPT_TIMER_B_BUG, true);

    setFallback(OPT_VIC_REVISION, VICII_PAL_8565);
    setFallback(OPT_VIC_POWER_SAVE, true);
    setFallback(OPT_GRAY_DOT_BUG, true);
    setFallback(OPT_GLUE_LOGIC, GLUE_LOGIC_DISCRETE);
    setFallback(OPT_PALETTE, PALETTE_COLOR);
    setFallback(OPT_BRIGHTNESS, 50);
    setFallback(OPT_CONTRAST, 100);
    setFallback(OPT_SATURATION, 50);
    setFallback(OPT_HIDE_SPRITES, false);
    setFallback(OPT_SB_COLLISIONS, true);
    setFallback(OPT_SS_COLLISIONS, true);

    setFallback(OPT_DMA_DEBUG_ENABLE, false);
    setFallback(OPT_DMA_DEBUG_MODE, DMA_DISPLAY_MODE_FG_LAYER);
    setFallback(OPT_DMA_DEBUG_OPACITY, 0x80);
    setFallback(OPT_DMA_DEBUG_CHANNEL, 0, true);
    setFallback(OPT_DMA_DEBUG_CHANNEL, 1, true);
    setFallback(OPT_DMA_DEBUG_CHANNEL, 2, true);
    setFallback(OPT_DMA_DEBUG_CHANNEL, 3, true);
    setFallback(OPT_DMA_DEBUG_CHANNEL, 4, true);
    setFallback(OPT_DMA_DEBUG_CHANNEL, 5, true);
    setFallback(OPT_DMA_DEBUG_COLOR, 0, GpuColor(0xFF, 0x00, 0x00).abgr);
    setFallback(OPT_DMA_DEBUG_COLOR, 1, GpuColor(0xFF, 0xC0, 0x00).abgr);
    setFallback(OPT_DMA_DEBUG_COLOR, 2, GpuColor(0xFF, 0xFF, 0x00).abgr);
    setFallback(OPT_DMA_DEBUG_COLOR, 3, GpuColor(0x00, 0xFF, 0xFF).abgr);
    setFallback(OPT_DMA_DEBUG_COLOR, 4, GpuColor(0x00, 0xFF, 0x00).abgr);
    setFallback(OPT_DMA_DEBUG_COLOR, 5, GpuColor(0x00, 0x80, 0xFF).abgr);
    setFallback(OPT_CUT_LAYERS, 0xFF);
    setFallback(OPT_CUT_OPACITY, 0xFF);

    setFallback(OPT_SID_REVISION, MOS_8580);
    setFallback(OPT_SID_POWER_SAVE, false);
    setFallback(OPT_SID_FILTER, false);
    setFallback(OPT_SID_ENGINE, SIDENGINE_RESID);
    setFallback(OPT_SID_SAMPLING, SAMPLING_INTERPOLATE);
    setFallback(OPT_AUDVOLL, 50);
    setFallback(OPT_AUDVOLR, 50);

    setFallback(OPT_SID_ENABLE, 0, true);
    setFallback(OPT_SID_ENABLE, {1, 2, 3}, false);
    setFallback(OPT_SID_ADDRESS, 0, 0xD400);
    setFallback(OPT_SID_ADDRESS, 1, 0xD420);
    setFallback(OPT_SID_ADDRESS, 2, 0xD440);
    setFallback(OPT_SID_ADDRESS, 3, 0xD460);
    setFallback(OPT_AUDVOL, {0, 1, 2, 3}, 400);
    setFallback(OPT_AUDPAN, {0, 1, 2, 3}, 0);

    setFallback(OPT_RAM_PATTERN, RAM_PATTERN_VICE);
    setFallback(OPT_SAVE_ROMS, true);
    
    setFallback(OPT_DRV_AUTO_CONFIG, {DRIVE8, DRIVE9}, true);
    setFallback(OPT_DRV_TYPE, {DRIVE8, DRIVE9}, DRIVE_VC1541II);
    setFallback(OPT_DRV_RAM, {DRIVE8, DRIVE9}, DRVRAM_NONE);
    setFallback(OPT_DRV_PARCABLE, {DRIVE8, DRIVE9}, PAR_CABLE_NONE);
    setFallback(OPT_DRV_CONNECT, DRIVE8, true);
    setFallback(OPT_DRV_CONNECT, DRIVE9, false);
    setFallback(OPT_DRV_POWER_SWITCH, DRIVE8, true);
    setFallback(OPT_DRV_POWER_SWITCH, DRIVE9, true);
    setFallback(OPT_DRV_POWER_SAVE, {DRIVE8, DRIVE9}, true);
    setFallback(OPT_DRV_EJECT_DELAY, {DRIVE8, DRIVE9}, 30);
    setFallback(OPT_DRV_SWAP_DELAY, {DRIVE8, DRIVE9}, 30);
    setFallback(OPT_DRV_INSERT_DELAY, {DRIVE8, DRIVE9}, 30);
    setFallback(OPT_DRV_PAN, DRIVE8, 100);
    setFallback(OPT_DRV_PAN, DRIVE9, 300);
    setFallback(OPT_DRV_POWER_VOL, {DRIVE8, DRIVE9}, 50);
    setFallback(OPT_DRV_STEP_VOL, {DRIVE8, DRIVE9}, 50);
    setFallback(OPT_DRV_INSERT_VOL, {DRIVE8, DRIVE9}, 50);
    setFallback(OPT_DRV_EJECT_VOL, {DRIVE8, DRIVE9}, 50);

    setFallback(OPT_DAT_MODEL, DATASETTE_C1530);
    setFallback(OPT_DAT_CONNECT, true);

    setFallback(OPT_AUTOFIRE, false);
    setFallback(OPT_AUTOFIRE_BULLETS, -3);
    setFallback(OPT_AUTOFIRE_DELAY, 125);

    setFallback(OPT_MOUSE_MODEL, MOUSE_C1350);
    setFallback(OPT_SHAKE_DETECTION, true);
    setFallback(OPT_MOUSE_VELOCITY, 100);

    setFallback("BASIC_PATH", "");
    setFallback("CHAR_PATH", "");
    setFallback("KERNAL_PATH", "");
    setFallback("VC1541_PATH", "");
}

void
Defaults::_dump(Category category, std::ostream& os) const
{
    for (const auto &it: fallbacks) {

        const string key = it.first;

        if (values.contains(key)) {

            os << util::tab(key);
            os << values.at(key) << std::endl;

        } else {

            os << util::tab(key);
            os << fallbacks.at(key) << " (Default)" << std::endl;
        }
    }
}

void
Defaults::load(const fs::path &path)
{
    auto fs = std::ifstream(path, std::ifstream::binary);

    if (!fs.is_open()) {
        throw VC64Error(ERROR_FILE_NOT_FOUND);
    }

    debug(DEF_DEBUG, "Loading user defaults from %s...\n", path.string().c_str());
    load(fs);
}

void
Defaults::load(std::ifstream &stream)
{
    std::stringstream ss;
    ss << stream.rdbuf();

    load(ss);
}

void
Defaults::load(std::stringstream &stream)
{
    isize line = 0;
    isize accepted = 0;
    isize skipped = 0;
    string input;
    string section;

    {   SYNCHRONIZED

        debug(DEF_DEBUG, "Loading user defaults from string stream...\n");

        while(std::getline(stream, input)) {

            line++;

            // Remove white spaces
            util::trim(input);

            // Ignore empty lines
            if (input == "") continue;

            // Ignore comments
            if (input.substr(0,1) == "#") continue;

            // Check if this line contains a section marker
            if (input.front() == '[' && input.back() == ']') {

                // Extract the section name
                section = input.substr(1, input.size() - 2);
                continue;
            }

            // Check if this line is a key-value pair
            if (auto pos = input.find("="); pos != std::string::npos) {

                auto key = input.substr(0, pos);
                auto value = input.substr(pos + 1, std::string::npos);

                // Remove white spaces
                util::trim(key);
                util::trim(value);

                // Assemble the key
                auto delimiter = section.empty() ? "" : ".";
                key = section + delimiter + key;

                // Check if the key is a known key
                if (!fallbacks.contains(key)) {

                    warn("Ignoring invalid key %s\n", key.c_str());
                    skipped++;
                    continue;
                }

                // Add the key-value pair
                values[key] = value;
                accepted++;
                continue;
            }

            throw VC64Error(ERROR_SYNTAX, line);
        }

        debug(DEF_DEBUG, "%ld keys accepted, %ld ignored\n", accepted, skipped);
    }
}

void
Defaults::save(const fs::path &path)
{
    auto fs = std::ofstream(path, std::ofstream::binary);

    if (!fs.is_open()) {
        throw VC64Error(ERROR_FILE_CANT_WRITE);
    }

    save(fs);
}

void
Defaults::save(std::ofstream &stream)
{
    std::stringstream ss;
    save(ss);

    stream << ss.rdbuf();
}

void
Defaults::save(std::stringstream &stream)
{
    {   SYNCHRONIZED

        debug(DEF_DEBUG, "Saving user defaults...\n");

        std::map <string, std::map <string, string>> groups;

        // Write header
        stream << "# VirtualC64 " << C64::build() << std::endl;
        stream << "# dirkwhoffmann.github.io/virtualc64" << std::endl;
        stream << std::endl;

        // Iterate through all known keys
        for (const auto &it: fallbacks) {

            auto key = it.first;
            auto value = getString(key);

            // Check if the key belongs to a group
            if (auto pos = key.find('.'); pos == std::string::npos) {

                // Write ungrouped keys immediately
                stream << key << "=" << value << std::endl;

            } else {

                // Save the key temporarily
                auto prefix = key.substr(0, pos);
                auto suffix = key.substr(pos + 1, string::npos);
                groups[prefix][suffix] = value;
            }
        }

        // Write all groups
        for (const auto &[group, values]: groups) {

            stream << std::endl << "[" << group << "]" << std::endl;

            for (const auto &[key, value]: values) {

                stream << key << "=" << value << std::endl;
            }
        }
    }
}

string
Defaults::getString(const string &key)
{
    if (values.contains(key)) return values[key];
    if (fallbacks.contains(key)) return fallbacks[key];

    warn("Invalid key: %s\n", key.c_str());
    assert(false);
    throw VC64Error(ERROR_INVALID_KEY, key);
}

i64
Defaults::getInt(const string &key)
{
    auto value = getString(key);
    i64 result = 0;

    try {

        result = i64(std::stoll(value));
        debug(DEF_DEBUG, "get(%s) = %lld\n", key.c_str(), result);

    } catch (...) {

        warn("Can't parse value %s\n", key.c_str());
    }

    return result;
}

i64
Defaults::get(Option option)
{
    return getInt(string(OptionEnum::key(option)));
}

i64
Defaults::get(Option option, isize nr)
{
    return getInt(string(OptionEnum::key(option)) + std::to_string(nr));
}

string
Defaults::getFallback(const string &key)
{
    if (!fallbacks.contains(key)) {

        warn("Invalid key: %s\n", key.c_str());
        assert(false);
        throw VC64Error(ERROR_INVALID_KEY, key);
    }

    return fallbacks[key];
}

void
Defaults::setString(const string &key, const string &value)
{
    {   SYNCHRONIZED

        debug(DEF_DEBUG, "%s = %s\n", key.c_str(), value.c_str());

        if (!fallbacks.contains(key)) {

            warn("Invalid key: %s\n", key.c_str());
            assert(false);
            throw VC64Error(ERROR_INVALID_KEY, key);
        }

        values[key] = value;
    }
}

void
Defaults::set(Option option, i64 value)
{
    auto key = string(OptionEnum::key(option));
    auto val = std::to_string(value);

    setString(key, val);
}

void
Defaults::set(Option option, isize nr, i64 value)
{
    auto key = string(OptionEnum::key(option)) + std::to_string(nr);
    auto val = std::to_string(value);

    setString(key, val);
}

void
Defaults::set(Option option, std::vector <isize> nrs, i64 value)
{
    for (auto &nr : nrs) set(option, nr, value);
}

void
Defaults::setFallback(const string &key, const string &value)
{
    {   SYNCHRONIZED

        debug(DEF_DEBUG, "Fallback: %s = %s\n", key.c_str(), value.c_str());

        fallbacks[key] = value;
    }
}

void
Defaults::setFallback(Option option, const string &value)
{
    setFallback(string(OptionEnum::key(option)), value);
}

void
Defaults::setFallback(Option option, i64 value)
{
    setFallback(option, std::to_string(value));
}

void
Defaults::setFallback(Option option, isize nr, const string &value)
{
    setFallback(string(OptionEnum::key(option)) + std::to_string(nr), value);
}

void
Defaults::setFallback(Option option, isize nr, i64 value)
{
    setFallback(option, nr, std::to_string(value));
}

void
Defaults::setFallback(Option option, std::vector <isize> nrs, const string &value)
{
    for (auto &nr : nrs) setFallback(option, nr, value);
}

void
Defaults::setFallback(Option option, std::vector <isize> nrs, i64 value)
{
    setFallback(option, nrs, std::to_string(value));
}

void
Defaults::remove()
{
    SYNCHRONIZED values.clear();
}

void
Defaults::remove(const string &key)
{
    {   SYNCHRONIZED

        if (!fallbacks.contains(key)) {

            warn("Invalid key: %s\n", key.c_str());
            assert(false);
            throw VC64Error(ERROR_INVALID_KEY, key);
        }
        if (values.contains(key)) {
            values.erase(key);
        }
    }
}

void
Defaults::remove(Option option)
{
    remove(string(OptionEnum::key(option)));
}

void
Defaults::remove(Option option, isize nr)
{
    remove(string(OptionEnum::key(option)) + std::to_string(nr));
}

void
Defaults::remove(Option option, std::vector <isize> nrs)
{
    for (auto &nr : nrs) remove(option, nr);
}

}

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
#include "Defaults.h"
#include "C64.h"
#include "StringUtils.h"
#include "IOUtils.h"

namespace vc64 {

Defaults::Defaults()
{
    setFallback(OPT_HOST_REFRESH_RATE,          60);
    setFallback(OPT_HOST_SAMPLE_RATE,           44100);
    setFallback(OPT_HOST_FRAMEBUF_WIDTH,        0);
    setFallback(OPT_HOST_FRAMEBUF_HEIGHT,       0);

    setFallback(OPT_C64_WARP_BOOT,              0);
    setFallback(OPT_C64_WARP_MODE,              WARP_NEVER);
    setFallback(OPT_C64_VSYNC,                  false);
    setFallback(OPT_C64_SPEED_BOOST,            100);
    setFallback(OPT_C64_RUN_AHEAD,              0);

    setFallback(OPT_C64_SNAP_AUTO,              false);
    setFallback(OPT_C64_SNAP_DELAY,             10);
    setFallback(OPT_C64_SNAP_COMPRESS,          true);

    setFallback(OPT_POWER_GRID,                 GRID_STABLE_50HZ);

    setFallback(OPT_GLUE_LOGIC,                 GLUE_LOGIC_DISCRETE);

    setFallback(OPT_CIA_REVISION,               MOS_6526);
    setFallback(OPT_CIA_TIMER_B_BUG,            true);
    setFallback(OPT_CIA_IDLE_SLEEP,             true);

    setFallback(OPT_VICII_REVISION,             VICII_PAL_8565);
    setFallback(OPT_VICII_POWER_SAVE,           true);
    setFallback(OPT_VICII_GRAY_DOT_BUG,         true);
    setFallback(OPT_VICII_HIDE_SPRITES,         false);
    setFallback(OPT_VICII_SB_COLLISIONS,        true);
    setFallback(OPT_VICII_SS_COLLISIONS,        true);
    setFallback(OPT_VICII_CUT_LAYERS,           0xFF);
    setFallback(OPT_VICII_CUT_OPACITY,          0xFF);

    setFallback(OPT_DMA_DEBUG_ENABLE,           false);
    setFallback(OPT_DMA_DEBUG_MODE,             DMA_DISPLAY_MODE_FG_LAYER);
    setFallback(OPT_DMA_DEBUG_OPACITY,          0x80);
    setFallback(OPT_DMA_DEBUG_CHANNEL0,         true);
    setFallback(OPT_DMA_DEBUG_CHANNEL1,         true);
    setFallback(OPT_DMA_DEBUG_CHANNEL2,         true);
    setFallback(OPT_DMA_DEBUG_CHANNEL3,         true);
    setFallback(OPT_DMA_DEBUG_CHANNEL4,         true);
    setFallback(OPT_DMA_DEBUG_CHANNEL5,         true);
    setFallback(OPT_DMA_DEBUG_COLOR0,           GpuColor(0xFF, 0x00, 0x00).abgr);
    setFallback(OPT_DMA_DEBUG_COLOR1,           GpuColor(0xFF, 0xC0, 0x00).abgr);
    setFallback(OPT_DMA_DEBUG_COLOR2,           GpuColor(0xFF, 0xFF, 0x00).abgr);
    setFallback(OPT_DMA_DEBUG_COLOR3,           GpuColor(0x00, 0xFF, 0xFF).abgr);
    setFallback(OPT_DMA_DEBUG_COLOR4,           GpuColor(0x00, 0xFF, 0x00).abgr);
    setFallback(OPT_DMA_DEBUG_COLOR5,           GpuColor(0x00, 0x80, 0xFF).abgr);

    setFallback(OPT_EXP_REU_SPEED,              1);

    setFallback(OPT_USR_DEVICE,                 USR_RS232);

    setFallback(OPT_VID_WHITE_NOISE,            true);

    setFallback(OPT_MON_PALETTE,                PALETTE_COLOR);
    setFallback(OPT_MON_BRIGHTNESS,             50);
    setFallback(OPT_MON_CONTRAST,               100);
    setFallback(OPT_MON_SATURATION,             50);
    setFallback(OPT_MON_HCENTER,                0);
    setFallback(OPT_MON_VCENTER,                0);
    setFallback(OPT_MON_HZOOM,                  0);
    setFallback(OPT_MON_VZOOM,                  46);
    setFallback(OPT_MON_UPSCALER,               UPSCALER_NONE);
    setFallback(OPT_MON_BLUR,                   true);
    setFallback(OPT_MON_BLUR_RADIUS,            0);
    setFallback(OPT_MON_BLOOM,                  false);
    setFallback(OPT_MON_BLOOM_RADIUS,           1000);
    setFallback(OPT_MON_BLOOM_BRIGHTNESS,       400);
    setFallback(OPT_MON_BLOOM_WEIGHT,           1210);
    setFallback(OPT_MON_DOTMASK,                DOTMASK_NONE);
    setFallback(OPT_MON_DOTMASK_BRIGHTNESS,     700);
    setFallback(OPT_MON_SCANLINES,              SCANLINES_NONE);
    setFallback(OPT_MON_SCANLINE_BRIGHTNESS,    550);
    setFallback(OPT_MON_SCANLINE_WEIGHT,        110);
    setFallback(OPT_MON_DISALIGNMENT,           0);
    setFallback(OPT_MON_DISALIGNMENT_H,         1000);
    setFallback(OPT_MON_DISALIGNMENT_V,         1000);

    setFallback(OPT_AUD_VOL0,                   100);
    setFallback(OPT_AUD_VOL1,                   100);
    setFallback(OPT_AUD_VOL2,                   100);
    setFallback(OPT_AUD_VOL3,                   100);
    setFallback(OPT_AUD_PAN0,                   0);
    setFallback(OPT_AUD_PAN1,                   0);
    setFallback(OPT_AUD_PAN2,                   0);
    setFallback(OPT_AUD_PAN3,                   0);
    setFallback(OPT_AUD_VOL_L,                  50);
    setFallback(OPT_AUD_VOL_R,                  50);

    setFallback(OPT_SID_ENABLE,                 true,                   {0});
    setFallback(OPT_SID_ENABLE,                 false,                  {1, 2, 3});
    setFallback(OPT_SID_ADDRESS,                0xD400,                 {0});
    setFallback(OPT_SID_ADDRESS,                0xD420,                 {1});
    setFallback(OPT_SID_ADDRESS,                0xD440,                 {2});
    setFallback(OPT_SID_ADDRESS,                0xD460,                 {3});
    setFallback(OPT_SID_REVISION,               MOS_8580,               {0, 1, 2, 3});
    setFallback(OPT_SID_FILTER,                 false,                  {0, 1, 2, 3});
    setFallback(OPT_SID_ENGINE,                 SIDENGINE_RESID,        {0, 1, 2, 3});
    setFallback(OPT_SID_SAMPLING,               SAMPLING_INTERPOLATE,   {0, 1, 2, 3});
    setFallback(OPT_SID_POWER_SAVE,             false,                  {0, 1, 2, 3});

    setFallback(OPT_MEM_INIT_PATTERN,           RAM_PATTERN_VICE);
    setFallback(OPT_MEM_HEATMAP,                false);
    setFallback(OPT_MEM_SAVE_ROMS,              true);

    setFallback(OPT_DRV_AUTO_CONFIG,            true,               {DRIVE8, DRIVE9});
    setFallback(OPT_DRV_TYPE,                   DRIVE_VC1541II,     {DRIVE8, DRIVE9});
    setFallback(OPT_DRV_RAM,                    DRVRAM_NONE,        {DRIVE8, DRIVE9});
    setFallback(OPT_DRV_SAVE_ROMS,              true,               {DRIVE8, DRIVE9});
    setFallback(OPT_DRV_PARCABLE,               PAR_CABLE_NONE,     {DRIVE8, DRIVE9});
    setFallback(OPT_DRV_CONNECT,                true,               {DRIVE8});
    setFallback(OPT_DRV_CONNECT,                false,              {DRIVE9});
    setFallback(OPT_DRV_POWER_SWITCH,           true,               {DRIVE8});
    setFallback(OPT_DRV_POWER_SWITCH,           true,               {DRIVE9});
    setFallback(OPT_DRV_POWER_SAVE,             true,               {DRIVE8, DRIVE9});
    setFallback(OPT_DRV_EJECT_DELAY,            30,                 {DRIVE8, DRIVE9});
    setFallback(OPT_DRV_SWAP_DELAY,             30,                 {DRIVE8, DRIVE9});
    setFallback(OPT_DRV_INSERT_DELAY,           30,                 {DRIVE8, DRIVE9});
    setFallback(OPT_DRV_PAN,                    0,                  {DRIVE8, DRIVE9});
    setFallback(OPT_DRV_POWER_VOL,              50,                 {DRIVE8, DRIVE9});
    setFallback(OPT_DRV_STEP_VOL,               50,                 {DRIVE8, DRIVE9});
    setFallback(OPT_DRV_INSERT_VOL,             50,                 {DRIVE8, DRIVE9});
    setFallback(OPT_DRV_EJECT_VOL,              50,                 {DRIVE8, DRIVE9});

    setFallback(OPT_DAT_MODEL,                  DATASETTE_C1530);
    setFallback(OPT_DAT_CONNECT,                true);

    setFallback(OPT_MOUSE_MODEL,                MOUSE_C1350);
    setFallback(OPT_MOUSE_SHAKE_DETECT,         true);
    setFallback(OPT_MOUSE_VELOCITY,             100);

    setFallback(OPT_AUTOFIRE,                   false);
    setFallback(OPT_AUTOFIRE_BURSTS,            false);
    setFallback(OPT_AUTOFIRE_BULLETS,           3);
    setFallback(OPT_AUTOFIRE_DELAY,             5);

    setFallback(OPT_RS232_DEVICE,               COMDEV_NONE);
    setFallback(OPT_RS232_BAUD,                 600);

    setFallback(OPT_PADDLE_ORIENTATION,         PADDLE_HORIZONTAL);

    setFallback(OPT_RS232_BAUD,                 600);

    setFallback(OPT_REC_FRAME_RATE,             50);
    setFallback(OPT_REC_BIT_RATE,               512);
    setFallback(OPT_REC_SAMPLE_RATE,            44100);
    setFallback(OPT_REC_ASPECT_X,               768);
    setFallback(OPT_REC_ASPECT_Y,               702);

    setFallback(OPT_SRV_PORT,                   8081,                   { SERVER_RSH });
    setFallback(OPT_SRV_PROTOCOL,               SRVPROT_DEFAULT,        { SERVER_RSH });
    setFallback(OPT_SRV_AUTORUN,                false,                  { SERVER_RSH });
    setFallback(OPT_SRV_VERBOSE,                true,                   { SERVER_RSH });

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
        throw Error(VC64ERROR_FILE_NOT_FOUND);
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

            throw Error(VC64ERROR_SYNTAX, line);
        }

        if (accepted || skipped) {
            debug(DEF_DEBUG, "%ld keys accepted, %ld ignored\n", accepted, skipped);
        }
    }
}

void
Defaults::save(const fs::path &path)
{
    auto fs = std::ofstream(path, std::ofstream::binary);

    if (!fs.is_open()) {
        throw Error(VC64ERROR_FILE_CANT_WRITE);
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
            auto value = getRaw(key);

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
Defaults::getRaw(const string &key) const
{
    if (values.contains(key)) return values.at(key);
    if (fallbacks.contains(key)) return fallbacks.at(key);

    throw Error(VC64ERROR_INVALID_KEY, key);
}

i64
Defaults::get(const string &key) const
{
    auto value = getRaw(key);

    try {

        return i64(std::stoll(value));

    } catch (...) {

        warn("Can't parse value %s\n", key.c_str());
        return 0;
    }
}

i64
Defaults::get(Option option, isize nr) const
{
    try {

        return get(string(OptionEnum::rawkey(option)) + std::to_string(nr));

    } catch (...) {

        return get(string(OptionEnum::rawkey(option)));
    }
}

string
Defaults::getFallbackRaw(const string &key) const
{
    if (fallbacks.contains(key)) return fallbacks.at(key);

    throw Error(VC64ERROR_INVALID_KEY, key);
}

i64
Defaults::getFallback(const string &key) const
{
    auto value = getFallbackRaw(key);

    try {

        return i64(std::stoll(value));

    } catch (...) {

        warn("Can't parse value %s\n", key.c_str());
        return 0;
    }
}

i64
Defaults::getFallback(Option option, isize nr) const
{
    try {

        return getFallback(string(OptionEnum::rawkey(option)) + std::to_string(nr));

    } catch (...) {

        return getFallback(string(OptionEnum::rawkey(option)));
    }
}

void
Defaults::set(const string &key, const string &value)
{
    {   SYNCHRONIZED

        debug(DEF_DEBUG, "%s = %s\n", key.c_str(), value.c_str());

        if (!fallbacks.contains(key)) {

            warn("Invalid key: %s\n", key.c_str());
            assert(false);
            throw Error(VC64ERROR_INVALID_KEY, key);
        }

        values[key] = value;
    }
}

void
Defaults::set(Option option, const string &value)
{
    set(OptionEnum::rawkey(option), value);
}

void
Defaults::set(Option option, const string &value, std::vector <isize> objids)
{
    auto key = string(OptionEnum::rawkey(option));

    for (auto &nr : objids) {
        set(key + std::to_string(nr), value);
    }
}

void 
Defaults::set(Option option, i64 value)
{
    set(option, std::to_string(value));
}

void
Defaults::set(Option option, i64 value, std::vector <isize> objids)
{
    set(option, std::to_string(value), objids);
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
    setFallback(OptionEnum::rawkey(option), value);
}

void
Defaults::setFallback(Option option, const string &value, std::vector <isize> objids)
{
    auto key = string(OptionEnum::rawkey(option));

    for (auto &nr : objids) {
        setFallback(key + std::to_string(nr), value);
    }
}

void
Defaults::setFallback(Option option, i64 value)
{
    setFallback(option, std::to_string(value));
}

void
Defaults::setFallback(Option option, i64 value, std::vector <isize> objids)
{
    setFallback(option, std::to_string(value), objids);
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
            throw Error(VC64ERROR_INVALID_KEY, key);
        }
        if (values.contains(key)) {
            values.erase(key);
        }
    }
}

void
Defaults::remove(Option option)
{
    remove(string(OptionEnum::rawkey(option)));
}

void
Defaults::remove(Option option, std::vector <isize> nrs)
{
    for (auto &nr : nrs) {
        remove(string(OptionEnum::rawkey(option)) + std::to_string(nr));
    }
}

}

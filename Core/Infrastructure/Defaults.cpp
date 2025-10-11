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
    setFallback(Opt::HOST_REFRESH_RATE,          60);
    setFallback(Opt::HOST_SAMPLE_RATE,           44100);
    setFallback(Opt::HOST_FRAMEBUF_WIDTH,        0);
    setFallback(Opt::HOST_FRAMEBUF_HEIGHT,       0);

    setFallback(Opt::C64_WARP_BOOT,              0);
    setFallback(Opt::C64_WARP_MODE,              (i64)Warp::NEVER);
    setFallback(Opt::C64_VSYNC,                  false);
    setFallback(Opt::C64_SPEED_BOOST,            100);
    setFallback(Opt::C64_RUN_AHEAD,              0);

    setFallback(Opt::C64_SNAP_AUTO,              false);
    setFallback(Opt::C64_SNAP_DELAY,             10);
    setFallback(Opt::C64_SNAP_COMPRESSOR,        (i64)Compressor::GZIP);

    setFallback(Opt::DASM_NUMBERS,               (i64)DasmNumbers::HEX0);
    
    setFallback(Opt::POWER_GRID,                 (i64)PowerGrid::STABLE_50HZ);

    setFallback(Opt::GLUE_LOGIC,                 (i64)GlueLogic::DISCRETE);

    setFallback(Opt::CIA_REVISION,               (i64)CIARev::MOS_6526);
    setFallback(Opt::CIA_TIMER_B_BUG,            true);
    setFallback(Opt::CIA_IDLE_SLEEP,             true);

    setFallback(Opt::VICII_REVISION,             (i64)VICIIRev::PAL_8565);
    setFallback(Opt::VICII_POWER_SAVE,           true);
    setFallback(Opt::VICII_GRAY_DOT_BUG,         true);
    setFallback(Opt::VICII_HIDE_SPRITES,         false);
    setFallback(Opt::VICII_SB_COLLISIONS,        true);
    setFallback(Opt::VICII_SS_COLLISIONS,        true);
    setFallback(Opt::VICII_CUT_LAYERS,           0xFF);
    setFallback(Opt::VICII_CUT_OPACITY,          0xFF);

    setFallback(Opt::DMA_DEBUG_ENABLE,           false);
    setFallback(Opt::DMA_DEBUG_OVERLAY,          false);
    setFallback(Opt::DMA_DEBUG_MODE,             (i64)DmaDisplayMode::FG_LAYER);
    setFallback(Opt::DMA_DEBUG_OPACITY,          0x80);
    setFallback(Opt::DMA_DEBUG_CHANNEL0,         true);
    setFallback(Opt::DMA_DEBUG_CHANNEL1,         true);
    setFallback(Opt::DMA_DEBUG_CHANNEL2,         true);
    setFallback(Opt::DMA_DEBUG_CHANNEL3,         true);
    setFallback(Opt::DMA_DEBUG_CHANNEL4,         true);
    setFallback(Opt::DMA_DEBUG_CHANNEL5,         true);
    setFallback(Opt::DMA_DEBUG_COLOR0,           GpuColor(0xFF, 0x00, 0x00).abgr);
    setFallback(Opt::DMA_DEBUG_COLOR1,           GpuColor(0xFF, 0xC0, 0x00).abgr);
    setFallback(Opt::DMA_DEBUG_COLOR2,           GpuColor(0xFF, 0xFF, 0x00).abgr);
    setFallback(Opt::DMA_DEBUG_COLOR3,           GpuColor(0x00, 0xFF, 0xFF).abgr);
    setFallback(Opt::DMA_DEBUG_COLOR4,           GpuColor(0x00, 0xFF, 0x00).abgr);
    setFallback(Opt::DMA_DEBUG_COLOR5,           GpuColor(0x00, 0x80, 0xFF).abgr);

    setFallback(Opt::EXP_REU_SPEED,              1);

    setFallback(Opt::USR_DEVICE,                 (i64)UserPortDevice::RS232);

    setFallback(Opt::VID_WHITE_NOISE,            true);

    setFallback(Opt::MON_PALETTE,                (i64)Palette::COLOR);
    setFallback(Opt::MON_BRIGHTNESS,             50);
    setFallback(Opt::MON_CONTRAST,               100);
    setFallback(Opt::MON_SATURATION,             50);
    setFallback(Opt::MON_HCENTER,                0);
    setFallback(Opt::MON_VCENTER,                0);
    setFallback(Opt::MON_HZOOM,                  0);
    setFallback(Opt::MON_VZOOM,                  46);
    setFallback(Opt::MON_UPSCALER,               (i64)Upscaler::NONE);
    setFallback(Opt::MON_BLUR,                   true);
    setFallback(Opt::MON_BLUR_RADIUS,            0);
    setFallback(Opt::MON_BLOOM,                  false);
    setFallback(Opt::MON_BLOOM_RADIUS,           200);
    setFallback(Opt::MON_BLOOM_BRIGHTNESS,       200);
    setFallback(Opt::MON_BLOOM_WEIGHT,           100);
    setFallback(Opt::MON_DOTMASK,                (i64)Dotmask::NONE);
    setFallback(Opt::MON_DOTMASK_BRIGHTNESS,     550);
    setFallback(Opt::MON_SCANLINES,              (i64)Scanlines::NONE);
    setFallback(Opt::MON_SCANLINE_BRIGHTNESS,    550);
    setFallback(Opt::MON_SCANLINE_WEIGHT,        110);
    setFallback(Opt::MON_DISALIGNMENT,           0);
    setFallback(Opt::MON_DISALIGNMENT_H,         250);
    setFallback(Opt::MON_DISALIGNMENT_V,         250);

    setFallback(Opt::AUD_VOL0,                   100);
    setFallback(Opt::AUD_VOL1,                   100);
    setFallback(Opt::AUD_VOL2,                   100);
    setFallback(Opt::AUD_VOL3,                   100);
    setFallback(Opt::AUD_PAN0,                   0);
    setFallback(Opt::AUD_PAN1,                   0);
    setFallback(Opt::AUD_PAN2,                   0);
    setFallback(Opt::AUD_PAN3,                   0);
    setFallback(Opt::AUD_VOL_L,                  50);
    setFallback(Opt::AUD_VOL_R,                  50);
    setFallback(Opt::AUD_BUFFER_SIZE,            4096);
    setFallback(Opt::AUD_ASR,                    true);

    setFallback(Opt::SID_ENABLE,                 true,                   {0});
    setFallback(Opt::SID_ENABLE,                 false,                  {1, 2, 3});
    setFallback(Opt::SID_ADDRESS,                0xD400,                 {0});
    setFallback(Opt::SID_ADDRESS,                0xD420,                 {1});
    setFallback(Opt::SID_ADDRESS,                0xD440,                 {2});
    setFallback(Opt::SID_ADDRESS,                0xD460,                 {3});
    setFallback(Opt::SID_REV,                    (i64)SIDRevision::MOS_8580, {0, 1, 2, 3});
    setFallback(Opt::SID_FILTER,                 false,                  {0, 1, 2, 3});
    setFallback(Opt::SID_ENGINE,                 (i64)SIDEngine::RESID,  {0, 1, 2, 3});
    setFallback(Opt::SID_SAMPLING,               (i64)SamplingMethod::INTERPOLATE, {0, 1, 2, 3});
    setFallback(Opt::SID_POWER_SAVE,             false,                  {0, 1, 2, 3});

    setFallback(Opt::MEM_INIT_PATTERN,           (i64)RamPattern::VICE);
    setFallback(Opt::MEM_HEATMAP,                false);
    setFallback(Opt::MEM_SAVE_ROMS,              true);

    setFallback(Opt::DRV_AUTO_CONFIG,            true,               {DRIVE8, DRIVE9});
    setFallback(Opt::DRV_TYPE,                   (i64)DriveType::VC1541II, {DRIVE8, DRIVE9});
    setFallback(Opt::DRV_RAM,                    (i64)DriveRam::NONE, {DRIVE8, DRIVE9});
    setFallback(Opt::DRV_SAVE_ROMS,              true,               {DRIVE8, DRIVE9});
    setFallback(Opt::DRV_PARCABLE,               (i64)ParCableType::NONE,{DRIVE8, DRIVE9});
    setFallback(Opt::DRV_CONNECT,                true,               {DRIVE8});
    setFallback(Opt::DRV_CONNECT,                false,              {DRIVE9});
    setFallback(Opt::DRV_POWER_SWITCH,           true,               {DRIVE8});
    setFallback(Opt::DRV_POWER_SWITCH,           true,               {DRIVE9});
    setFallback(Opt::DRV_POWER_SAVE,             true,               {DRIVE8, DRIVE9});
    setFallback(Opt::DRV_EJECT_DELAY,            30,                 {DRIVE8, DRIVE9});
    setFallback(Opt::DRV_SWAP_DELAY,             30,                 {DRIVE8, DRIVE9});
    setFallback(Opt::DRV_INSERT_DELAY,           30,                 {DRIVE8, DRIVE9});
    setFallback(Opt::DRV_PAN,                    0,                  {DRIVE8, DRIVE9});
    setFallback(Opt::DRV_POWER_VOL,              50,                 {DRIVE8, DRIVE9});
    setFallback(Opt::DRV_STEP_VOL,               50,                 {DRIVE8, DRIVE9});
    setFallback(Opt::DRV_INSERT_VOL,             50,                 {DRIVE8, DRIVE9});
    setFallback(Opt::DRV_EJECT_VOL,              50,                 {DRIVE8, DRIVE9});

    setFallback(Opt::DAT_MODEL,                  (i64)DatasetteModel::C1530);
    setFallback(Opt::DAT_CONNECT,                true);

    setFallback(Opt::MOUSE_MODEL,                (i64)MouseModel::C1350);
    setFallback(Opt::MOUSE_SHAKE_DETECT,         true);
    setFallback(Opt::MOUSE_VELOCITY,             100);

    setFallback(Opt::AUTOFIRE,                   false);
    setFallback(Opt::AUTOFIRE_BURSTS,            false);
    setFallback(Opt::AUTOFIRE_BULLETS,           3);
    setFallback(Opt::AUTOFIRE_DELAY,             5);

    setFallback(Opt::RS232_DEVICE,               (i64)CommunicationDevice::NONE);
    setFallback(Opt::RS232_BAUD,                 600);

    setFallback(Opt::PADDLE_ORIENTATION,         (i64)PaddleOrientation::HORIZONTAL);

    setFallback(Opt::RS232_BAUD,                 600);

    setFallback(Opt::REC_FRAME_RATE,             50);
    setFallback(Opt::REC_BIT_RATE,               512);
    setFallback(Opt::REC_SAMPLE_RATE,            44100);
    setFallback(Opt::REC_ASPECT_X,               768);
    setFallback(Opt::REC_ASPECT_Y,               702);

    setFallback(Opt::SRV_PORT,                   8081,                   { (i64)ServerType::RSH });
    setFallback(Opt::SRV_PROTOCOL,               (i64)ServerProtocol::DEFAULT, { (i64)ServerType::RSH });
    setFallback(Opt::SRV_AUTORUN,                false,                  { (i64)ServerType::RSH });
    setFallback(Opt::SRV_VERBOSE,                true,                   { (i64)ServerType::RSH });

    setFallback(Opt::DBG_DEBUGCART,              0);
    setFallback(Opt::DBG_WATCHDOG,               0);

    setFallback("BASIC_PATH", "");
    setFallback("CHAR_PATH", "");
    setFallback("KERNAL_PATH", "");
    setFallback("VC1541_PATH", "");
}

void
Defaults::_dump(Category category, std::ostream &os) const
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
        throw AppError(Fault::FILE_NOT_FOUND);
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
    {   SYNCHRONIZED

        isize line = 0;
        isize accepted = 0;
        isize skipped = 0;
        string input;
        string section;

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

                    warn("Ignoring invalid key %s = %s\n", key.c_str(), value.c_str());
                    skipped++;
                    continue;
                }

                // Add the key-value pair
                values[key] = value;
                accepted++;
                continue;
            }

            throw AppError(Fault::SYNTAX, line);
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
        throw AppError(Fault::FILE_CANT_WRITE);
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

    throw AppError(Fault::INVALID_KEY, key);
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
Defaults::get(Opt option, isize nr) const
{
    try {

        return get(string(OptEnum::fullKey(option)) + std::to_string(nr));

    } catch (...) {

        return get(string(OptEnum::fullKey(option)));
    }
}

string
Defaults::getFallbackRaw(const string &key) const
{
    if (fallbacks.contains(key)) return fallbacks.at(key);

    throw AppError(Fault::INVALID_KEY, key);
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
Defaults::getFallback(Opt option, isize nr) const
{
    try {

        return getFallback(string(OptEnum::fullKey(option)) + std::to_string(nr));

    } catch (...) {

        return getFallback(string(OptEnum::fullKey(option)));
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
            throw AppError(Fault::INVALID_KEY, key);
        }

        values[key] = value;
    }
}

void
Defaults::set(Opt option, const string &value)
{
    set(OptEnum::fullKey(option), value);
}

void
Defaults::set(Opt option, const string &value, std::vector <isize> objids)
{
    auto key = string(OptEnum::fullKey(option));

    for (auto &nr : objids) {
        set(key + std::to_string(nr), value);
    }
}

void
Defaults::set(Opt option, i64 value)
{
    set(option, std::to_string(value));
}

void
Defaults::set(Opt option, i64 value, std::vector <isize> objids)
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
Defaults::setFallback(Opt option, const string &value)
{
    setFallback(OptEnum::fullKey(option), value);
}

void
Defaults::setFallback(Opt option, const string &value, std::vector <isize> objids)
{
    auto key = string(OptEnum::fullKey(option));

    for (auto &nr : objids) {
        setFallback(key + std::to_string(nr), value);
    }
}

void
Defaults::setFallback(Opt option, i64 value)
{
    setFallback(option, std::to_string(value));
}

void
Defaults::setFallback(Opt option, i64 value, std::vector <isize> objids)
{
    setFallback(option, std::to_string(value), objids);
}

void
Defaults::remove()
{
    {   SYNCHRONIZED

        values.clear();
    }
}

void
Defaults::remove(const string &key)
{
    {   SYNCHRONIZED

        if (!fallbacks.contains(key)) {

            warn("Invalid key: %s\n", key.c_str());
            assert(false);
            throw AppError(Fault::INVALID_KEY, key);
        }
        if (values.contains(key)) {
            values.erase(key);
        }
    }
}

void
Defaults::remove(Opt option)
{
    remove(string(OptEnum::fullKey(option)));
}

void
Defaults::remove(Opt option, std::vector <isize> nrs)
{
    for (auto &nr : nrs) {
        remove(string(OptEnum::fullKey(option)) + std::to_string(nr));
    }
}

}

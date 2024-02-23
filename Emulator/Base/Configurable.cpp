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
#include "Configurable.h"
#include "Parser.h"
#include "VICIITypes.h"
#include "DatasetteTypes.h"
#include "MemoryTypes.h"
#include "SIDTypes.h"
#include "CIATypes.h"
#include "DriveTypes.h"
#include "ParCableTypes.h"
#include "PowerSupplyTypes.h"
#include "MouseTypes.h"
#include "IOUtils.h"
#include <algorithm>

namespace vc64 {

using namespace util;

string
Configurable::opt2str(Option opt) const
{
    return OptionEnum::key(opt);
}

Option 
Configurable::str2opt(const string &opt) const
{
    auto accept = [&](long i) { return isValidOption(Option(i)); };
    return util::parsePartialEnum<OptionEnum>(opt, accept);
}

string 
Configurable::arg2str(Option opt, i64 arg) const
{
    auto toBool = [](i64 arg) { return arg ? "true" : "false"; };
    auto toNum = [](i64 arg) { return std::to_string(arg); };

    switch (opt) {

        case OPT_EMU_WARP_MODE:             return VICIIRevisionEnum::key(arg);
        case OPT_EMU_WARP_BOOT:             return toBool(arg);
        case OPT_EMU_VSYNC:                 return toBool(arg);
        case OPT_EMU_TIME_LAPSE:            return toNum(arg);
        case OPT_EMU_RUN_AHEAD:             return toNum(arg);

        case OPT_HOST_SAMPLE_RATE:          return toNum(arg);

        case OPT_HOST_REFRESH_RATE:         return toNum(arg);
        case OPT_HOST_FRAMEBUF_WIDTH:       return toNum(arg);
        case OPT_HOST_FRAMEBUF_HEIGHT:      return toNum(arg);

        case OPT_VICII_REVISION:            return VICIIRevisionEnum::key(arg);
        case OPT_VICII_PALETTE:             return PaletteEnum::key(arg);
        case OPT_VICII_BRIGHTNESS:          return toNum(arg);
        case OPT_VICII_CONTRAST:            return toNum(arg);
        case OPT_VICII_SATURATION:          return toNum(arg);
        case OPT_VICII_GRAY_DOT_BUG:        return toBool(arg);
        case OPT_VICII_POWER_SAVE:          return toBool(arg);
        case OPT_VICII_HIDE_SPRITES:        return toBool(arg);
        case OPT_VICII_SS_COLLISIONS:       return toBool(arg);
        case OPT_VICII_SB_COLLISIONS:       return toBool(arg);
        case OPT_GLUE_LOGIC:                return GlueLogicEnum::key(arg);

        case OPT_VICII_CUT_LAYERS:          return toNum(arg);
        case OPT_VICII_CUT_OPACITY:         return toNum(arg);
        case OPT_DMA_DEBUG_ENABLE:          return toBool(arg);
        case OPT_DMA_DEBUG_MODE:            return DmaDisplayModeEnum::key(arg);
        case OPT_DMA_DEBUG_OPACITY:         return toNum(arg);

        case OPT_POWER_GRID:                return PowerGridEnum::key(arg);

        case OPT_CIA_REVISION:              return CIARevisionEnum::key(arg);
        case OPT_CIA_TIMER_B_BUG:           return toBool(arg);

        case OPT_SID_ENABLE:                return toNum(arg);
        case OPT_SID_ADDRESS:               return toNum(arg);

        case OPT_SID_REVISION:              return SIDRevisionEnum::key(arg);
        case OPT_SID_FILTER:                return toBool(arg);
        case OPT_SID_SAMPLING:              return SamplingMethodEnum::key(arg);
        case OPT_SID_POWER_SAVE:            return toBool(arg);
        case OPT_SID_ENGINE:                return SIDEngineEnum::key(arg);
        case OPT_AUD_PAN:                   return toNum(arg);
        case OPT_AUD_VOL:                   return toNum(arg);
        case OPT_AUD_VOL_L:                 return toNum(arg);
        case OPT_AUD_VOL_R:                 return toNum(arg);

        case OPT_RAM_PATTERN:               return RamPatternEnum::key(arg);

        case OPT_SAVE_ROMS:                 return toBool(arg);

        case OPT_DRV_AUTO_CONFIG:           return toBool(arg);
        case OPT_DRV_TYPE:                  return DriveTypeEnum::key(arg);
        case OPT_DRV_RAM:                   return DriveRamEnum::key(arg);
        case OPT_DRV_PARCABLE:              return ParCableTypeEnum::key(arg);
        case OPT_DRV_CONNECT:               return toBool(arg);
        case OPT_DRV_POWER_SWITCH:          return toBool(arg);
        case OPT_DRV_POWER_SAVE:            return toBool(arg);
        case OPT_DRV_EJECT_DELAY:           return toNum(arg);
        case OPT_DRV_SWAP_DELAY:            return toNum(arg);
        case OPT_DRV_INSERT_DELAY:          return toNum(arg);
        case OPT_DRV_PAN:                   return toNum(arg);
        case OPT_DRV_POWER_VOL:             return toNum(arg);
        case OPT_DRV_STEP_VOL:              return toNum(arg);
        case OPT_DRV_INSERT_VOL:            return toNum(arg);
        case OPT_DRV_EJECT_VOL:             return toNum(arg);

        case OPT_DAT_MODEL:                 return DatasetteModelEnum::key(arg);
        case OPT_DAT_CONNECT:               return toBool(arg);

        case OPT_MOUSE_MODEL:               return MouseModelEnum::key(arg);
        case OPT_MOUSE_SHAKE_DETECT:        return toBool(arg);
        case OPT_MOUSE_VELOCITY:            return toNum(arg);

        case OPT_JOY_AUTOFIRE:              return toBool(arg);
        case OPT_JOY_AUTOFIRE_BULLETS:      return toNum(arg);
        case OPT_JOY_AUTOFIRE_DELAY:        return toNum(arg);

        default:
            // printf("Unrecognized option: %s\n", OptionEnum::key(opt));
            fatalError;
    }
}

i64 
Configurable::str2arg(Option opt, const string &arg) const
{
    switch (opt) {

        case OPT_EMU_WARP_MODE:             return parseEnum<VICIIRevisionEnum>(arg);
        case OPT_EMU_WARP_BOOT:             return parseBool(arg);
        case OPT_EMU_VSYNC:                 return parseBool(arg);
        case OPT_EMU_TIME_LAPSE:            return parseNum(arg);
        case OPT_EMU_RUN_AHEAD:             return parseNum(arg);

        case OPT_HOST_SAMPLE_RATE:          return parseNum(arg);

        case OPT_HOST_REFRESH_RATE:         return parseNum(arg);
        case OPT_HOST_FRAMEBUF_WIDTH:       return parseNum(arg);
        case OPT_HOST_FRAMEBUF_HEIGHT:      return parseNum(arg);

        case OPT_VICII_REVISION:            return parseEnum<VICIIRevisionEnum>(arg);
        case OPT_VICII_PALETTE:             return parseEnum<PaletteEnum>(arg);
        case OPT_VICII_BRIGHTNESS:          return parseNum(arg);
        case OPT_VICII_CONTRAST:            return parseNum(arg);
        case OPT_VICII_SATURATION:          return parseNum(arg);
        case OPT_VICII_GRAY_DOT_BUG:        return parseBool(arg);
        case OPT_VICII_POWER_SAVE:          return parseBool(arg);
        case OPT_VICII_HIDE_SPRITES:        return parseBool(arg);
        case OPT_VICII_SS_COLLISIONS:       return parseBool(arg);
        case OPT_VICII_SB_COLLISIONS:       return parseBool(arg);
        case OPT_GLUE_LOGIC:                return parseEnum<GlueLogicEnum>(arg);

        case OPT_VICII_CUT_LAYERS:          return parseNum(arg);
        case OPT_VICII_CUT_OPACITY:         return parseNum(arg);
        case OPT_DMA_DEBUG_ENABLE:          return parseBool(arg);
        case OPT_DMA_DEBUG_MODE:            return parseEnum<DmaDisplayModeEnum>(arg);
        case OPT_DMA_DEBUG_OPACITY:         return parseNum(arg);

        case OPT_POWER_GRID:                return parseEnum<PowerGridEnum>(arg);

        case OPT_CIA_REVISION:              return parseEnum<CIARevisionEnum>(arg);
        case OPT_CIA_TIMER_B_BUG:           return parseBool(arg);

        case OPT_SID_ENABLE:                return parseNum(arg);
        case OPT_SID_ADDRESS:               return parseNum(arg);

        case OPT_SID_REVISION:              return parseEnum<SIDRevisionEnum>(arg);
        case OPT_SID_FILTER:                return parseBool(arg);
        case OPT_SID_SAMPLING:              return parseEnum<SamplingMethodEnum>(arg);
        case OPT_SID_POWER_SAVE:            return parseBool(arg);
        case OPT_SID_ENGINE:                return parseEnum<SIDEngineEnum>(arg);
        case OPT_AUD_PAN:                   return parseNum(arg);
        case OPT_AUD_VOL:                   return parseNum(arg);
        case OPT_AUD_VOL_L:                 return parseNum(arg);
        case OPT_AUD_VOL_R:                 return parseNum(arg);

        case OPT_RAM_PATTERN:               return parseEnum<RamPatternEnum>(arg);

        case OPT_SAVE_ROMS:                 return parseBool(arg);

        case OPT_DRV_AUTO_CONFIG:           return parseBool(arg);
        case OPT_DRV_TYPE:                  return parseEnum<DriveTypeEnum>(arg);
        case OPT_DRV_RAM:                   return parseEnum<DriveRamEnum>(arg);
        case OPT_DRV_PARCABLE:              return parseEnum<ParCableTypeEnum>(arg);
        case OPT_DRV_CONNECT:               return parseBool(arg);
        case OPT_DRV_POWER_SWITCH:          return parseBool(arg);
        case OPT_DRV_POWER_SAVE:            return parseBool(arg);
        case OPT_DRV_EJECT_DELAY:           return parseNum(arg);
        case OPT_DRV_SWAP_DELAY:            return parseNum(arg);
        case OPT_DRV_INSERT_DELAY:          return parseNum(arg);
        case OPT_DRV_PAN:                   return parseNum(arg);
        case OPT_DRV_POWER_VOL:             return parseNum(arg);
        case OPT_DRV_STEP_VOL:              return parseNum(arg);
        case OPT_DRV_INSERT_VOL:            return parseNum(arg);
        case OPT_DRV_EJECT_VOL:             return parseNum(arg);

        case OPT_DAT_MODEL:                 return parseEnum<DatasetteModelEnum>(arg);
        case OPT_DAT_CONNECT:               return parseBool(arg);

        case OPT_MOUSE_MODEL:               return parseEnum<MouseModelEnum>(arg);
        case OPT_MOUSE_SHAKE_DETECT:        return parseBool(arg);
        case OPT_MOUSE_VELOCITY:            return parseNum(arg);

        case OPT_JOY_AUTOFIRE:              return parseBool(arg);
        case OPT_JOY_AUTOFIRE_BULLETS:      return parseNum(arg);
        case OPT_JOY_AUTOFIRE_DELAY:        return parseNum(arg);

        default:
            // printf("Unrecognized option: %s\n", OptionEnum::key(opt));
            fatalError;
    }
}

bool
Configurable::isValidOption(Option opt) const
{
    for (auto &it: getOptions()) {
        if (it.first == opt) return true;
    }
    return false;
}

void 
Configurable::dumpConfig(std::ostream& os) const
{
    using namespace util;

    for (auto &opt: getOptions()) {

        auto name = opt2str(opt.first);
        auto help = "(" + opt.second + ")";
        auto arg  = arg2str(opt.first, getOption(opt.first));

        os << tab(name);
        os << std::setw(16) << std::left << std::setfill(' ') << arg;
        os <<help << std::endl;
    }
}

}

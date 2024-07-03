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
#include "Option.h"
#include "VirtualC64Types.h"
#include "Error.h"
#include "IOUtils.h"

namespace vc64 {

std::unique_ptr<OptionParser>
OptionParser::create(Option opt, i64 arg)
{
    auto enumParser = [&]<typename T>() { return std::unique_ptr<EnumParser<T>>(new EnumParser<T>(opt, arg)); };
    auto boolParser = [&]() { return std::unique_ptr<BoolParser>(new BoolParser(opt, arg)); };
    auto numParser  = [&](string unit = "") { return std::unique_ptr<NumParser>(new NumParser(opt, arg, unit)); };
    auto hexParser  = [&](string unit = "") { return std::unique_ptr<HexParser>(new HexParser(opt, arg, unit)); };

    switch (opt) {

        case OPT_HOST_SAMPLE_RATE:          return numParser(" Hz");
        case OPT_HOST_REFRESH_RATE:         return numParser(" fps");
        case OPT_HOST_FRAMEBUF_WIDTH:       return numParser(" pixels");
        case OPT_HOST_FRAMEBUF_HEIGHT:      return numParser(" pixels");

        case OPT_EMU_WARP_MODE:             return enumParser.template operator()<WarpModeEnum>();
        case OPT_EMU_WARP_BOOT:             return numParser(" sec");
        case OPT_EMU_VSYNC:                 return boolParser();
        case OPT_EMU_SPEED_ADJUST:          return numParser("%");
        case OPT_EMU_SNAPSHOTS:             return boolParser();
        case OPT_EMU_SNAPSHOT_DELAY:        return numParser(" sec");
        case OPT_EMU_RUN_AHEAD:             return numParser(" frames");

        case OPT_VICII_REVISION:            return enumParser.template operator()<VICIIRevisionEnum>();
        case OPT_VICII_GRAY_DOT_BUG:        return boolParser();
        case OPT_VICII_POWER_SAVE:          return boolParser();
        case OPT_VICII_HIDE_SPRITES:        return boolParser();
        case OPT_VICII_SS_COLLISIONS:       return boolParser();
        case OPT_VICII_SB_COLLISIONS:       return boolParser();
        case OPT_GLUE_LOGIC:                return enumParser.template operator()<GlueLogicEnum>();
        case OPT_VICII_CUT_LAYERS:          return numParser();
        case OPT_VICII_CUT_OPACITY:         return numParser("%");
        case OPT_DMA_DEBUG_ENABLE:          return boolParser();
        case OPT_DMA_DEBUG_MODE:            return enumParser.template operator()<DmaDisplayModeEnum>();
        case OPT_DMA_DEBUG_OPACITY:         return numParser("%");
        case OPT_DMA_DEBUG_CHANNEL0:        return boolParser();
        case OPT_DMA_DEBUG_CHANNEL1:        return boolParser();
        case OPT_DMA_DEBUG_CHANNEL2:        return boolParser();
        case OPT_DMA_DEBUG_CHANNEL3:        return boolParser();
        case OPT_DMA_DEBUG_CHANNEL4:        return boolParser();
        case OPT_DMA_DEBUG_CHANNEL5:        return boolParser();
        case OPT_DMA_DEBUG_COLOR0:          return numParser();
        case OPT_DMA_DEBUG_COLOR1:          return numParser();
        case OPT_DMA_DEBUG_COLOR2:          return numParser();
        case OPT_DMA_DEBUG_COLOR3:          return numParser();
        case OPT_DMA_DEBUG_COLOR4:          return numParser();
        case OPT_DMA_DEBUG_COLOR5:          return numParser();

        case OPT_USR_DEVICE:                return enumParser.template operator()<UserPortDeviceEnum>();

        case OPT_VID_WHITE_NOISE:           return boolParser();
            
        case OPT_MON_PALETTE:               return enumParser.template operator()<PaletteEnum>();
        case OPT_MON_BRIGHTNESS:            return numParser("%");
        case OPT_MON_CONTRAST:              return numParser("%");
        case OPT_MON_SATURATION:            return numParser("%");
        case OPT_MON_HCENTER:               return numParser();
        case OPT_MON_VCENTER:               return numParser();
        case OPT_MON_HZOOM:                 return numParser();
        case OPT_MON_VZOOM:                 return numParser();
        case OPT_MON_UPSCALER:              return enumParser.template operator()<UpscalerEnum>();
        case OPT_MON_BLUR:                  return boolParser();
        case OPT_MON_BLUR_RADIUS:           return numParser();
        case OPT_MON_BLOOM:                 return boolParser();
        case OPT_MON_BLOOM_RADIUS:          return numParser();
        case OPT_MON_BLOOM_BRIGHTNESS:      return numParser();
        case OPT_MON_BLOOM_WEIGHT:          return numParser();
        case OPT_MON_DOTMASK:               return enumParser.template operator()<DotmaskEnum>();
        case OPT_MON_DOTMASK_BRIGHTNESS:    return numParser();
        case OPT_MON_SCANLINES:             return enumParser.template operator()<ScanlinesEnum>();
        case OPT_MON_SCANLINE_BRIGHTNESS:   return numParser();
        case OPT_MON_SCANLINE_WEIGHT:       return numParser();
        case OPT_MON_DISALIGNMENT:          return boolParser();
        case OPT_MON_DISALIGNMENT_H:        return numParser();
        case OPT_MON_DISALIGNMENT_V:        return numParser();

        case OPT_POWER_GRID:                return enumParser.template operator()<PowerGridEnum>();

        case OPT_CIA_REVISION:              return enumParser.template operator()<CIARevisionEnum>();
        case OPT_CIA_TIMER_B_BUG:           return boolParser();

        case OPT_SID_ENABLE:                return boolParser();
        case OPT_SID_ADDRESS:               return hexParser();
        case OPT_SID_REVISION:              return enumParser.template operator()<SIDRevisionEnum>();
        case OPT_SID_FILTER:                return boolParser();
        case OPT_SID_ENGINE:                return enumParser.template operator()<SIDEngineEnum>();
        case OPT_SID_SAMPLING:              return enumParser.template operator()<SamplingMethodEnum>();
        case OPT_SID_POWER_SAVE:            return boolParser();

        case OPT_AUD_VOL0:                  return numParser("%");
        case OPT_AUD_VOL1:                  return numParser("%");
        case OPT_AUD_VOL2:                  return numParser("%");
        case OPT_AUD_VOL3:                  return numParser("%");
        case OPT_AUD_PAN0:                  return numParser();
        case OPT_AUD_PAN1:                  return numParser();
        case OPT_AUD_PAN2:                  return numParser();
        case OPT_AUD_PAN3:                  return numParser();
        case OPT_AUD_VOL_L:                 return numParser("%");
        case OPT_AUD_VOL_R:                 return numParser("%");

        case OPT_MEM_INIT_PATTERN:          return enumParser.template operator()<RamPatternEnum>();
        case OPT_MEM_HEATMAP:               return boolParser();
        case OPT_MEM_SAVE_ROMS:             return boolParser();

        case OPT_DRV_AUTO_CONFIG:           return boolParser();
        case OPT_DRV_TYPE:                  return enumParser.template operator()<DriveTypeEnum>();
        case OPT_DRV_RAM:                   return enumParser.template operator()<DriveRamEnum>();
        case OPT_DRV_SAVE_ROMS:             return boolParser();
        case OPT_DRV_PARCABLE:              return enumParser.template operator()<ParCableTypeEnum>();
        case OPT_DRV_CONNECT:               return boolParser();
        case OPT_DRV_POWER_SWITCH:          return boolParser();
        case OPT_DRV_POWER_SAVE:            return boolParser();
        case OPT_DRV_EJECT_DELAY:           return numParser(" frames");
        case OPT_DRV_SWAP_DELAY:            return numParser(" frames");
        case OPT_DRV_INSERT_DELAY:          return numParser(" frames");
        case OPT_DRV_PAN:                   return numParser();
        case OPT_DRV_POWER_VOL:             return numParser("%");
        case OPT_DRV_STEP_VOL:              return numParser("%");
        case OPT_DRV_INSERT_VOL:            return numParser("%");
        case OPT_DRV_EJECT_VOL:             return numParser("%");

        case OPT_DAT_MODEL:                 return enumParser.template operator()<DatasetteModelEnum>();
        case OPT_DAT_CONNECT:               return boolParser();

        case OPT_MOUSE_MODEL:               return enumParser.template operator()<MouseModelEnum>();
        case OPT_MOUSE_SHAKE_DETECT:        return boolParser();
        case OPT_MOUSE_VELOCITY:            return numParser();

        case OPT_AUTOFIRE:                  return boolParser();
        case OPT_AUTOFIRE_BURSTS:           return boolParser();
        case OPT_AUTOFIRE_BULLETS:          return numParser(" bullets");
        case OPT_AUTOFIRE_DELAY:            return numParser(" frames");

        case OPT_PADDLE_ORIENTATION:        return enumParser.template operator()<PaddleOrientationEnum>();

        case OPT_RS232_DEVICE:              return enumParser.template operator()<CommunicationDeviceEnum>();
        case OPT_RS232_BAUD:                return numParser(" Bd");

        case OPT_REC_FRAME_RATE:            return numParser(" fps");
        case OPT_REC_BIT_RATE:              return numParser(" kBit");
        case OPT_REC_SAMPLE_RATE:           return numParser(" Hz");
        case OPT_REC_ASPECT_X:              return numParser();
        case OPT_REC_ASPECT_Y:              return numParser();

        default:
            fatalError;
    }
}

i64 
OptionParser::parse(Option opt, const string &arg)
{
    return create(opt)->parse(arg);
}

string
OptionParser::asPlainString(Option opt, i64 arg)
{
    return create(opt, arg)->asPlainString();
}

string
OptionParser::asString(Option opt, i64 arg)
{
    return create(opt, arg)->asString();
}

string
OptionParser::keyList(Option opt)
{
    return create(opt)->keyList();
}

string
OptionParser::argList(Option opt)
{
    return create(opt)->argList();
}

string
HexParser::asPlainString()
{
    std::stringstream ss;
    ss << std::hex << "0x" << std::setw(4) << std::setfill('0') << arg;

    return ss.str();
}

}

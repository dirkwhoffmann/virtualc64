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
OptionParser::create(Opt opt, i64 arg)
{
    auto enumParser = [&]<class T, typename E>() { return std::unique_ptr<EnumParser<T,E>>(new EnumParser<T,E>(opt, arg)); };
    auto boolParser = [&]() { return std::unique_ptr<BoolParser>(new BoolParser(opt, arg)); };
    auto numParser  = [&](string unit = "") { return std::unique_ptr<NumParser>(new NumParser(opt, arg, unit)); };
    auto hexParser  = [&](string unit = "") { return std::unique_ptr<HexParser>(new HexParser(opt, arg, unit)); };

    switch (opt) {

        case Opt::HOST_SAMPLE_RATE:          return numParser(" Hz");
        case Opt::HOST_REFRESH_RATE:         return numParser(" fps");
        case Opt::HOST_FRAMEBUF_WIDTH:       return numParser(" pixels");
        case Opt::HOST_FRAMEBUF_HEIGHT:      return numParser(" pixels");

        case Opt::C64_WARP_MODE:             return enumParser.template operator()<WarpEnum,Warp>();
        case Opt::C64_WARP_BOOT:             return numParser(" sec");
        case Opt::C64_VSYNC:                 return boolParser();
        case Opt::C64_SPEED_BOOST:           return numParser("%");
        case Opt::C64_RUN_AHEAD:             return numParser(" frames");

        case Opt::DASM_NUMBERS:              return enumParser.template operator()<DasmNumbersEnum,DasmNumbers>();
            
        case Opt::VICII_REVISION:            return enumParser.template operator()<VICIIRevEnum,VICIIRev>();
        case Opt::VICII_GRAY_DOT_BUG:        return boolParser();
        case Opt::VICII_POWER_SAVE:          return boolParser();
        case Opt::VICII_HIDE_SPRITES:        return boolParser();
        case Opt::VICII_SS_COLLISIONS:       return boolParser();
        case Opt::VICII_SB_COLLISIONS:       return boolParser();
        case Opt::GLUE_LOGIC:                return enumParser.template operator()<GlueLogicEnum,GlueLogic>();
        case Opt::VICII_CUT_LAYERS:          return numParser();
        case Opt::VICII_CUT_OPACITY:         return numParser("%");
        case Opt::DMA_DEBUG_ENABLE:          return boolParser();
        case Opt::DMA_DEBUG_OVERLAY:         return boolParser();
        case Opt::DMA_DEBUG_MODE:            return enumParser.template operator()<DmaDisplayModeEnum,DmaDisplayMode>();
        case Opt::DMA_DEBUG_OPACITY:         return numParser("%");
        case Opt::DMA_DEBUG_CHANNEL0:        return boolParser();
        case Opt::DMA_DEBUG_CHANNEL1:        return boolParser();
        case Opt::DMA_DEBUG_CHANNEL2:        return boolParser();
        case Opt::DMA_DEBUG_CHANNEL3:        return boolParser();
        case Opt::DMA_DEBUG_CHANNEL4:        return boolParser();
        case Opt::DMA_DEBUG_CHANNEL5:        return boolParser();
        case Opt::DMA_DEBUG_COLOR0:          return numParser();
        case Opt::DMA_DEBUG_COLOR1:          return numParser();
        case Opt::DMA_DEBUG_COLOR2:          return numParser();
        case Opt::DMA_DEBUG_COLOR3:          return numParser();
        case Opt::DMA_DEBUG_COLOR4:          return numParser();
        case Opt::DMA_DEBUG_COLOR5:          return numParser();

        case Opt::EXP_REU_SPEED:             return numParser();

        case Opt::USR_DEVICE:                return enumParser.template operator()<UserPortDeviceEnum,UserPortDevice>();

        case Opt::VID_WHITE_NOISE:           return boolParser();
            
        case Opt::MON_PALETTE:               return enumParser.template operator()<PaletteEnum,Palette>();
        case Opt::MON_BRIGHTNESS:            return numParser("%");
        case Opt::MON_CONTRAST:              return numParser("%");
        case Opt::MON_SATURATION:            return numParser("%");
        case Opt::MON_HCENTER:               return numParser();
        case Opt::MON_VCENTER:               return numParser();
        case Opt::MON_HZOOM:                 return numParser();
        case Opt::MON_VZOOM:                 return numParser();
        case Opt::MON_UPSCALER:              return enumParser.template operator()<UpscalerEnum,Upscaler>();
        case Opt::MON_BLUR:                  return boolParser();
        case Opt::MON_BLUR_RADIUS:           return numParser();
        case Opt::MON_BLOOM:                 return boolParser();
        case Opt::MON_BLOOM_RADIUS:          return numParser();
        case Opt::MON_BLOOM_BRIGHTNESS:      return numParser();
        case Opt::MON_BLOOM_WEIGHT:          return numParser();
        case Opt::MON_DOTMASK:               return enumParser.template operator()<DotmaskEnum,Dotmask>();
        case Opt::MON_DOTMASK_BRIGHTNESS:    return numParser();
        case Opt::MON_SCANLINES:             return enumParser.template operator()<ScanlinesEnum,Scanlines>();
        case Opt::MON_SCANLINE_BRIGHTNESS:   return numParser();
        case Opt::MON_SCANLINE_WEIGHT:       return numParser();
        case Opt::MON_DISALIGNMENT:          return boolParser();
        case Opt::MON_DISALIGNMENT_H:        return numParser();
        case Opt::MON_DISALIGNMENT_V:        return numParser();

        case Opt::POWER_GRID:                return enumParser.template operator()<PowerGridEnum,PowerGrid>();

        case Opt::CIA_REVISION:              return enumParser.template operator()<CIARevisionEnum,CIARev>();
        case Opt::CIA_TIMER_B_BUG:           return boolParser();
        case Opt::CIA_IDLE_SLEEP:            return boolParser();

        case Opt::SID_ENABLE:                return boolParser();
        case Opt::SID_ADDRESS:               return hexParser();
        case Opt::SID_REV:              return enumParser.template operator()<SIDRevisionEnum,SIDRevision>();
        case Opt::SID_FILTER:                return boolParser();
        case Opt::SID_ENGINE:                return enumParser.template operator()<SIDEngineEnum,SIDEngine>();
        case Opt::SID_SAMPLING:              return enumParser.template operator()<SamplingMethodEnum,SamplingMethod>();
        case Opt::SID_POWER_SAVE:            return boolParser();

        case Opt::AUD_VOL0:                  return numParser("%");
        case Opt::AUD_VOL1:                  return numParser("%");
        case Opt::AUD_VOL2:                  return numParser("%");
        case Opt::AUD_VOL3:                  return numParser("%");
        case Opt::AUD_PAN0:                  return numParser();
        case Opt::AUD_PAN1:                  return numParser();
        case Opt::AUD_PAN2:                  return numParser();
        case Opt::AUD_PAN3:                  return numParser();
        case Opt::AUD_VOL_L:                 return numParser("%");
        case Opt::AUD_VOL_R:                 return numParser("%");
        case Opt::AUD_BUFFER_SIZE:           return numParser(" samples");
        case Opt::AUD_ASR:                   return boolParser();

        case Opt::MEM_INIT_PATTERN:          return enumParser.template operator()<RamPatternEnum,RamPattern>();
        case Opt::MEM_HEATMAP:               return boolParser();
        case Opt::MEM_SAVE_ROMS:             return boolParser();

        case Opt::DRV_AUTO_CONFIG:           return boolParser();
        case Opt::DRV_TYPE:                  return enumParser.template operator()<DriveTypeEnum,DriveType>();
        case Opt::DRV_RAM:                   return enumParser.template operator()<DriveRamEnum,DriveRam>();
        case Opt::DRV_SAVE_ROMS:             return boolParser();
        case Opt::DRV_PARCABLE:              return enumParser.template operator()<ParCableTypeEnum,ParCableType>();
        case Opt::DRV_CONNECT:               return boolParser();
        case Opt::DRV_POWER_SWITCH:          return boolParser();
        case Opt::DRV_POWER_SAVE:            return boolParser();
        case Opt::DRV_EJECT_DELAY:           return numParser(" frames");
        case Opt::DRV_SWAP_DELAY:            return numParser(" frames");
        case Opt::DRV_INSERT_DELAY:          return numParser(" frames");
        case Opt::DRV_PAN:                   return numParser();
        case Opt::DRV_POWER_VOL:             return numParser("%");
        case Opt::DRV_STEP_VOL:              return numParser("%");
        case Opt::DRV_INSERT_VOL:            return numParser("%");
        case Opt::DRV_EJECT_VOL:             return numParser("%");

        case Opt::DAT_MODEL:                 return enumParser.template operator()<DatasetteModelEnum,DatasetteModel>();
        case Opt::DAT_CONNECT:               return boolParser();

        case Opt::MOUSE_MODEL:               return enumParser.template operator()<MouseModelEnum,MouseModel>();
        case Opt::MOUSE_SHAKE_DETECT:        return boolParser();
        case Opt::MOUSE_VELOCITY:            return numParser();

        case Opt::AUTOFIRE:                  return boolParser();
        case Opt::AUTOFIRE_BURSTS:           return boolParser();
        case Opt::AUTOFIRE_BULLETS:          return numParser(" bullets");
        case Opt::AUTOFIRE_DELAY:            return numParser(" frames");

        case Opt::PADDLE_ORIENTATION:        return enumParser.template operator()<PaddleOrientationEnum,PaddleOrientation>();

        case Opt::RS232_DEVICE:              return enumParser.template operator()<CommunicationDeviceEnum,CommunicationDevice>();
        case Opt::RS232_BAUD:                return numParser(" Bd");

        case Opt::SRV_ENABLE:                return boolParser();
        case Opt::SRV_PORT:                  return numParser();
        case Opt::SRV_TRANSPORT:              return enumParser.template operator()<TransportProtocolEnum,TransportProtocol>();
        case Opt::SRV_VERBOSE:               return boolParser();

        case Opt::DBG_DEBUGCART:             return boolParser();
        case Opt::DBG_WATCHDOG:              return numParser();

        default:
            fatalError;
    }
}

i64 
OptionParser::parse(Opt opt, const string &arg)
{
    return create(opt)->parse(arg);
}

std::vector<std::pair<string, long>>
OptionParser::pairs(Opt opt)
{
    return create(opt)->pairs();
}

string
OptionParser::asPlainString(Opt opt, i64 arg)
{
    return create(opt, arg)->asPlainString();
}

string
OptionParser::asString(Opt opt, i64 arg)
{
    return create(opt, arg)->asString();
}

string
OptionParser::keyList(Opt opt)
{
    return create(opt)->keyList();
}

string
OptionParser::argList(Opt opt)
{
    return create(opt)->argList();
}

string
OptionParser::help(Opt opt, i64 arg)
{
    return create(opt)->help(arg);
}

string
HexParser::asPlainString()
{
    std::stringstream ss;
    ss << std::hex << "0x" << std::setw(4) << std::setfill('0') << arg;

    return ss.str();
}

}

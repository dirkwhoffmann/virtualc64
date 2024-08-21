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
#include "DmaDebugger.h"
#include "VICII.h"
#include "MsgQueue.h"

namespace vc64 {

void
DmaDebugger::_dump(Category category, std::ostream& os) const
{
    using namespace util;

    if (category == Category::Config) {

        dumpConfig(os);
    }
}

i64
DmaDebugger::getOption(Option option) const
{
    switch (option) {

        case OPT_DMA_DEBUG_ENABLE:      return config.dmaDebug;
        case OPT_DMA_DEBUG_MODE:        return config.dmaDisplayMode;
        case OPT_DMA_DEBUG_OPACITY:     return config.dmaOpacity;

        case OPT_VICII_CUT_LAYERS:      return config.cutLayers;
        case OPT_VICII_CUT_OPACITY:     return config.cutOpacity;

        case OPT_DMA_DEBUG_CHANNEL0:    return config.dmaChannel[0];
        case OPT_DMA_DEBUG_CHANNEL1:    return config.dmaChannel[1];
        case OPT_DMA_DEBUG_CHANNEL2:    return config.dmaChannel[2];
        case OPT_DMA_DEBUG_CHANNEL3:    return config.dmaChannel[3];
        case OPT_DMA_DEBUG_CHANNEL4:    return config.dmaChannel[4];
        case OPT_DMA_DEBUG_CHANNEL5:    return config.dmaChannel[5];
        case OPT_DMA_DEBUG_COLOR0:      return config.dmaColor[0];
        case OPT_DMA_DEBUG_COLOR1:      return config.dmaColor[1];
        case OPT_DMA_DEBUG_COLOR2:      return config.dmaColor[2];
        case OPT_DMA_DEBUG_COLOR3:      return config.dmaColor[3];
        case OPT_DMA_DEBUG_COLOR4:      return config.dmaColor[4];
        case OPT_DMA_DEBUG_COLOR5:      return config.dmaColor[5];

        default:
            fatalError;
    }
}

void
DmaDebugger::checkOption(Option opt, i64 value)
{
    switch (opt) {

        case OPT_DMA_DEBUG_ENABLE:

            return;

        case OPT_DMA_DEBUG_MODE:

            if (!DmaDisplayModeEnum::isValid(value)) {
                throw Error(VC64ERROR_OPT_INV_ARG, DmaDisplayModeEnum::keyList());
            }
            return;

        case OPT_DMA_DEBUG_OPACITY:
        case OPT_VICII_CUT_LAYERS:
        case OPT_VICII_CUT_OPACITY:

        case OPT_DMA_DEBUG_CHANNEL0:
        case OPT_DMA_DEBUG_CHANNEL1:
        case OPT_DMA_DEBUG_CHANNEL2:
        case OPT_DMA_DEBUG_CHANNEL3:
        case OPT_DMA_DEBUG_CHANNEL4:
        case OPT_DMA_DEBUG_CHANNEL5:

        case OPT_DMA_DEBUG_COLOR0:
        case OPT_DMA_DEBUG_COLOR1:
        case OPT_DMA_DEBUG_COLOR2:
        case OPT_DMA_DEBUG_COLOR3:
        case OPT_DMA_DEBUG_COLOR4:
        case OPT_DMA_DEBUG_COLOR5:

            return;

        default:
            throw Error(VC64ERROR_OPT_UNSUPPORTED);
    }
}

void
DmaDebugger::setOption(Option opt, i64 value)
{
    checkOption(opt, value);

    switch (opt) {

        case OPT_DMA_DEBUG_ENABLE:

            config.dmaDebug = value;
            vic.resetDmaTextures();
            vic.resetEmuTextures();
            msgQueue.put(MSG_DMA_DEBUG, value);
            return;

        case OPT_DMA_DEBUG_MODE:

            config.dmaDisplayMode = (DmaDisplayMode)value;
            return;

        case OPT_DMA_DEBUG_OPACITY:     config.dmaOpacity = (u8)value; return;
        case OPT_VICII_CUT_LAYERS:      config.cutLayers = (u16)value; return;
        case OPT_VICII_CUT_OPACITY:     config.cutOpacity = (u8)value; return;

        case OPT_DMA_DEBUG_CHANNEL0:    config.dmaChannel[0] = value; return;
        case OPT_DMA_DEBUG_CHANNEL1:    config.dmaChannel[1] = value; return;
        case OPT_DMA_DEBUG_CHANNEL2:    config.dmaChannel[2] = value; return;
        case OPT_DMA_DEBUG_CHANNEL3:    config.dmaChannel[3] = value; return;
        case OPT_DMA_DEBUG_CHANNEL4:    config.dmaChannel[4] = value; return;
        case OPT_DMA_DEBUG_CHANNEL5:    config.dmaChannel[5] = value; return;

        case OPT_DMA_DEBUG_COLOR0:      setDmaDebugColor(MEMACCESS_R, GpuColor((u32)value)); return;
        case OPT_DMA_DEBUG_COLOR1:      setDmaDebugColor(MEMACCESS_I, GpuColor((u32)value)); return;
        case OPT_DMA_DEBUG_COLOR2:      setDmaDebugColor(MEMACCESS_C, GpuColor((u32)value)); return;
        case OPT_DMA_DEBUG_COLOR3:      setDmaDebugColor(MEMACCESS_G, GpuColor((u32)value)); return;
        case OPT_DMA_DEBUG_COLOR4:      setDmaDebugColor(MEMACCESS_P, GpuColor((u32)value)); return;
        case OPT_DMA_DEBUG_COLOR5:      setDmaDebugColor(MEMACCESS_S, GpuColor((u32)value)); return;

        default:
            fatalError;
    }
}

}

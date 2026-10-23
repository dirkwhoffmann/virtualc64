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

#include "vcconfig.h"
#include "XRay.h"
#include "VICII.h"
#include "MsgQueue.h"

namespace vc64 {

void
XRay::_dump(Category category, std::ostream &os) const
{
    using namespace utl;

    if (category == Category::Config) {

        dumpConfig(os);
    }
}

i64
XRay::getOption(Opt option) const
{
    switch (option) {

        case Opt::XRAY_MODE:            return (i64)config.mode;
        case Opt::XRAY_OVERLAY:         return (i64)config.dmaOverlay;
        case Opt::XRAY_OVERLAY_STYLE:   return (i64)config.dmaDisplayMode;
        case Opt::XRAY_OVERLAY_OPACITY: return (i64)config.dmaOpacity;

        case Opt::VICII_CUT_LAYERS:     return (i64)config.cutLayers;

        case Opt::XRAY_DMA_CHANNEL0:    return (i64)config.dmaChannel[0];
        case Opt::XRAY_DMA_CHANNEL1:    return (i64)config.dmaChannel[1];
        case Opt::XRAY_DMA_CHANNEL2:    return (i64)config.dmaChannel[2];
        case Opt::XRAY_DMA_CHANNEL3:    return (i64)config.dmaChannel[3];
        case Opt::XRAY_DMA_CHANNEL4:    return (i64)config.dmaChannel[4];
        case Opt::XRAY_DMA_CHANNEL5:    return (i64)config.dmaChannel[5];
        case Opt::XRAY_COLOR0:          return (i64)config.dmaColor[0];
        case Opt::XRAY_COLOR1:          return (i64)config.dmaColor[1];
        case Opt::XRAY_COLOR2:          return (i64)config.dmaColor[2];
        case Opt::XRAY_COLOR3:          return (i64)config.dmaColor[3];
        case Opt::XRAY_COLOR4:          return (i64)config.dmaColor[4];
        case Opt::XRAY_COLOR5:          return (i64)config.dmaColor[5];
        case Opt::XRAY_COLOR6:          return (i64)config.dmaColor[6];
        case Opt::XRAY_COLOR7:          return (i64)config.dmaColor[7];
        case Opt::XRAY_COLOR8:          return (i64)config.dmaColor[8];
        case Opt::XRAY_COLOR9:          return (i64)config.dmaColor[9];
        case Opt::XRAY_COLOR10:         return (i64)config.dmaColor[10];
        case Opt::XRAY_COMPOSITOR:      return (i64)config.compositor;

        default:
            fatalError;
    }
}

void
XRay::checkOption(Opt opt, i64 value)
{
    switch (opt) {

        case Opt::XRAY_MODE:
            if (!XRayModeEnum::isValid(value)) {
                throw CoreError(CoreError::OPT_INV_ARG, XRayModeEnum::keyList());
            }
            return;

        case Opt::XRAY_OVERLAY:

            return;

        case Opt::XRAY_COMPOSITOR:
            if (!XRayCompositorEnum::isValid(value)) {
                throw CoreError(CoreError::OPT_INV_ARG, XRayCompositorEnum::keyList());
            }
            return;

        case Opt::XRAY_OVERLAY_STYLE: 
            if (!DmaDisplayModeEnum::isValid(value)) {
                throw CoreError(CoreError::OPT_INV_ARG, DmaDisplayModeEnum::keyList());
            }
            return;

        case Opt::XRAY_OVERLAY_OPACITY:
        case Opt::VICII_CUT_LAYERS:
        case Opt::XRAY_DMA_CHANNEL0:
        case Opt::XRAY_DMA_CHANNEL1:
        case Opt::XRAY_DMA_CHANNEL2:
        case Opt::XRAY_DMA_CHANNEL3:
        case Opt::XRAY_DMA_CHANNEL4:
        case Opt::XRAY_DMA_CHANNEL5:

        case Opt::XRAY_COLOR0:
        case Opt::XRAY_COLOR1:
        case Opt::XRAY_COLOR2:
        case Opt::XRAY_COLOR3:
        case Opt::XRAY_COLOR4:
        case Opt::XRAY_COLOR5:
        case Opt::XRAY_COLOR6:
        case Opt::XRAY_COLOR7:
        case Opt::XRAY_COLOR8:
        case Opt::XRAY_COLOR9:
        case Opt::XRAY_COLOR10:

            return;

        default:
            throw CoreError(CoreError::OPT_UNSUPPORTED);
    }
}

void
XRay::setOption(Opt opt, i64 value)
{
    checkOption(opt, value);

    switch (opt) {

        case Opt::XRAY_MODE:

            config.mode = (XRayMode)value;
            vic.resetXrayTextures();
            vic.resetEmuTextures();
            return;

        case Opt::XRAY_OVERLAY:
            
            config.dmaOverlay = value;
            msgQueue.put(Msg::XRAY, value);
            return;
            
        case Opt::XRAY_OVERLAY_STYLE: 
            config.dmaDisplayMode = (DmaDisplayMode)value;
            return;

        case Opt::XRAY_COMPOSITOR:      config.compositor = (XRayCompositor)value; return;
        case Opt::XRAY_OVERLAY_OPACITY: config.dmaOpacity = (u8)value; return;
        case Opt::VICII_CUT_LAYERS:      config.cutLayers = (u16)value; return;

        case Opt::XRAY_DMA_CHANNEL0:    config.dmaChannel[0] = value; return;
        case Opt::XRAY_DMA_CHANNEL1:    config.dmaChannel[1] = value; return;
        case Opt::XRAY_DMA_CHANNEL2:    config.dmaChannel[2] = value; return;
        case Opt::XRAY_DMA_CHANNEL3:    config.dmaChannel[3] = value; return;
        case Opt::XRAY_DMA_CHANNEL4:    config.dmaChannel[4] = value; return;
        case Opt::XRAY_DMA_CHANNEL5:    config.dmaChannel[5] = value; return;

        case Opt::XRAY_COLOR0:      setXRayColor(MemAccess::R, GpuColor<TexelFormat::ABGR>((u32)value)); return;
        case Opt::XRAY_COLOR1:      setXRayColor(MemAccess::I, GpuColor<TexelFormat::ABGR>((u32)value)); return;
        case Opt::XRAY_COLOR2:      setXRayColor(MemAccess::C, GpuColor<TexelFormat::ABGR>((u32)value)); return;
        case Opt::XRAY_COLOR3:      setXRayColor(MemAccess::G, GpuColor<TexelFormat::ABGR>((u32)value)); return;
        case Opt::XRAY_COLOR4:      setXRayColor(MemAccess::P, GpuColor<TexelFormat::ABGR>((u32)value)); return;
        case Opt::XRAY_COLOR5:      setXRayColor(MemAccess::S, GpuColor<TexelFormat::ABGR>((u32)value)); return;

        // Layer colors only: they have no DMA channel, hence no shade table
        case Opt::XRAY_COLOR6:
        case Opt::XRAY_COLOR7:
        case Opt::XRAY_COLOR8:
        case Opt::XRAY_COLOR9:
        case Opt::XRAY_COLOR10:

            config.dmaColor[long(opt) - long(Opt::XRAY_COLOR0)] = GpuColor<TexelFormat::ABGR>((u32)value).rawValue;
            return;

        default:
            fatalError;
    }
}

}

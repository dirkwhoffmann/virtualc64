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
#include "Monitor.h"
#include "MsgQueue.h"
#include "VICII.h"
#include <cmath>

namespace vc64 {

i64
Monitor::getOption(Opt option) const
{
    switch (option) {
            
        case Opt::MON_PALETTE:               return (i64)config.palette;
        case Opt::MON_BRIGHTNESS:            return (i64)config.brightness;
        case Opt::MON_CONTRAST:              return (i64)config.contrast;
        case Opt::MON_SATURATION:            return (i64)config.saturation;
        case Opt::MON_HCENTER:               return (i64)config.hCenter;
        case Opt::MON_VCENTER:               return (i64)config.vCenter;
        case Opt::MON_HZOOM:                 return (i64)config.hZoom;
        case Opt::MON_VZOOM:                 return (i64)config.vZoom;
        case Opt::MON_UPSCALER:              return (i64)config.upscaler;
        case Opt::MON_BLUR:                  return (i64)config.blur;
        case Opt::MON_BLUR_RADIUS:           return (i64)config.blurRadius;
        case Opt::MON_BLOOM:                 return (i64)config.bloom;
        case Opt::MON_BLOOM_RADIUS:          return (i64)config.bloomRadius;
        case Opt::MON_BLOOM_BRIGHTNESS:      return (i64)config.bloomBrightness;
        case Opt::MON_BLOOM_WEIGHT:          return (i64)config.bloomWeight;
        case Opt::MON_DOTMASK:               return (i64)config.dotmask;
        case Opt::MON_DOTMASK_BRIGHTNESS:    return (i64)config.dotMaskBrightness;
        case Opt::MON_SCANLINES:             return (i64)config.scanlines;
        case Opt::MON_SCANLINE_BRIGHTNESS:   return (i64)config.scanlineBrightness;
        case Opt::MON_SCANLINE_WEIGHT:       return (i64)config.scanlineWeight;
        case Opt::MON_DISALIGNMENT:          return (i64)config.disalignment;
        case Opt::MON_DISALIGNMENT_H:        return (i64)config.disalignmentH;
        case Opt::MON_DISALIGNMENT_V:        return (i64)config.disalignmentV;
            
        default:
            fatalError;
    }
}

void
Monitor::checkOption(Opt opt, i64 value)
{
    switch (opt) {
            
        case Opt::MON_PALETTE:
            
            if (!PaletteEnum::isValid(value)) {
                throw AppError(Fault::OPT_INV_ARG, PaletteEnum::keyList());
            }
            return;
            
        case Opt::MON_UPSCALER:
            
            if (!UpscalerEnum::isValid(value)) {
                throw AppError(Fault::OPT_INV_ARG, UpscalerEnum::keyList());
            }
            return;

        case Opt::MON_DOTMASK:
            
            if (!DotmaskEnum::isValid(value)) {
                throw AppError(Fault::OPT_INV_ARG, DotmaskEnum::keyList());
            }
            return;

        case Opt::MON_SCANLINES:
            
            if (!ScanlinesEnum::isValid(value)) {
                throw AppError(Fault::OPT_INV_ARG, ScanlinesEnum::keyList());
            }
            return;

            
        case Opt::MON_BRIGHTNESS:
        case Opt::MON_CONTRAST:
        case Opt::MON_SATURATION:
            
            if (value < 0 || value > 100) {
                throw AppError(Fault::OPT_INV_ARG, "0...100");
            }
            return;
            
        case Opt::MON_HCENTER:
        case Opt::MON_VCENTER:
        case Opt::MON_HZOOM:
        case Opt::MON_VZOOM:
        case Opt::MON_BLUR:
        case Opt::MON_BLUR_RADIUS:
        case Opt::MON_BLOOM:
        case Opt::MON_BLOOM_RADIUS:
        case Opt::MON_BLOOM_BRIGHTNESS:
        case Opt::MON_BLOOM_WEIGHT:
        case Opt::MON_DOTMASK_BRIGHTNESS:
        case Opt::MON_SCANLINE_BRIGHTNESS:
        case Opt::MON_SCANLINE_WEIGHT:
        case Opt::MON_DISALIGNMENT:
        case Opt::MON_DISALIGNMENT_H:
        case Opt::MON_DISALIGNMENT_V:
            
            if (value < 0 || value > 1000) {
                throw AppError(Fault::OPT_INV_ARG, "0...1000");
            }
            return;
            
        default:
            throw AppError(Fault::OPT_UNSUPPORTED);
    }
}

void
Monitor::setOption(Opt opt, i64 value)
{
    checkOption(opt, value);
    
    switch (opt) {
            
        case Opt::MON_PALETTE:
            
            config.palette = Palette(value);
            vic.updatePalette();
            break;
            
        case Opt::MON_BRIGHTNESS:
            
            config.brightness = isize(value);
            vic.updatePalette();
            break;
            
        case Opt::MON_CONTRAST:
            
            config.contrast = isize(value);
            vic.updatePalette();
            break;
            
        case Opt::MON_SATURATION:
            
            config.saturation = isize(value);
            vic.updatePalette();
            break;
            
        case Opt::MON_HCENTER:
            
            config.hCenter = isize(value);
            break;
            
        case Opt::MON_VCENTER:
            
            config.vCenter = isize(value);
            break;
            
        case Opt::MON_HZOOM:
            
            config.hZoom = isize(value);
            break;
            
        case Opt::MON_VZOOM:
            
            config.vZoom = isize(value);
            break;
            
        case Opt::MON_UPSCALER:
            
            config.upscaler = Upscaler(value);
            break;
            
        case Opt::MON_BLUR:
            
            config.blur = isize(value);
            break;
            
        case Opt::MON_BLUR_RADIUS:
            
            config.blurRadius = isize(value);
            break;
            
        case Opt::MON_BLOOM:
            
            config.bloom = isize(value);
            break;
            
        case Opt::MON_BLOOM_RADIUS:
            
            config.bloomRadius = isize(value);
            break;
            
        case Opt::MON_BLOOM_BRIGHTNESS:
            
            config.bloomBrightness = isize(value);
            break;
            
        case Opt::MON_BLOOM_WEIGHT:
            
            config.bloomWeight = isize(value);
            break;
            
        case Opt::MON_DOTMASK:
            
            config.dotmask = Dotmask(value);
            break;
            
        case Opt::MON_DOTMASK_BRIGHTNESS:
            
            config.dotMaskBrightness = isize(value);
            break;
            
        case Opt::MON_SCANLINES:
            
            config.scanlines = Scanlines(value);
            break;
            
        case Opt::MON_SCANLINE_BRIGHTNESS:
            
            config.scanlineBrightness = isize(value);
            break;
            
        case Opt::MON_SCANLINE_WEIGHT:
            
            config.scanlineWeight = isize(value);
            break;
            
        case Opt::MON_DISALIGNMENT:
            
            config.disalignment = isize(value);
            break;
            
        case Opt::MON_DISALIGNMENT_H:
            
            config.disalignmentH = isize(value);
            break;
            
        case Opt::MON_DISALIGNMENT_V:
            
            config.disalignmentV = isize(value);
            break;
            
        default:
            fatalError;
    }
    
    msgQueue.put(Msg::MON_SETTING, (i64)opt, value);
}

void
Monitor::_dump(Category category, std::ostream &os) const
{
    using namespace util;
    
    if (category == Category::Config) {
        
        dumpConfig(os);
    }
}

}

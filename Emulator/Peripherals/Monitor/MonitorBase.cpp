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
#include "VICII.h"
#include <cmath>

namespace vc64 {

i64
Monitor::getOption(Option option) const
{
    switch (option) {
            
        case OPT_MON_PALETTE:               return config.palette;
        case OPT_MON_BRIGHTNESS:            return config.brightness;
        case OPT_MON_CONTRAST:              return config.contrast;
        case OPT_MON_SATURATION:            return config.saturation;
        case OPT_MON_HCENTER:               return config.hCenter;
        case OPT_MON_VCENTER:               return config.vCenter;
        case OPT_MON_HZOOM:                 return config.hZoom;
        case OPT_MON_VZOOM:                 return config.vZoom;
        case OPT_MON_UPSCALER:              return config.upscaler;
        case OPT_MON_BLUR:                  return config.blur;
        case OPT_MON_BLUR_RADIUS:           return config.blurRadius;
        case OPT_MON_BLOOM:                 return config.bloom;
        case OPT_MON_BLOOM_RADIUS:          return config.bloomRadius;
        case OPT_MON_BLOOM_BRIGHTNESS:      return config.bloomBrightness;
        case OPT_MON_BLOOM_WEIGHT:          return config.bloomWeight;
        case OPT_MON_DOTMASK:               return config.dotmask;
        case OPT_MON_DOTMASK_BRIGHTNESS:    return config.dotMaskBrightness;
        case OPT_MON_SCANLINES:             return config.scanlines;
        case OPT_MON_SCANLINE_BRIGHTNESS:   return config.scanlineBrightness;
        case OPT_MON_SCANLINE_WEIGHT:       return config.scanlineWeight;
        case OPT_MON_DISALIGNMENT:          return config.disalignment;
        case OPT_MON_DISALIGNMENT_H:        return config.disalignmentH;
        case OPT_MON_DISALIGNMENT_V:        return config.disalignmentV;
            
        default:
            fatalError;
    }
}

void
Monitor::checkOption(Option opt, i64 value)
{
    switch (opt) {
            
        case OPT_MON_PALETTE:
            
            if (!PaletteEnum::isValid(value)) {
                throw Error(VC64ERROR_OPT_INV_ARG, PaletteEnum::keyList());
            }
            return;
            
        case OPT_MON_BRIGHTNESS:
        case OPT_MON_CONTRAST:
        case OPT_MON_SATURATION:
            
            if (value < 0 || value > 100) {
                throw Error(VC64ERROR_OPT_INV_ARG, "0...100");
            }
            return;
            
        case OPT_MON_HCENTER:
        case OPT_MON_VCENTER:
        case OPT_MON_HZOOM:
        case OPT_MON_VZOOM:
            
            return;
            
        case OPT_MON_UPSCALER:
            
            if (!UpscalerEnum::isValid(value)) {
                throw Error(VC64ERROR_OPT_INV_ARG, UpscalerEnum::keyList());
            }
            return;
            
        case OPT_MON_BLUR:
        case OPT_MON_BLUR_RADIUS:
        case OPT_MON_BLOOM:
        case OPT_MON_BLOOM_RADIUS:
        case OPT_MON_BLOOM_BRIGHTNESS:
        case OPT_MON_BLOOM_WEIGHT:
            
            return;
            
        case OPT_MON_DOTMASK:
            
            if (!DotmaskEnum::isValid(value)) {
                throw Error(VC64ERROR_OPT_INV_ARG, DotmaskEnum::keyList());
            }
            return;
            
        case OPT_MON_DOTMASK_BRIGHTNESS:
            
            return;
            
        case OPT_MON_SCANLINES:
            
            if (!ScanlinesEnum::isValid(value)) {
                throw Error(VC64ERROR_OPT_INV_ARG, ScanlinesEnum::keyList());
            }
            return;
            
        case OPT_MON_SCANLINE_BRIGHTNESS:
        case OPT_MON_SCANLINE_WEIGHT:
        case OPT_MON_DISALIGNMENT:
        case OPT_MON_DISALIGNMENT_H:
        case OPT_MON_DISALIGNMENT_V:
            
            return;
            
        default:
            throw Error(VC64ERROR_OPT_UNSUPPORTED);
    }
}

void
Monitor::setOption(Option opt, i64 value)
{
    checkOption(opt, value);
    
    switch (opt) {
            
        case OPT_MON_PALETTE:
            
            config.palette = Palette(value);
            vic.updatePalette();
            return;
            
        case OPT_MON_BRIGHTNESS:
            
            config.brightness = isize(value);
            vic.updatePalette();
            return;
            
        case OPT_MON_CONTRAST:
            
            config.contrast = isize(value);
            vic.updatePalette();
            return;
            
        case OPT_MON_SATURATION:
            
            config.saturation = isize(value);
            vic.updatePalette();
            return;
            
        case OPT_MON_HCENTER:
            
            config.hCenter = isize(value);
            return;
            
        case OPT_MON_VCENTER:
            
            config.vCenter = isize(value);
            return;
            
        case OPT_MON_HZOOM:
            
            config.hZoom = isize(value);
            return;
            
        case OPT_MON_VZOOM:
            
            config.vZoom = isize(value);
            return;
            
        case OPT_MON_UPSCALER:
            
            config.upscaler = Upscaler(value);
            return;
            
        case OPT_MON_BLUR:
            
            config.blur = isize(value);
            return;
            
        case OPT_MON_BLUR_RADIUS:
            
            config.blurRadius = isize(value);
            return;
            
        case OPT_MON_BLOOM:
            
            config.bloom = isize(value);
            return;
            
        case OPT_MON_BLOOM_RADIUS:
            
            config.bloomRadius = isize(value);
            return;
            
        case OPT_MON_BLOOM_BRIGHTNESS:
            
            config.bloomBrightness = isize(value);
            return;
            
        case OPT_MON_BLOOM_WEIGHT:
            
            config.bloomWeight = isize(value);
            return;
            
        case OPT_MON_DOTMASK:
            
            config.dotmask = Dotmask(value);
            return;
            
        case OPT_MON_DOTMASK_BRIGHTNESS:
            
            config.dotMaskBrightness = isize(value);
            return;
            
        case OPT_MON_SCANLINES:
            
            config.scanlines = Scanlines(value);
            return;
            
        case OPT_MON_SCANLINE_BRIGHTNESS:
            
            config.scanlineBrightness = isize(value);
            return;
            
        case OPT_MON_SCANLINE_WEIGHT:
            
            config.scanlineWeight = isize(value);
            return;
            
        case OPT_MON_DISALIGNMENT:
            
            config.disalignment = isize(value);
            return;
            
        case OPT_MON_DISALIGNMENT_H:
            
            config.disalignmentH = isize(value);
            return;
            
        case OPT_MON_DISALIGNMENT_V:
            
            config.disalignmentV = isize(value);
            return;
            
        default:
            fatalError;
    }
}

void
Monitor::_dump(Category category, std::ostream& os) const
{
    using namespace util;
    
    if (category == Category::Config) {
        
        dumpConfig(os);
    }
}

}

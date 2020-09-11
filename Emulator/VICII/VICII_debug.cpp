// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "C64.h"


ScreenGeometry
VICII::getScreenGeometry(void)
{
    unsigned rows = GET_BIT(reg.current.ctrl1, 3) ? 25 : 24;
    unsigned cols = GET_BIT(reg.current.ctrl2, 3) ? 40 : 38;
    
    if (cols == 40) {
        return rows == 25 ? COL_40_ROW_25 : COL_40_ROW_24;
    } else {
        return rows == 25 ? COL_38_ROW_25 : COL_38_ROW_24;
    }
}

void
VICII::computeOverlay()
{
    double bgWeight, fgWeight;

    // Only proceed if DMA debugging has been turned on
    if (!visualize[0]) return;

    switch (displayMode) {

        case MODULATE_FG_LAYER:

            bgWeight = 0.0;
            fgWeight = 1.0 - opacity;
            break;

        case MODULATE_BG_LAYER:

            bgWeight = 1.0 - opacity;
            fgWeight = 0.0;
            break;

        case MODULATE_ODD_EVEN_LAYERS:

            bgWeight = opacity;
            fgWeight = 1.0 - opacity;
            break;

        default: assert(false);

    }

    for (int y = 0; y < TEX_HEIGHT; y++) {
        
        int *emu = emuTexture + (y * TEX_WIDTH);
        int *dma = dmaTexture + (y * TEX_WIDTH);
        
        for (int x = 0; x < TEX_WIDTH; x++) {
            
            GpuColor emuColor = emu[x];
            GpuColor dmaColor = dma[x];
            GpuColor mixColor = emuColor.mix(dmaColor, fgWeight);
            
            emu[x] = mixColor.rawValue;
        }
    }
}

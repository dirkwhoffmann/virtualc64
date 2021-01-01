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
        return rows == 25 ? ScreenGeometry_25_40 : ScreenGeometry_24_40;
    } else {
        return rows == 25 ? ScreenGeometry_25_38 : ScreenGeometry_24_38;
    }
}

void
VICII::visualizeDma(u8 offset, u8 data, MemAccess type)
{
    int *p = dmaTexturePtr + bufferoffset + offset;
    
    p[3] = debugColor[type][data & 0b11]; data >>= 2;
    p[2] = debugColor[type][data & 0b11]; data >>= 2;
    p[1] = debugColor[type][data & 0b11]; data >>= 2;
    p[0] = debugColor[type][data & 0b11];
}

void
VICII::computeOverlay()
{
    // double bgWeight, fgWeight;
    double weight = config.dmaOpacity / 255.0;

    switch (config.dmaDisplayMode) {

        case DmaDisplayMode_FG_LAYER:
            
            for (int y = 0; y < TEX_HEIGHT; y++) {
                
                int *emu = emuTexture + (y * TEX_WIDTH);
                int *dma = dmaTexture + (y * TEX_WIDTH);
                
                for (int x = 0; x < TEX_WIDTH; x++) {
                    
                    if ((dma[x] & 0xFFFFFF) == 0) continue;

                    GpuColor emuColor = emu[x];
                    GpuColor dmaColor = dma[x];
                    GpuColor mixColor = emuColor.mix(dmaColor, weight);
                    emu[x] = mixColor.rawValue;
                }
            }
            break;

        case DmaDisplayMode_BG_LAYER:
            
            for (int y = 0; y < TEX_HEIGHT; y++) {
                
                int *emu = emuTexture + (y * TEX_WIDTH);
                int *dma = dmaTexture + (y * TEX_WIDTH);
                
                for (int x = 0; x < TEX_WIDTH; x++) {
                    
                    if ((dma[x] & 0xFFFFFF) != 0) {
                        emu[x] = dma[x];
                    } else {
                        GpuColor emuColor = emu[x];
                        GpuColor mixColor = emuColor.shade(weight);
                        emu[x] = mixColor.rawValue;
                    }
                }
            }
            break;

        case DmaDisplayMode_ODD_EVEN_LAYERS:
            
            for (int y = 0; y < TEX_HEIGHT; y++) {
                
                int *emu = emuTexture + (y * TEX_WIDTH);
                int *dma = dmaTexture + (y * TEX_WIDTH);
                
                for (int x = 0; x < TEX_WIDTH; x++) {
                    
                    GpuColor emuColor = emu[x];
                    GpuColor dmaColor = dma[x];
                    GpuColor mixColor = dmaColor.mix(emuColor, weight);
                    emu[x] = mixColor.rawValue;
                }
            }
            break;
            
        default: assert(false);
            
    }
}

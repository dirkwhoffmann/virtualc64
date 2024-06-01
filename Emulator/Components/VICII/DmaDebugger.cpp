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
#include "Emulator.h"

namespace vc64 {

DmaDebugger::DmaDebugger(C64 &ref) : SubComponent(ref)
{

}

void
DmaDebugger::setDmaDebugColor(MemAccess type, GpuColor color)
{
    assert_enum(MemAccess, type);
    
    config.dmaColor[type] = color.abgr;

    debugColor[type][0] = color.shade(0.3).abgr;
    debugColor[type][1] = color.shade(0.1).abgr;
    debugColor[type][2] = color.tint(0.1).abgr;
    debugColor[type][3] = color.tint(0.3).abgr;
}

void
DmaDebugger::setDmaDebugColor(MemAccess type, RgbColor color)
{
    setDmaDebugColor(type, GpuColor(color));
}

void
DmaDebugger::visualizeDma(isize offset, u8 data, MemAccess type)
{
    visualizeDma((u32 *)vic.dmaTexturePtr + offset, data, type);
}

void
DmaDebugger::visualizeDma(u32 *p, u8 data, MemAccess type)
{
    if (config.dmaChannel[type]) {
        
        p[3] = debugColor[type][data & 0b11]; data >>= 2;
        p[2] = debugColor[type][data & 0b11]; data >>= 2;
        p[1] = debugColor[type][data & 0b11]; data >>= 2;
        p[0] = debugColor[type][data & 0b11];
    }
}

void
DmaDebugger::computeOverlay(u32 *emuTexture, u32 *dmaTexture)
{
    double weight = config.dmaOpacity / 255.0;
    
    switch (config.dmaDisplayMode) {

        case DMA_DISPLAY_MODE_FG_LAYER:

            for (isize y = 0; y < Texture::height; y++) {

                u32 *emu = emuTexture + (y * Texture::width);
                u32 *dma = dmaTexture + (y * Texture::width);

                for (isize x = 0; x < Texture::width; x++) {

                    if ((dma[x] & 0xFFFFFF) == 0) continue;
                    
                    GpuColor emuColor = emu[x];
                    GpuColor dmaColor = dma[x];
                    GpuColor mixColor = emuColor.mix(dmaColor, weight);
                    emu[x] = mixColor.abgr;
                }
            }
            break;

        case DMA_DISPLAY_MODE_BG_LAYER:
            
            for (isize y = 0; y < Texture::height; y++) {

                u32 *emu = emuTexture + (y * Texture::width);
                u32 *dma = dmaTexture + (y * Texture::width);

                for (isize x = 0; x < Texture::width; x++) {

                    if ((dma[x] & 0xFFFFFF) != 0) {
                        emu[x] = dma[x];
                    } else {
                        GpuColor emuColor = emu[x];
                        GpuColor mixColor = emuColor.shade(weight);
                        emu[x] = mixColor.abgr;
                    }
                }
            }
            break;

        case DMA_DISPLAY_MODE_ODD_EVEN_LAYERS:
            
            for (isize y = 0; y < Texture::height; y++) {

                u32 *emu = emuTexture + (y * Texture::width);
                u32 *dma = dmaTexture + (y * Texture::width);

                for (isize x = 0; x < Texture::width; x++) {

                    GpuColor emuColor = emu[x];
                    GpuColor dmaColor = dma[x];
                    GpuColor mixColor = dmaColor.mix(emuColor, weight);
                    emu[x] = mixColor.abgr;
                }
            }
            break;
            
        default:
            fatalError;
    }
}

void
DmaDebugger::cutLayers()
{
    // Check master switch
    if (!(config.cutLayers & 0x1000)) return;
    
    // Only proceed if at least one channel is enabled
    if (!(config.cutLayers & 0x0F00)) return;
    
    u32 *emuTexturePtr = vic.emuTexturePtr;
    u8 *zBuffer = vic.zBuffer;
    
    for (isize i = 0; i < Texture::width; i++) {
        
        bool cut;

        switch (zBuffer[i] & 0xE0) {

            case DEPTH_BORDER & 0xE0:
                cut = config.cutLayers & 0x800;
                break;

            case DEPTH_FG & 0xE0:
                cut = config.cutLayers & 0x400;
                break;
                
            case DEPTH_BG & 0xE0:
                cut = config.cutLayers & 0x200;
                break;
                
            case DEPTH_SPRITE_BG & 0xE0:
            case DEPTH_SPRITE_FG & 0xE0:
                cut = GET_BIT(config.cutLayers, zBuffer[i] & 0xF);
                if (!(config.cutLayers & 0x100)) cut = false;
                break;
                
            default:
                cut = false;
        }
        
        if (cut) {
            
            u8 r = emuTexturePtr[i] & 0xFF;
            u8 g = (emuTexturePtr[i] >> 8) & 0xFF;
            u8 b = (emuTexturePtr[i] >> 16) & 0xFF;

            double scale = config.cutOpacity / 255.0;
            u8 bg = (vic.scanline() / 4) % 2 == (i / 4) % 2 ? 0x22 : 0x44;
            u8 newr = (u8)(r * (1 - scale) + bg * scale);
            u8 newg = (u8)(g * (1 - scale) + bg * scale);
            u8 newb = (u8)(b * (1 - scale) + bg * scale);
            
            emuTexturePtr[i] = 0xFF000000 | newb << 16 | newg << 8 | newr;
        }
    }
}

}

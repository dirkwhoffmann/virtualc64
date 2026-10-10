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
#include "DmaDebugger.h"
#include "Emulator.h"

namespace vc64 {

DmaDebugger::DmaDebugger(C64 &ref) : SubComponent(ref)
{

}

void
DmaDebugger::setDmaDebugColor(MemAccess type, GpuColor<TexelFormat::ABGR> color)
{
    setDmaDebugColor(type, RgbColor(color));
}

void
DmaDebugger::setDmaDebugColor(MemAccess type, RgbColor color)
{
    assert(MemAccessEnum::isValid(type));

    auto channel = (long)type;
    config.dmaColor[channel] = GpuColor<TexelFormat::ABGR>(color).rawValue;

    switch (static_cast<TexelFormat>(host.getConfig().texFormat)) {

        case TexelFormat::ABGR: updateDebugColor<TexelFormat::ABGR>(channel, color); break;
        case TexelFormat::ARGB: updateDebugColor<TexelFormat::ARGB>(channel, color); break;

        default: // RGBA
            updateDebugColor<TexelFormat::RGBA>(channel, color); break;
    }
}

void
DmaDebugger::updateDebugColors()
{
    for (long channel = 0; channel < 6; channel++) {
        setDmaDebugColor(MemAccess(channel), RgbColor(GpuColor<TexelFormat::ABGR>(config.dmaColor[channel])));
    }
}

template <TexelFormat F>
void
DmaDebugger::updateDebugColor(long channel, const RgbColor &color)
{
    GpuColor<F> gpuColor(color);

    debugColor[channel][0] = gpuColor.shade(0.3).rawValue;
    debugColor[channel][1] = gpuColor.shade(0.1).rawValue;
    debugColor[channel][2] = gpuColor.tint(0.1).rawValue;
    debugColor[channel][3] = gpuColor.tint(0.3).rawValue;
}

void
DmaDebugger::visualizeDma(isize offset, u8 data, MemAccess type)
{
    visualizeDma((u32 *)vic.xrayTexturePtr + offset, data, type);
}

void
DmaDebugger::visualizeDma(u32 *p, u8 data, MemAccess type)
{
    auto channel = (long)type;
    
    if (config.dmaChannel[channel]) {
        
        p[3] = debugColor[channel][data & 0b11]; data >>= 2;
        p[2] = debugColor[channel][data & 0b11]; data >>= 2;
        p[1] = debugColor[channel][data & 0b11]; data >>= 2;
        p[0] = debugColor[channel][data & 0b11];
    }
}

void
DmaDebugger::computeOverlay(u32 *emuTexture, u32 *xrayTexture)
{
    // Dispatched once per call (not per pixel) -- see the class comment in
    // DmaDebugger.h. Each branch below calls a separate instantiation of
    // the templated overload, so the format is a compile-time constant for
    // the whole per-pixel loop inside it.
    switch (static_cast<TexelFormat>(host.getConfig().texFormat)) {

        case TexelFormat::ABGR: computeOverlay<TexelFormat::ABGR>(emuTexture, xrayTexture); return;
        case TexelFormat::ARGB: computeOverlay<TexelFormat::ARGB>(emuTexture, xrayTexture); return;

        default: // RGBA
            computeOverlay<TexelFormat::RGBA>(emuTexture, xrayTexture); return;
    }
}

template <TexelFormat F>
void
DmaDebugger::computeOverlay(u32 *emuTexture, u32 *xrayTexture)
{
    double weight = config.dmaOpacity / 255.0;

    if (config.dmaOverlay) {

        switch (config.dmaDisplayMode) {

            case DmaDisplayMode::FG_LAYER:

                for (isize y = 0; y < Texture::height; y++) {

                    u32 *emu = emuTexture + (y * Texture::width);
                    u32 *dma = xrayTexture + (y * Texture::width);

                    for (isize x = 0; x < Texture::width; x++) {

                        if ((dma[x] & 0xFFFFFF) == 0) continue;

                        GpuColor<F> emuColor(emu[x]);
                        GpuColor<F> dmaColor(dma[x]);
                        GpuColor<F> mixColor = emuColor.mix(dmaColor, weight);
                        emu[x] = mixColor.rawValue;
                    }
                }
                break;

            case DmaDisplayMode::BG_LAYER:

                for (isize y = 0; y < Texture::height; y++) {

                    u32 *emu = emuTexture + (y * Texture::width);
                    u32 *dma = xrayTexture + (y * Texture::width);

                    for (isize x = 0; x < Texture::width; x++) {

                        if ((dma[x] & 0xFFFFFF) != 0) {
                            emu[x] = dma[x];
                        } else {
                            GpuColor<F> emuColor(emu[x]);
                            GpuColor<F> mixColor = emuColor.shade(weight);
                            emu[x] = mixColor.rawValue;
                        }
                    }
                }
                break;

            case DmaDisplayMode::ODD_EVEN_LAYERS:

                for (isize y = 0; y < Texture::height; y++) {

                    u32 *emu = emuTexture + (y * Texture::width);
                    u32 *dma = xrayTexture + (y * Texture::width);

                    for (isize x = 0; x < Texture::width; x++) {

                        GpuColor<F> emuColor(emu[x]);
                        GpuColor<F> dmaColor(dma[x]);
                        GpuColor<F> mixColor = dmaColor.mix(emuColor, weight);
                        emu[x] = mixColor.rawValue;
                    }
                }
                break;

            default:
                fatalError;
        }
    }
}

void
DmaDebugger::cutLayers()
{
    // Dispatched once per call (not per pixel) -- see computeOverlay above.
    switch (static_cast<TexelFormat>(host.getConfig().texFormat)) {

        case TexelFormat::ABGR: cutLayers<TexelFormat::ABGR>(); return;
        case TexelFormat::ARGB: cutLayers<TexelFormat::ARGB>(); return;

        default: // RGBA
            cutLayers<TexelFormat::RGBA>(); return;
    }
}

template <TexelFormat F>
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

            GpuColor<F> color(emuTexturePtr[i]);

            double scale = config.cutOpacity / 255.0;
            u8 bg = (vic.scanline() / 4) % 2 == (i / 4) % 2 ? 0x22 : 0x44;
            u8 newr = (u8)(color.r() * (1 - scale) + bg * scale);
            u8 newg = (u8)(color.g() * (1 - scale) + bg * scale);
            u8 newb = (u8)(color.b() * (1 - scale) + bg * scale);

            emuTexturePtr[i] = GpuColor<F>(newr, newg, newb).rawValue;
        }
    }
}

}

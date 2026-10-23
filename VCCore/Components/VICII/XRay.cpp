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
#include "Emulator.h"

namespace vc64 {

XRay::XRay(C64 &ref) : SubComponent(ref)
{

}

void
XRay::setXRayColor(MemAccess type, GpuColor<TexelFormat::ABGR> color)
{
    setXRayColor(type, RgbColor(color));
}

void
XRay::setXRayColor(MemAccess type, RgbColor color)
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
XRay::updateDebugColors()
{
    for (long channel = 0; channel < 6; channel++) {
        setXRayColor(MemAccess(channel), RgbColor(GpuColor<TexelFormat::ABGR>(config.dmaColor[channel])));
    }
}

template <TexelFormat F>
void
XRay::updateDebugColor(long channel, const RgbColor &color)
{
    GpuColor<F> gpuColor(color);

    debugColor[channel][0] = gpuColor.shade(0.3).rawValue;
    debugColor[channel][1] = gpuColor.shade(0.1).rawValue;
    debugColor[channel][2] = gpuColor.tint(0.1).rawValue;
    debugColor[channel][3] = gpuColor.tint(0.3).rawValue;
}

void
XRay::visualizeDma(isize offset, u8 data, MemAccess type)
{
    visualizeDma((u32 *)vic.xrayTexturePtr + offset, data, type);
}

void
XRay::visualizeDma(u32 *p, u8 data, MemAccess type)
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
XRay::computeOverlay(u32 *emuTexture, u32 *xrayTexture)
{
    // The host merges the textures itself (e.g., in a shader)
    if (config.compositor != XRayCompositor::CPU) return;

    // Dispatched once per call (not per pixel) -- see the class comment in
    // XRay.h. Each branch below calls a separate instantiation of
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
XRay::computeOverlay(u32 *emuTexture, u32 *xrayTexture)
{
    if (!config.dmaOverlay) return;

    /* A non-black X-ray pixel is an effect pixel that is mixed with the
     * picture. A black one dims the picture. The weights depend on the
     * display mode. They are the same in all cores, and they are the ones
     * used by the host if it merges the textures on the GPU (see the overlay
     * function in the merge shaders).
     */
    double opacity = double(config.dmaOpacity) / 255.0;
    double bgWeight = 0;
    double fgWeight = 0;

    switch (config.dmaDisplayMode) {

        case DmaDisplayMode::FG_LAYER:

            bgWeight = 0.0;
            fgWeight = 1.0 - opacity;
            break;

        case DmaDisplayMode::BG_LAYER:

            bgWeight = 1.0 - opacity;
            fgWeight = 0.0;
            break;

        case DmaDisplayMode::ODD_EVEN_LAYERS:

            bgWeight = opacity;
            fgWeight = 1.0 - opacity;
            break;

        default:
            fatalError;
    }

    for (isize i = 0; i < Texture::width * Texture::height; i++) {

        if ((xrayTexture[i] & 0xFFFFFF) != 0) {

            if (fgWeight != 0.0) {
                GpuColor<F> xrayColor(xrayTexture[i]);
                GpuColor<F> emuColor(emuTexture[i]);
                emuTexture[i] = xrayColor.mix(emuColor, fgWeight).rawValue;
            } else {
                emuTexture[i] = xrayTexture[i];
            }

        } else if (bgWeight != 0.0) {

            GpuColor<F> emuColor(emuTexture[i]);
            emuTexture[i] = emuColor.shade(bgWeight).rawValue;
        }
    }
}

void
XRay::cutLayers()
{
    // Cutting is a feature of the layer scan
    if (config.mode != XRayMode::XRAY_LAYERS) return;

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
XRay::cutLayers()
{
    /* Paints a checkerboard into the X-ray texture for each pixel that
     * belongs to a cut layer, and black (no effect) for all others. The
     * emulator texture is not touched, see computeOverlay() for how (and if)
     * the result ends up in the picture. The z buffer tells which layer
     * provided a pixel (see the depth constants in VICIITypes.h).
     */
    u32 *ptr = vic.xrayTexturePtr;
    u8 *zBuffer = vic.zBuffer;

    // Resolve the color slots into a light and a dark checkerboard shade
    u32 colA[XRAY_COLOR_COUNT], colB[XRAY_COLOR_COUNT];
    for (isize n = 0; n < XRAY_COLOR_COUNT; n++) {

        GpuColor<F> base(RgbColor(GpuColor<TexelFormat::ABGR>(config.dmaColor[n])));
        colA[n] = base.shade(0.3).rawValue;
        colB[n] = base.shade(0.1).rawValue;
    }

    bool rowBit = ((vic.scanline() / 4) & 1) != 0;

    for (isize i = 0; i < Texture::width; i++) {

        isize slot = -1;

        switch (zBuffer[i] & 0xE0) {

            case DEPTH_BORDER & 0xE0:
                if (config.cutLayers & 0x800) slot = 8;
                break;

            case DEPTH_FG & 0xE0:
                if (config.cutLayers & 0x400) slot = 9;
                break;

            case DEPTH_BG & 0xE0:
                if (config.cutLayers & 0x200) slot = 10;
                break;

            case DEPTH_SPRITE_BG & 0xE0:
            case DEPTH_SPRITE_FG & 0xE0:
                if (GET_BIT(config.cutLayers, zBuffer[i] & 0xF)) {
                    slot = zBuffer[i] & 0xF;
                }
                break;

            default:
                break;
        }

        bool colBit = ((i / 4) & 1) != 0;
        ptr[i] = slot < 0 ? Texture::black : (rowBit == colBit ? colA[slot] : colB[slot]);
    }
}

}

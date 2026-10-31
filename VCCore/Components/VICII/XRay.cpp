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

// Returns the number of accesses to the memory covered by a bar (k: 0 = reads, 1 = writes)
static u64
barTotal(const AccessTrace &trace, isize k, isize bar)
{
    constexpr isize span = 65536 / XRay::spectrumBars;

    const u32 *counters = k == 0 ? trace.reads : trace.writes;
    u64 total = 0;
    for (isize addr = bar * span; addr < (bar + 1) * span; addr++) total += counters[addr];
    return total;
}

void
XRay::updateAccessRecorder()
{
    auto &trace = mem.trace;
    auto mode = config.mode;

    trace.enabled = mode == XRayMode::XRAY_SPECTRUM || mode == XRayMode::XRAY_HEATMAP;
}

void
XRay::resetAccessMonitor()
{
    auto &trace = mem.trace;

    for (isize k = 0; k < 2; k++) {
        for (isize bar = 0; bar < spectrumBars; bar++) lastTotals[k][bar] = barTotal(trace, k, bar);
    }

    memset(bars, 0, sizeof(bars));
    memset(peaks, 0, sizeof(peaks));
    heatmap.reset(trace);
}

// Converts an access count into a level between 0 and 1 (logarithmic)
static double
accessLevel(u32 count, double full)
{
    return std::min(1.0, std::log2(1.0 + count) / std::log2(1.0 + full));
}

// Colors of the read and write graphs
static constexpr double readColor[3]  = { 0.15, 0.70, 1.00 };
static constexpr double writeColor[3] = { 1.00, 0.45, 0.15 };

void
XRay::drawSpectrum()
{
    switch (static_cast<TexelFormat>(host.getConfig().texFormat)) {

        case TexelFormat::ABGR: drawSpectrum<TexelFormat::ABGR>(); return;
        case TexelFormat::ARGB: drawSpectrum<TexelFormat::ARGB>(); return;

        default: // RGBA
            drawSpectrum<TexelFormat::RGBA>(); return;
    }
}

template <TexelFormat F>
void
XRay::drawSpectrum()
{
    /* One bar per 1KB of memory. Reads grow upwards from the center line,
     * writes downwards. The bar jumps up with the access count of the frame
     * and falls back slowly. The part of a bar that reflects the current
     * frame is bright, the part that is merely decaying is dim.
     */
    constexpr isize pitch = 8, barWidth = 6;
    constexpr isize x0 = (Texture::width - spectrumBars * pitch) / 2;
    constexpr isize centerY = Texture::height / 2;
    constexpr isize segments = 36, segHeight = 3, segPitch = 4;
    constexpr double decay = 0.92, peakFall = 0.012, fullScale = 4096;

    u32 *texture = vic.xrayTexture;
    auto &trace = mem.trace;

    auto plot = [&](isize x, isize y, double r, double g, double b) {

        if (x < 0 || x >= Texture::width || y < 0 || y >= Texture::height) return;
        auto c = [](double v) { return u8(255 * std::clamp(v, 0.0, 1.0)); };
        texture[y * Texture::width + x] = GpuColor<F>(c(r), c(g), c(b)).rawValue;
    };

    // Center line
    for (isize x = x0; x < x0 + spectrumBars * pitch; x++) plot(x, centerY, 0.2, 0.2, 0.25);

    for (isize bar = 0; bar < spectrumBars; bar++) {

        for (isize k = 0; k < 2; k++) {

            // Accesses within the last frame
            u64 total = barTotal(trace, k, bar);
            u32 count = u32(total - lastTotals[k][bar]);
            lastTotals[k][bar] = total;

            auto level = accessLevel(count, fullScale);
            bars[k][bar] = float(std::max(level, bars[k][bar] * decay));
            peaks[k][bar] = float(std::max(level, peaks[k][bar] - peakFall));

            isize lit = isize(bars[k][bar] * segments + 0.5);
            isize now = isize(level * segments + 0.5);
            isize cap = isize(peaks[k][bar] * segments + 0.5);
            const double *base = k == 0 ? readColor : writeColor;
            isize dir = k == 0 ? -1 : 1;

            for (isize s = 0; s < std::max(lit, cap); s++) {

                bool isCap = s == cap - 1 && cap > lit;
                if (s >= lit && !isCap) continue;

                double t = double(s) / (segments - 1);
                double glow = t * t * t * 0.5;
                double bright = isCap || s < now ? 1.0 : 0.45;
                double r = (base[0] * (0.45 + 0.55 * t) + glow) * bright;
                double g = (base[1] * (0.45 + 0.55 * t) + glow) * bright;
                double b = (base[2] * (0.45 + 0.55 * t) + glow) * bright;

                isize y = centerY + dir * (2 + s * segPitch);
                for (isize row = 0; row < segHeight; row++) {
                    for (isize col = 0; col < barWidth; col++) {
                        plot(x0 + bar * pitch + col, y + dir * row, r, g, b);
                    }
                }
            }
        }
    }

}

void
XRay::drawHeatmap()
{
    switch (static_cast<TexelFormat>(host.getConfig().texFormat)) {

        case TexelFormat::ABGR: drawHeatmap<TexelFormat::ABGR>(); return;
        case TexelFormat::ARGB: drawHeatmap<TexelFormat::ARGB>(); return;

        default: // RGBA
            drawHeatmap<TexelFormat::RGBA>(); return;
    }
}

template <TexelFormat F>
void
XRay::drawHeatmap()
{
    /* The heatmap grid is stretched over the whole texture. Each cell is
     * separated from its neighbors by a one pixel gap on its right and bottom
     * side. Cells that haven't been touched recently stay black (no effect).
     */
    constexpr isize cols = Heatmap::cols, rows = Heatmap::rows;
    constexpr isize width = Texture::width, height = Texture::height;

    u32 *texture = vic.xrayTexture;

    heatmap.update(mem.trace);

    float levels[rows][cols];
    heatmap.compute(levels);

    for (isize y = 0; y < height; y++) {

        isize row = y * rows / height;
        bool gapY = (y + 1) * rows / height != row;

        for (isize x = 0; x < width; x++) {

            isize col = x * cols / width;
            bool gapX = (x + 1) * cols / width != col;

            float level = levels[row][col];
            if (gapX || gapY || level <= 0) continue;

            texture[y * width + x] = GpuColor<F>(u8(255 * level), 0, 0).rawValue;
        }
    }
}

void
XRay::_pause()
{
    if (config.mode != XRayMode::XRAY_CROSSHAIRS) return;

    // Show the position the emulator has stopped at
    emulator.lockTexture();
    computeCrosshairs(vic.getStableXrayBuffer().pixels.ptr);
    emulator.unlockTexture();
}

void
XRay::computeCrosshairs(u32 *xray)
{
    switch (static_cast<TexelFormat>(host.getConfig().texFormat)) {

        case TexelFormat::ABGR: computeCrosshairs<TexelFormat::ABGR>(xray); return;
        case TexelFormat::ARGB: computeCrosshairs<TexelFormat::ARGB>(xray); return;

        default: // RGBA
            computeCrosshairs<TexelFormat::RGBA>(xray); return;
    }
}

template <TexelFormat F>
void
XRay::computeCrosshairs(u32 *xray)
{
    constexpr isize width = Texture::width, height = Texture::height;

    // Fill the texture with black
    for (isize i = 0; i < width * height; i++) xray[i] = Texture::black;

    // Locate the beam (one cycle covers 8 pixels)
    auto row = std::clamp(isize(c64.scanline), isize(0), height - 1);
    auto col = std::clamp(isize(c64.rasterCycle - 1) * 8 + 4, isize(0), width - 1);

    // Draw the crosshair
    auto color = GpuColor<F>(0xFF, 0x40, 0x40).rawValue;
    for (isize x = 0; x < width; x++) xray[row * width + x] = color;
    for (isize y = 0; y < height; y++) xray[y * width + col] = color;
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

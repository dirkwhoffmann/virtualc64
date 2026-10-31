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
XRay::drawHeatmap()
{
    // Dispatched once per call (not per pixel) -- see computeOverlay above.
    switch (static_cast<TexelFormat>(host.getConfig().texFormat)) {

        case TexelFormat::ABGR: drawHeatmap<TexelFormat::ABGR>(); return;
        case TexelFormat::ARGB: drawHeatmap<TexelFormat::ARGB>(); return;

        default: // RGBA
            drawHeatmap<TexelFormat::RGBA>(); return;
    }
}

/* Maps an address to a position in a 256 x 256 grid. This is the one place
 * where the mapping is chosen.
 *
 *   Linear:  The high byte selects the row, the low byte selects the column.
 *   Hilbert: Addresses are laid out along a Hilbert curve. Neighboring
 *            addresses stay close together, so code that executes
 *            sequentially crawls across the grid like a snake.
 */
static constexpr bool heatmapHilbert = true;

static void
heatmapPosition(u16 addr, isize &x, isize &y)
{
    if constexpr (!heatmapHilbert) {

        x = addr & 0xFF;
        y = addr >> 8;

    } else {

        u32 t = addr;
        u32 px = 0, py = 0;

        for (u32 s = 1; s < 256; s <<= 1) {

            u32 rx = 1 & (t >> 1);
            u32 ry = 1 & (t ^ rx);

            // Rotate the quadrant
            if (ry == 0) {
                if (rx == 1) { px = s - 1 - px; py = s - 1 - py; }
                std::swap(px, py);
            }
            px += s * rx;
            py += s * ry;
            t >>= 2;
        }
        x = px;
        y = py;
    }
}

template <TexelFormat F>
void
XRay::drawHeatmap()
{
    constexpr isize gridSize = 256;

    // Center the grid inside the texture
    constexpr isize x0 = (Texture::width - gridSize) / 2;
    constexpr isize y0 = (Texture::height - gridSize) / 2;
    static_assert(x0 >= 0 && y0 >= 0);

    // Colors of reads and writes
    constexpr u8 readRgb[3]  = { 0x40, 0xFF, 0x70 };
    constexpr u8 writeRgb[3] = { 0xFF, 0x50, 0x30 };

    u32 *texture = vic.xrayTexture;
    auto &trace = mem.trace;
    auto n = trace.count();

    /* Walk the trace from the oldest to the newest access. The newer an
     * access, the more opaque its dot. Opacity is emulated by scaling the
     * color towards black, because the compositor works with a single weight
     * per mode instead of a per-pixel alpha. Newer dots overwrite older ones.
     */
    for (usize i = 0; i < n; i++) {

        const auto &entry = trace[i];

        isize x, y;
        heatmapPosition(entry.addr, x, y);

        double age = double(i + 1) / double(n);
        double alpha = age * age;
        const u8 *rgb = entry.write ? writeRgb : readRgb;

        texture[(y0 + y) * Texture::width + x0 + x] =
            GpuColor<F>(u8(rgb[0] * alpha), u8(rgb[1] * alpha), u8(rgb[2] * alpha)).rawValue;
    }
}

void
XRay::updateAccessRecorder()
{
    auto &trace = mem.trace;
    auto mode = config.mode;

    trace.enabled = mode == XRayMode::XRAY_HEATMAP ||
                    mode == XRayMode::XRAY_SPECTRUM ||
                    mode == XRayMode::XRAY_SCOPE;
    trace.ring = mode == XRayMode::XRAY_HEATMAP;
}

void
XRay::resetAccessMonitor()
{
    auto &trace = mem.trace;

    trace.clear();
    for (isize i = 0; i < 256; i++) { lastReads[i] = trace.reads[i]; lastWrites[i] = trace.writes[i]; }

    memset(bars, 0, sizeof(bars));
    memset(peaks, 0, sizeof(peaks));
    memset(scope, 0, sizeof(scope));
}

// Converts an access count into a level between 0 and 1 (logarithmic)
static double
accessLevel(u32 count, double full)
{
    return std::min(1.0, std::log2(1.0 + count) / std::log2(1.0 + full));
}

/* Memory regions shown by the oscilloscope (one lane per region). The ranges
 * are given in pages (256 bytes).
 */
struct ScopeRegion { u8 first, last; };
static constexpr ScopeRegion scopeRegions[XRay::scopeLanes] = {

    { 0x00, 0x00 },     // Zero page
    { 0x01, 0x01 },     // Stack
    { 0x02, 0x9F },     // RAM
    { 0xA0, 0xBF },     // BASIC ROM
    { 0xC0, 0xCF },     // RAM
    { 0xD0, 0xD3 },     // VICII
    { 0xD4, 0xD7 },     // SID
    { 0xD8, 0xDF },     // Color RAM, CIAs
    { 0xE0, 0xFF }      // KERNAL ROM
};

// Colors of the read and write graphs
static constexpr double readColor[3]  = { 0.15, 0.70, 1.00 };
static constexpr double writeColor[3] = { 1.00, 0.45, 0.15 };

void
XRay::sampleScanline(isize line)
{
    if (config.mode != XRayMode::XRAY_SCOPE) return;
    if (line < 0 || line >= 512) return;

    auto &trace = mem.trace;

    for (isize lane = 0; lane < scopeLanes; lane++) {

        u32 r = 0, w = 0;
        for (isize p = scopeRegions[lane].first; p <= scopeRegions[lane].last; p++) {
            r += trace.reads[p] - lastReads[p];
            w += trace.writes[p] - lastWrites[p];
        }
        scope[0][lane][line] = u8(255 * accessLevel(r, 64));
        scope[1][lane][line] = u8(255 * accessLevel(w, 64));
    }
    for (isize i = 0; i < 256; i++) { lastReads[i] = trace.reads[i]; lastWrites[i] = trace.writes[i]; }
}

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

void
XRay::drawScope()
{
    switch (static_cast<TexelFormat>(host.getConfig().texFormat)) {

        case TexelFormat::ABGR: drawScope<TexelFormat::ABGR>(); return;
        case TexelFormat::ARGB: drawScope<TexelFormat::ARGB>(); return;

        default: // RGBA
            drawScope<TexelFormat::RGBA>(); return;
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
    constexpr isize pagesPerBar = 256 / spectrumBars;
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
            u32 count = 0;
            for (isize p = bar * pagesPerBar; p < (bar + 1) * pagesPerBar; p++) {
                count += k == 0 ? trace.reads[p] - lastReads[p] : trace.writes[p] - lastWrites[p];
            }

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

    // Start the next measurement
    for (isize i = 0; i < 256; i++) { lastReads[i] = trace.reads[i]; lastWrites[i] = trace.writes[i]; }
}

template <TexelFormat F>
void
XRay::drawScope()
{
    /* One lane per memory region with a time axis that spans a frame (x is
     * the scanline, so the picture is triggered by the vertical sync). The
     * read signal swings up from the lane's center line, the write signal
     * swings down.
     */
    constexpr isize laneHeight = (Texture::height - 6) / scopeLanes;
    constexpr isize swing = laneHeight / 2 - 2;

    u32 *texture = vic.xrayTexture;
    isize lines = std::min<isize>(vic.getLinesPerFrame(), 512);

    auto plot = [&](isize x, isize y, double r, double g, double b) {

        if (x < 0 || x >= Texture::width || y < 0 || y >= Texture::height) return;
        auto c = [](double v) { return u8(255 * std::clamp(v, 0.0, 1.0)); };
        texture[y * Texture::width + x] = GpuColor<F>(c(r), c(g), c(b)).rawValue;
    };

    auto line = [&](isize x1, isize y1, isize x2, isize y2, const double *col) {

        // Bresenham with a thickness of two pixels
        isize dx = std::abs(x2 - x1), dy = -std::abs(y2 - y1);
        isize sx = x1 < x2 ? 1 : -1, sy = y1 < y2 ? 1 : -1;
        isize err = dx + dy;

        while (true) {
            plot(x1, y1, col[0], col[1], col[2]);
            plot(x1, y1 + 1, col[0] * 0.6, col[1] * 0.6, col[2] * 0.6);
            if (x1 == x2 && y1 == y2) break;
            isize e2 = 2 * err;
            if (e2 >= dy) { err += dy; x1 += sx; }
            if (e2 <= dx) { err += dx; y1 += sy; }
        }
    };

    for (isize lane = 0; lane < scopeLanes; lane++) {

        isize centerY = 3 + lane * laneHeight + laneHeight / 2;

        // Center line
        for (isize x = 0; x < Texture::width; x++) plot(x, centerY, 0.12, 0.2, 0.15);

        for (isize k = 0; k < 2; k++) {

            isize dir = k == 0 ? -1 : 1;
            const double *col = k == 0 ? readColor : writeColor;

            isize px = 0, py = centerY;
            for (isize l = 0; l < lines; l++) {

                isize x = l * (Texture::width - 1) / (lines - 1);
                isize y = centerY + dir * (scope[k][lane][l] * swing / 255);
                if (l) line(px, py, x, y, col);
                px = x; py = y;
            }
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

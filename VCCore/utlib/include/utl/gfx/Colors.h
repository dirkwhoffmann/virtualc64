// -----------------------------------------------------------------------------
// This file is part of utlib - A lightweight utility library
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the Mozilla Public License v2
//
// See https://mozilla.org/MPL/2.0 for license information
// -----------------------------------------------------------------------------

#pragma once

#include "utl/common.h"
#include "utl/abilities/Streamable.h"
#include "GraphicsTypes.h"

namespace utl {

struct RgbColor;
struct YuvColor;
struct AmigaColor;
template <TexelFormat> struct GpuColor;

struct RgbColor : Streamable {

    double r;
    double g;
    double b;

    RgbColor() : r(0), g(0), b(0) {}
    RgbColor(double rv, double gv, double bv) : r(rv), g(gv), b(bv) {}
    RgbColor(u8 rv, u8 gv, u8 bv) : r(rv / 255.0), g(gv / 255.0), b(bv / 255.0) {}
    RgbColor(u32 rgba) : RgbColor(u8(rgba >> 24), u8(rgba >> 16), u8(rgba >> 8)) {}
    RgbColor(const YuvColor &c);
    RgbColor(const AmigaColor &c);
    template <TexelFormat F> RgbColor(const GpuColor<F> &c);

    static const RgbColor black;
    static const RgbColor white;
    static const RgbColor red;
    static const RgbColor green;
    static const RgbColor blue;
    static const RgbColor yellow;
    static const RgbColor magenta;
    static const RgbColor cyan;

    bool operator==(const RgbColor &rhs) const {
        return r == rhs.r && g == rhs.g && b == rhs.b;
    }

    RgbColor mix(RgbColor additive, double weight) const;
    RgbColor mix(RgbColor additive, double weight1, double weight2) const;
    RgbColor tint(double weight) const { return mix(white, weight); }
    RgbColor shade(double weight) const { return mix(black, weight); }
};

struct YuvColor {

    double y;
    double u;
    double v;

    YuvColor() : y(0), u(0), v(0) { }
    YuvColor(double yv, double uv, double vv) : y(yv), u(uv), v(vv) { }
    YuvColor(u8 yv, u8 uv, u8 vv) : y(yv / 255.0), u(uv / 255.0), v(vv / 255.0) { }
    YuvColor(const RgbColor &c);
    YuvColor(const AmigaColor &c) : YuvColor(RgbColor(c)) { }
    template <TexelFormat F> YuvColor(const GpuColor<F> &c) : YuvColor(RgbColor(c)) { }

    static const YuvColor black;
    static const YuvColor white;
    static const YuvColor red;
    static const YuvColor green;
    static const YuvColor blue;
    static const YuvColor yellow;
    static const YuvColor magenta;
    static const YuvColor cyan;

    bool operator==(const YuvColor &rhs) const {
        return y == rhs.y && u == rhs.u && v == rhs.v;
    }
};

struct AmigaColor : Streamable
{
    u8 r;
    u8 g;
    u8 b;

    AmigaColor() : r(0), g(0), b(0) {}
    AmigaColor(u8 rv, u8 gv, u8 bv) : r(rv), g(gv), b(bv) {}
    AmigaColor(u32 rgb) : r(u8(rgb >> 16)), g(u8(rgb >> 8)), b(u8(rgb)) {}
    AmigaColor(u16 hi, u16 lo) : r(0), g(0), b(0) { setHiNibbles(hi); setLoNibbles(lo); }
    AmigaColor(const RgbColor &c);
    AmigaColor(const YuvColor &c) : AmigaColor(RgbColor(c)) { }
    template <TexelFormat F> AmigaColor(const GpuColor<F> &c);

    template <class W>
    W& operator<<(W& worker)
    {
        worker

        << r << g << b;

        return worker;
    }

    u16 getHiNibbles() const {
        return u16((r >> 4) << 8 | (g >> 4) << 4 | (b >> 4));
    }
    u16 getLoNibbles() const {
        return u16((r & 0xF) << 8 | (g & 0xF) << 4 | (b & 0xF));
    }

    // Replaces the high or low nibble of each channel, leaving the other half intact
    void setHiNibbles(u16 hi) {
        r = u8((r & 0x0F) | (((hi >> 8) & 0xF) << 4));
        g = u8((g & 0x0F) | (((hi >> 4) & 0xF) << 4));
        b = u8((b & 0x0F) | (((hi >> 0) & 0xF) << 4));
    }
    void setLoNibbles(u16 lo) {
        r = u8((r & 0xF0) | ((lo >> 8) & 0xF));
        g = u8((g & 0xF0) | ((lo >> 4) & 0xF));
        b = u8((b & 0xF0) | ((lo >> 0) & 0xF));
    }

    static const AmigaColor black;
    static const AmigaColor white;
    static const AmigaColor red;
    static const AmigaColor green;
    static const AmigaColor blue;
    static const AmigaColor yellow;
    static const AmigaColor magenta;
    static const AmigaColor cyan;

    bool operator==(const AmigaColor &rhs) const {
        return r == rhs.r && g == rhs.g && b == rhs.b;
    }

    AmigaColor ehb() const;
    AmigaColor shr() const;

    /* Sub-pixel colors for ECS super hires.
     *
     * Super hires emits two pixels where hires emits one, and Denise has no
     * time to put a full 4 bit component on the DAC twice. It sends each
     * component's high bit pair to the first sub-pixel and its low bit pair
     * to the second, each pair replicated across the whole component. So a
     * component of $3 shows as $0 then $F, and $C as $F then $0 -- the same
     * pair of sub-pixels in the opposite order, which is why the two are
     * indistinguishable once a monitor blurs them together.
     *
     * Measured on an A500+ by Denise/Modes/shres/shramp1 to shramp7.
     */
    AmigaColor shresHi() const;
    AmigaColor shresLo() const;

    AmigaColor mix(const AmigaColor &c) const;
};

/* A 32-bit pixel color, laid out exactly as a texel of format F.
 *
 * A GpuColor<F>'s rawValue is always already packed as format F, so
 * constructing one from raw channel values or reading r()/g()/b()/a() back
 * out is correct by construction, for whichever F it was made with. There is
 * no separate "convert to/from the host's texel format" step left where the
 * format and the data could get out of sync -- which is exactly the bug this
 * design replaced (a fixed-layout GpuColor silently mixing the wrong channel
 * whenever the actual host format didn't match the layout it assumed).
 *
 * Only three instantiations exist (one per TexelFormat), explicitly
 * instantiated in Colors.cpp.
 */
template <TexelFormat F>
struct GpuColor {

    u32 rawValue;

    constexpr GpuColor() : rawValue(0) {}
    constexpr explicit GpuColor(u32 v) : rawValue(v) {}
    constexpr explicit GpuColor(u64 v) : rawValue(u32(v)) {}
    GpuColor(const RgbColor &c);
    GpuColor(const AmigaColor &c);
    constexpr GpuColor(u8 r, u8 g, u8 b, u8 a = 0xFF) : rawValue(pack(r, g, b, a)) {}

    // Packs/unpacks the four channels per F -- the only place that needs to
    // know the byte layout at all.
    static constexpr u32 pack(u8 r, u8 g, u8 b, u8 a)
    {
        if constexpr (F == TexelFormat::ABGR) return u32(a) << 24 | u32(b) << 16 | u32(g) << 8 | u32(r);
        else if constexpr (F == TexelFormat::ARGB) return u32(a) << 24 | u32(r) << 16 | u32(g) << 8 | u32(b);
        else return u32(r) << 24 | u32(g) << 16 | u32(b) << 8 | u32(a); // RGBA
    }

    constexpr u8 r() const
    {
        if constexpr (F == TexelFormat::ABGR) return u8(rawValue);
        else if constexpr (F == TexelFormat::ARGB) return u8(rawValue >> 16);
        else return u8(rawValue >> 24); // RGBA
    }
    constexpr u8 g() const
    {
        if constexpr (F == TexelFormat::RGBA) return u8(rawValue >> 16);
        else return u8(rawValue >> 8); // ABGR and ARGB agree on green's position
    }
    constexpr u8 b() const
    {
        if constexpr (F == TexelFormat::ABGR) return u8(rawValue >> 16);
        else if constexpr (F == TexelFormat::ARGB) return u8(rawValue);
        else return u8(rawValue >> 8); // RGBA
    }
    constexpr u8 a() const
    {
        if constexpr (F == TexelFormat::RGBA) return u8(rawValue);
        else return u8(rawValue >> 24); // ABGR and ARGB agree on alpha's position
    }

    static const GpuColor black;
    static const GpuColor white;
    static const GpuColor red;
    static const GpuColor green;
    static const GpuColor blue;
    static const GpuColor yellow;
    static const GpuColor magenta;
    static const GpuColor cyan;

    bool operator==(const GpuColor &rhs) const {
        return rawValue == rhs.rawValue;
    }

    GpuColor mix(const RgbColor &color, double weight) const;
    GpuColor mix(const RgbColor &color, double weight1, double weight2) const;
    GpuColor tint(double weight) const { return mix(RgbColor::white, weight); }
    GpuColor shade(double weight) const { return mix(RgbColor::black, weight); }
};

}

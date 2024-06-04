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

#pragma once

#include "Types.h"

namespace vc64 {

struct RgbColor {

    double r;
    double g;
    double b;

    RgbColor() : r(0), g(0), b(0) {}
    RgbColor(double rv, double gv, double bv) : r(rv), g(gv), b(bv) {}
    RgbColor(u8 rv, u8 gv, u8 bv) : r(rv / 255.0), g(gv / 255.0), b(bv / 255.0) {}
    RgbColor(const struct YuvColor &c);
    RgbColor(const struct GpuColor &c);

    static const RgbColor black;
    static const RgbColor white;
    static const RgbColor red;
    static const RgbColor green;
    static const RgbColor blue;
    static const RgbColor yellow;
    static const RgbColor magenta;
    static const RgbColor cyan;

    RgbColor mix(RgbColor additive, double weight);
    RgbColor mix(RgbColor additive, double weight1, double weight2);
    RgbColor tint(double weight) { return mix(white, weight); }
    RgbColor shade(double weight) { return mix(black, weight); }
};

struct YuvColor {

    double y;
    double u;
    double v;

    YuvColor() : y(0), u(0), v(0) { }
    YuvColor(double yv, double uv, double vv) : y(yv), u(uv), v(vv) { }
    YuvColor(u8 yv, u8 uv, u8 vv) : y(yv / 255.0), u(uv / 255.0), v(vv / 255.0) { }
    YuvColor(const struct RgbColor &c);
    YuvColor(const struct GpuColor &c) : YuvColor(RgbColor(c)) { }

    static const YuvColor black;
    static const YuvColor white;
    static const YuvColor red;
    static const YuvColor green;
    static const YuvColor blue;
    static const YuvColor yellow;
    static const YuvColor magenta;
    static const YuvColor cyan;
};

struct GpuColor {

    u32 abgr;

    GpuColor() : abgr(0) {}
    GpuColor(u32 v) : abgr(v) {}
    GpuColor(const struct RgbColor &c);
    GpuColor(u8 r, u8 g, u8 b);

    static const GpuColor black;
    static const GpuColor white;
    static const GpuColor red;
    static const GpuColor green;
    static const GpuColor blue;
    static const GpuColor yellow;
    static const GpuColor magenta;
    static const GpuColor cyan;

    GpuColor mix(const struct RgbColor &color, double weight);
    GpuColor mix(const struct RgbColor &color, double weight1, double weight2);
    GpuColor tint(double weight) { return mix(RgbColor::white, weight); }
    GpuColor shade(double weight) { return mix(RgbColor::black, weight); }
};

}

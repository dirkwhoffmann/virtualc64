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

#include "DmaDebuggerTypes.h"
#include "SubComponent.h"
#include "utl/gfx/Colors.h"

namespace vc64 {

class DmaDebugger final : public SubComponent {

    friend class VICII;
    
    Descriptions descriptions = {{

        .name           = "DmaDebugger",
        .description    = "Bus Monitor",
        .shell          = "dmadebugger"
    }};

    Options options = {

        Opt::XRAY_MODE,
        Opt::XRAY_OVERLAY,
        Opt::XRAY_OVERLAY_STYLE,
        Opt::XRAY_OVERLAY_OPACITY,
        Opt::VICII_CUT_LAYERS,
        Opt::XRAY_DMA_CHANNEL0,
        Opt::XRAY_DMA_CHANNEL1,
        Opt::XRAY_DMA_CHANNEL2,
        Opt::XRAY_DMA_CHANNEL3,
        Opt::XRAY_DMA_CHANNEL4,
        Opt::XRAY_DMA_CHANNEL5,
        Opt::XRAY_COLOR0,
        Opt::XRAY_COLOR1,
        Opt::XRAY_COLOR2,
        Opt::XRAY_COLOR3,
        Opt::XRAY_COLOR4,
        Opt::XRAY_COLOR5,
        Opt::XRAY_COLOR6,
        Opt::XRAY_COLOR7,
        Opt::XRAY_COLOR8,
        Opt::XRAY_COLOR9,
        Opt::XRAY_COLOR10,
        Opt::XRAY_COMPOSITOR
    };

    // Current configuration
    DmaDebuggerConfig config = { };
    
    // Color lookup table. There are 6 colors with 4 different shades
    u32 debugColor[6][4];

    
    //
    // Methods
    //
    
public:
    
    DmaDebugger(C64 &ref);

    DmaDebugger& operator= (const DmaDebugger& other) {

        for (isize i = 0; i < 6; i++) CLONE_ARRAY(debugColor[i])
        CLONE(config)

        return *this;
    }


    //
    // Methods from Serializable
    //

public:

    template <class T>
    void serialize(T& worker)
    {
        if (isResetter(worker)) return;

        worker

        << debugColor

        << config.mode
        << config.dmaChannel
        << config.dmaColor
        << config.dmaDisplayMode
        << config.dmaOpacity;

    } SERIALIZERS(serialize);


    //
    // Methods from CoreComponent
    //

public:

    const Descriptions &getDescriptions() const override { return descriptions; }

private:

    void _dump(Category category, std::ostream &os) const override;


    //
    // Methods from Configurable
    //

public:

    const DmaDebuggerConfig &getConfig() const { return config; }
    const Options &getOptions() const override { return options; }
    i64 getOption(Opt opt) const override;
    void checkOption(Opt opt, i64 value) override;
    void setOption(Opt opt, i64 value) override;


    //
    // Managing colors
    //

    // Sets a debug color in ABGR format
    void setDmaDebugColor(MemAccess type, GpuColor<TexelFormat::ABGR> color);
    void setDmaDebugColor(MemAccess type, RgbColor color);

    // Derives all color shades in the host's current HOST_TEX_FORMAT
    void updateDebugColors();

private:

    template <TexelFormat F> void updateDebugColor(long channel, const RgbColor &color);


    //
    // Visualizing DMA
    //

public:

    // Visualizes a memory access by drawing into the DMA debuger texture
    void visualizeDma(isize offset, u8 data, MemAccess type);
    void visualizeDma(u32 *ptr, u8 data, MemAccess type);

    // Superimposes the debug output onto the current scanline
    void computeOverlay(u32 *emuTexture, u32 *xrayTexture);

    template <TexelFormat F> void computeOverlay(u32 *emuTexture, u32 *xrayTexture);


    //
    // Cutting layers
    //

public:

    // Cuts out certain graphics layers. Same dispatch shape as
    // computeOverlay -- see there.
    void cutLayers();

    template <TexelFormat F> void cutLayers();
};

}

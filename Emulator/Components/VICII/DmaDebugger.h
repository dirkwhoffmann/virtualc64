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
#include "Colors.h"

namespace vc64 {

class DmaDebugger : public SubComponent, public Dumpable {

    friend class VICII;
    
    // Current configuration
    DmaDebuggerConfig config = { };
    
    // Color lookup table. There are 6 colors with different shades
    u32 debugColor[6][4];

    
    //
    // Initializing
    //
    
public:
    
    DmaDebugger(C64 &ref);
    
    
    //
    // Methods from CoreObject
    //

    const char *getDescription() const override { return "DmaDebugger"; }
    void _dump(Category category, std::ostream& os) const override;

    
    //
    // Methods from CoreComponent
    //

private:
    
    void _reset(bool hard) override { };
    
    template <class T> void serialize(T& worker) { }
    isize _size() override { return 0; }
    isize _load(const u8 *buffer) override {return 0; }
    isize _save(u8 *buffer) override { return 0; }
    
    
    //
    // Configuring
    //
    
public:
    
    static DmaDebuggerConfig getDefaultConfig();
    const DmaDebuggerConfig &getConfig() const { return config; }
    void resetConfig() override;

    i64 getConfigItem(Option option) const;
    i64 getConfigItem(Option option, long id) const;
    void setConfigItem(Option option, i64 value);
    void setConfigItem(Option option, long id, i64 value);
    
    
    //
    // Managing colors
    //
    
    void setDmaDebugColor(MemAccess type, GpuColor color);
    void setDmaDebugColor(MemAccess type, RgbColor color);

    
    //
    // Visualizing DMA
    //
    
public:
    
    // Visualizes a memory access by drawing into the DMA debuger texture
    void visualizeDma(isize offset, u8 data, MemAccess type);
    void visualizeDma(u32 *ptr, u8 data, MemAccess type);
    
    // Superimposes the debug output onto the current scanline
    void computeOverlay(u32 *emuTexture, u32 *dmaTexture);

    
    //
    // Cutting layers
    //
    
public:
    
    // Cuts out certain graphics layers
    void cutLayers();
};

}

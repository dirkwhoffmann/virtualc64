// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "DmaDebuggerTypes.h"
#include "C64Component.h"
#include "Colors.h"

class DmaDebugger : public C64Component {

    friend class VICII;
    
    // Current configuration
    DmaDebuggerConfig config = getDefaultConfig();
    
    // Color lookup table. There are 6 colors with different shades
    u32 debugColor[6][4];

    
    //
    // Initializing
    //
    
public:
    
    DmaDebugger(C64 &ref);
    const char *getDescription() const override { return "DmaDebugger"; }

private:
    
    void _reset(bool hard) override { };

    
    //
    // Configuring
    //
    
public:
    
    static DmaDebuggerConfig getDefaultConfig();
    DmaDebuggerConfig getConfig() const { return config; }
    void resetConfig() override;

    i64 getConfigItem(Option option) const;
    i64 getConfigItem(Option option, long id) const;
    bool setConfigItem(Option option, i64 value) override;
    bool setConfigItem(Option option, long id, i64 value) override;

    
    //
    // Analyzing
    //

private:
    
    void _dump(dump::Category category, std::ostream& os) const override;
    
    
    //
    // Serializing
    //
    
private:
    
    isize _size() override { return 0; }
    isize _load(const u8 *buffer) override {return 0; }
    isize _save(u8 *buffer) override { return 0; }
    
    
    //
    // Managing colors
    //
    
    void setDmaDebugColor(MemAccess type, GpuColor color);
    void setDmaDebugColor(MemAccess type, RgbColor color);

    
    //
    // Running the debugger
    //
    
public:
    
    // Visualizes a memory access by drawing into the DMA debuger texture
    void visualizeDma(isize offset, u8 data, MemAccess type);
    void visualizeDma(u32 *ptr, u8 data, MemAccess type);
    
    // Superimposes the debug output onto the current rasterline
    void computeOverlay(int *emuTexture, int *dmaTexture);

};

// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#include "config.h"
#include "DmaDebugger.h"
#include "C64.h"

DmaDebugger::DmaDebugger(C64 &ref) : C64Component(ref)
{
}

DmaDebuggerConfig
DmaDebugger::getDefaultConfig()
{
    DmaDebuggerConfig config;
    
    config.dmaDebug = false;
    config.dmaChannel[0] = true;
    config.dmaChannel[1] = true;
    config.dmaChannel[2] = true;
    config.dmaChannel[3] = true;
    config.dmaChannel[4] = true;
    config.dmaChannel[5] = true;
    config.dmaColor[0] = GpuColor(0xFF, 0x00, 0x00).rawValue;
    config.dmaColor[1] = GpuColor(0xFF, 0xC0, 0x00).rawValue;
    config.dmaColor[2] = GpuColor(0xFF, 0xFF, 0x00).rawValue;
    config.dmaColor[3] = GpuColor(0x00, 0xFF, 0xFF).rawValue;
    config.dmaColor[4] = GpuColor(0x00, 0xFF, 0x00).rawValue;
    config.dmaColor[5] = GpuColor(0x00, 0x80, 0xFF).rawValue;
    config.dmaDisplayMode = DMA_DISPLAY_MODE_FG_LAYER;
    config.dmaOpacity = 0x80;

    return config;
}
    
void
DmaDebugger::resetConfig()
{
    DmaDebuggerConfig defaults = getDefaultConfig();
    
    setConfigItem(OPT_DMA_DEBUG_ENABLE, defaults.dmaDebug);
    setConfigItem(OPT_DMA_DEBUG_ENABLE, 0, defaults.dmaChannel[0]);
    setConfigItem(OPT_DMA_DEBUG_ENABLE, 1, defaults.dmaChannel[1]);
    setConfigItem(OPT_DMA_DEBUG_ENABLE, 2, defaults.dmaChannel[2]);
    setConfigItem(OPT_DMA_DEBUG_ENABLE, 3, defaults.dmaChannel[3]);
    setConfigItem(OPT_DMA_DEBUG_ENABLE, 4, defaults.dmaChannel[4]);
    setConfigItem(OPT_DMA_DEBUG_ENABLE, 5, defaults.dmaChannel[5]);
    setConfigItem(OPT_DMA_DEBUG_COLOR, 0, defaults.dmaColor[0]);
    setConfigItem(OPT_DMA_DEBUG_COLOR, 1, defaults.dmaColor[1]);
    setConfigItem(OPT_DMA_DEBUG_COLOR, 2, defaults.dmaColor[2]);
    setConfigItem(OPT_DMA_DEBUG_COLOR, 3, defaults.dmaColor[3]);
    setConfigItem(OPT_DMA_DEBUG_COLOR, 4, defaults.dmaColor[4]);
    setConfigItem(OPT_DMA_DEBUG_COLOR, 5, defaults.dmaColor[5]);
    setConfigItem(OPT_DMA_DEBUG_MODE, defaults.dmaDisplayMode);
    setConfigItem(OPT_DMA_DEBUG_OPACITY, defaults.dmaOpacity);
}

i64
DmaDebugger::getConfigItem(Option option) const
{
    switch (option) {
            
        case OPT_DMA_DEBUG_ENABLE:  return config.dmaDebug;
        case OPT_DMA_DEBUG_MODE:    return config.dmaDisplayMode;
        case OPT_DMA_DEBUG_OPACITY: return config.dmaOpacity;

        default:
            assert(false);
            return 0;
    }
}

i64
DmaDebugger::getConfigItem(Option option, long id) const
{
    assert(id >= 0 && id < MEMACCESS_COUNT);
    
    switch (option) {
            
        case OPT_DMA_DEBUG_ENABLE: return config.dmaChannel[id];
        case OPT_DMA_DEBUG_COLOR: return config.dmaColor[id];
            
        default:
            assert(false);
            return 0;
    }
}

bool
DmaDebugger::setConfigItem(Option option, i64 value)
{
    switch (option) {
            
        case OPT_DMA_DEBUG_ENABLE:
                     
            suspend();
            config.dmaDebug = value;
            vic.resetDmaTextures();
            c64.updateVicFunctionTable();
            messageQueue.put(value ? MSG_DMA_DEBUG_ON : MSG_DMA_DEBUG_OFF);
            resume();
            return true;

        case OPT_DMA_DEBUG_MODE:
            
            if (!DmaDisplayModeEnum::isValid(value)) {
                throw ConfigArgError(DmaDisplayModeEnum::keyList());
            }
            config.dmaDisplayMode = (DmaDisplayMode)value;
            return true;

        case OPT_DMA_DEBUG_OPACITY:
            
            config.dmaOpacity = value;
            return false; // 'false' to avoid a MSG_CONFIG being sent
            
        default:
            return false;
    }
}

bool
DmaDebugger::setConfigItem(Option option, long id, i64 value)
{
    if (!MemAccessEnum::isValid(id)) { return false; }
    MemAccess access = (MemAccess)id;
    
    switch (option) {
            
        case OPT_DMA_DEBUG_ENABLE:
            
            config.dmaChannel[access] = value;
            return true;
            
        case OPT_DMA_DEBUG_COLOR:
            
            setDmaDebugColor(access, GpuColor((u32)value));
            return true;
            
        default:
            return false;
    }
}

void
DmaDebugger::_dump(dump::Category category, std::ostream& os) const
{
    using namespace util;
    
    if (category & dump::Config) {
        
        os << tab("Enabled");
        os << bol(config.dmaDebug) << std::endl;
        os << tab("Display mode");
        os << DmaDisplayModeEnum::key(config.dmaDisplayMode) << std::endl;
        os << tab("Opacity");
        os << dec(config.dmaOpacity) << std::endl;
        
        for (isize i = 0; i < 6; i++) {
            
            string channel = "Channel " + std::to_string(i);
            os << tab(channel);
            os << bol(config.dmaChannel[i], "shown", "hidden") << std::endl;
        }
    }
}

void
DmaDebugger::setDmaDebugColor(MemAccess type, GpuColor color)
{
    assert_enum(MemAccess, type);
    
    config.dmaColor[type] = color.rawValue;
        
    debugColor[type][0] = color.shade(0.3).rawValue;
    debugColor[type][1] = color.shade(0.1).rawValue;
    debugColor[type][2] = color.tint(0.1).rawValue;
    debugColor[type][3] = color.tint(0.3).rawValue;
}

void
DmaDebugger::setDmaDebugColor(MemAccess type, RgbColor color)
{
    setDmaDebugColor(type, GpuColor(color));
}

void
DmaDebugger::visualizeDma(isize offset, u8 data, MemAccess type)
{
    visualizeDma(vic.getDmaTexPtr(c64.rasterLine) + offset, data, type);
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
DmaDebugger::computeOverlay(int *emuTexture, int *dmaTexture)
{
    double weight = config.dmaOpacity / 255.0;

    switch (config.dmaDisplayMode) {

        case DMA_DISPLAY_MODE_FG_LAYER:
            
            for (int y = 0; y < TEX_HEIGHT; y++) {
                
                int *emu = emuTexture + (y * TEX_WIDTH);
                int *dma = dmaTexture + (y * TEX_WIDTH);
                
                for (int x = 0; x < TEX_WIDTH; x++) {
                    
                    if ((dma[x] & 0xFFFFFF) == 0) continue;

                    GpuColor emuColor = emu[x];
                    GpuColor dmaColor = dma[x];
                    GpuColor mixColor = emuColor.mix(dmaColor, weight);
                    emu[x] = mixColor.rawValue;
                }
            }
            break;

        case DMA_DISPLAY_MODE_BG_LAYER:
            
            for (int y = 0; y < TEX_HEIGHT; y++) {
                
                int *emu = emuTexture + (y * TEX_WIDTH);
                int *dma = dmaTexture + (y * TEX_WIDTH);
                
                for (int x = 0; x < TEX_WIDTH; x++) {
                    
                    if ((dma[x] & 0xFFFFFF) != 0) {
                        emu[x] = dma[x];
                    } else {
                        GpuColor emuColor = emu[x];
                        GpuColor mixColor = emuColor.shade(weight);
                        emu[x] = mixColor.rawValue;
                    }
                }
            }
            break;

        case DMA_DISPLAY_MODE_ODD_EVEN_LAYERS:
            
            for (int y = 0; y < TEX_HEIGHT; y++) {
                
                int *emu = emuTexture + (y * TEX_WIDTH);
                int *dma = dmaTexture + (y * TEX_WIDTH);
                
                for (int x = 0; x < TEX_WIDTH; x++) {
                    
                    GpuColor emuColor = emu[x];
                    GpuColor dmaColor = dma[x];
                    GpuColor mixColor = dmaColor.mix(emuColor, weight);
                    emu[x] = mixColor.rawValue;
                }
            }
            break;
            
        default: assert(false);
            
    }
}

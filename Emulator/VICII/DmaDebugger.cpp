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

DmaDebugger::DmaDebugger(C64 &ref) : SubComponent(ref)
{
}

DmaDebuggerConfig
DmaDebugger::getDefaultConfig()
{
    DmaDebuggerConfig defaults;
    
    defaults.dmaDebug = false;
    defaults.dmaChannel[0] = true;
    defaults.dmaChannel[1] = true;
    defaults.dmaChannel[2] = true;
    defaults.dmaChannel[3] = true;
    defaults.dmaChannel[4] = true;
    defaults.dmaChannel[5] = true;
    defaults.dmaColor[0] = GpuColor(0xFF, 0x00, 0x00).abgr;
    defaults.dmaColor[1] = GpuColor(0xFF, 0xC0, 0x00).abgr;
    defaults.dmaColor[2] = GpuColor(0xFF, 0xFF, 0x00).abgr;
    defaults.dmaColor[3] = GpuColor(0x00, 0xFF, 0xFF).abgr;
    defaults.dmaColor[4] = GpuColor(0x00, 0xFF, 0x00).abgr;
    defaults.dmaColor[5] = GpuColor(0x00, 0x80, 0xFF).abgr;
    defaults.dmaDisplayMode = DMA_DISPLAY_MODE_FG_LAYER;
    defaults.dmaOpacity = 0x80;
    
    defaults.cutLayers = 0xFF;
    defaults.cutOpacity = 0xFF;

    return defaults;
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
    
    setConfigItem(OPT_CUT_LAYERS, defaults.cutLayers);
    setConfigItem(OPT_CUT_OPACITY, defaults.cutOpacity);
}

i64
DmaDebugger::getConfigItem(Option option) const
{
    switch (option) {
            
        case OPT_DMA_DEBUG_ENABLE:  return config.dmaDebug;
        case OPT_DMA_DEBUG_MODE:    return config.dmaDisplayMode;
        case OPT_DMA_DEBUG_OPACITY: return config.dmaOpacity;

        case OPT_CUT_LAYERS:        return config.cutLayers;
        case OPT_CUT_OPACITY:       return config.cutOpacity;

        default:
            fatalError;
    }
}

i64
DmaDebugger::getConfigItem(Option option, long id) const
{
    assert(id >= 0 && id < MEMACCESS_COUNT);
    
    switch (option) {
            
        case OPT_DMA_DEBUG_ENABLE: return config.dmaChannel[id];
        case OPT_DMA_DEBUG_COLOR:  return config.dmaColor[id];
            
        default:
            fatalError;
    }
}

void
DmaDebugger::setConfigItem(Option option, i64 value)
{
    switch (option) {
            
        case OPT_DMA_DEBUG_ENABLE:
                     
            suspended {
                
                config.dmaDebug = value;
                vic.resetDmaTextures();
                vic.resetEmuTextures();
                vic.updateVicFunctionTable();
                msgQueue.put(value ? MSG_DMA_DEBUG_ON : MSG_DMA_DEBUG_OFF);
            }
            return;

        case OPT_DMA_DEBUG_MODE:
            
            if (!DmaDisplayModeEnum::isValid(value)) {
                throw VC64Error(ERROR_OPT_INVARG, DmaDisplayModeEnum::keyList());
            }
            config.dmaDisplayMode = (DmaDisplayMode)value;
            return;

        case OPT_DMA_DEBUG_OPACITY:
            
            config.dmaOpacity = (u8)value;
            return;
            
        case OPT_CUT_LAYERS:
            
            config.cutLayers = (u16)value;
            return;
            
        case OPT_CUT_OPACITY:
            
            config.cutOpacity = (u8)value;
            return;

        default:
            fatalError;
    }
}

void
DmaDebugger::setConfigItem(Option option, long id, i64 value)
{
    if (!MemAccessEnum::isValid(id)) return;
    
    MemAccess access = (MemAccess)id;
    
    switch (option) {
            
        case OPT_DMA_DEBUG_ENABLE:
            
            config.dmaChannel[access] = value;
            return;
            
        case OPT_DMA_DEBUG_COLOR:
            
            setDmaDebugColor(access, GpuColor((u32)value));
            return;
            
        default:
            return;
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
    
    config.dmaColor[type] = color.abgr;
        
    debugColor[type][0] = color.shade(0.3).abgr;
    debugColor[type][1] = color.shade(0.1).abgr;
    debugColor[type][2] = color.tint(0.1).abgr;
    debugColor[type][3] = color.tint(0.3).abgr;
}

void
DmaDebugger::setDmaDebugColor(MemAccess type, RgbColor color)
{
    setDmaDebugColor(type, GpuColor(color));
}

void
DmaDebugger::visualizeDma(isize offset, u8 data, MemAccess type)
{
    visualizeDma((u32 *)vic.dmaTexturePtr + offset, data, type);
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
DmaDebugger::computeOverlay(u32 *emuTexture, u32 *dmaTexture)
{
    double weight = config.dmaOpacity / 255.0;
    
    switch (config.dmaDisplayMode) {

        case DMA_DISPLAY_MODE_FG_LAYER:
                        
            for (isize y = 0; y < TEX_HEIGHT; y++) {
                
                u32 *emu = emuTexture + (y * TEX_WIDTH);
                u32 *dma = dmaTexture + (y * TEX_WIDTH);
                
                for (isize x = 0; x < TEX_WIDTH; x++) {
                    
                    if ((dma[x] & 0xFFFFFF) == 0) continue;
                    
                    GpuColor emuColor = emu[x];
                    GpuColor dmaColor = dma[x];
                    GpuColor mixColor = emuColor.mix(dmaColor, weight);
                    emu[x] = mixColor.abgr;
                }
            }
            break;

        case DMA_DISPLAY_MODE_BG_LAYER:
            
            for (isize y = 0; y < TEX_HEIGHT; y++) {
                
                u32 *emu = emuTexture + (y * TEX_WIDTH);
                u32 *dma = dmaTexture + (y * TEX_WIDTH);
                
                for (isize x = 0; x < TEX_WIDTH; x++) {
                    
                    if ((dma[x] & 0xFFFFFF) != 0) {
                        emu[x] = dma[x];
                    } else {
                        GpuColor emuColor = emu[x];
                        GpuColor mixColor = emuColor.shade(weight);
                        emu[x] = mixColor.abgr;
                    }
                }
            }
            break;

        case DMA_DISPLAY_MODE_ODD_EVEN_LAYERS:
            
            for (isize y = 0; y < TEX_HEIGHT; y++) {
                          
                u32 *emu = emuTexture + (y * TEX_WIDTH);
                u32 *dma = dmaTexture + (y * TEX_WIDTH);

                for (isize x = 0; x < TEX_WIDTH; x++) {
                    
                    GpuColor emuColor = emu[x];
                    GpuColor dmaColor = dma[x];
                    GpuColor mixColor = dmaColor.mix(emuColor, weight);
                    emu[x] = mixColor.abgr;
                }
            }
            break;
            
        default:
            fatalError;
    }
}

void
DmaDebugger::cutLayers()
{
    // Check master switch
    if (!(config.cutLayers & 0x1000)) return;
    
    // Only proceed if at least one channel is enabled
    if (!(config.cutLayers & 0x0F00)) return;
    
    u32 *emuTexturePtr = vic.emuTexturePtr;
    u8 *zBuffer = vic.zBuffer;
    
    for (isize i = 0; i < TEX_WIDTH; i++) {
        
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
            
            u8 r = emuTexturePtr[i] & 0xFF;
            u8 g = (emuTexturePtr[i] >> 8) & 0xFF;
            u8 b = (emuTexturePtr[i] >> 16) & 0xFF;

            double scale = config.cutOpacity / 255.0;
            u8 bg = (vic.scanline() / 4) % 2 == (i / 4) % 2 ? 0x22 : 0x44;
            u8 newr = (u8)(r * (1 - scale) + bg * scale);
            u8 newg = (u8)(g * (1 - scale) + bg * scale);
            u8 newb = (u8)(b * (1 - scale) + bg * scale);
            
            emuTexturePtr[i] = 0xFF000000 | newb << 16 | newg << 8 | newr;
        }
    }
}

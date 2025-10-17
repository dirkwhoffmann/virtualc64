// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension Inspector {
    
    func refreshBus(count: Int = 0, full: Bool = false) {
          
        if let dma = emu?.dmaDebugger.getConfig() {
            
            let enabled = dma.dmaDebug

            // DMA debugger
            busDebug.state = enabled ? .on : .off
            busChannelR.state = dma.dmaChannel.0 ? .on : .off
            busChannelI.state = dma.dmaChannel.1 ? .on : .off
            busChannelC.state = dma.dmaChannel.2 ? .on : .off
            busChannelG.state = dma.dmaChannel.3 ? .on : .off
            busChannelP.state = dma.dmaChannel.4 ? .on : .off
            busChannelS.state = dma.dmaChannel.5 ? .on : .off
            busColorR.setColor(dma.dmaColor.0)
            busColorI.setColor(dma.dmaColor.1)
            busColorC.setColor(dma.dmaColor.2)
            busColorG.setColor(dma.dmaColor.3)
            busColorP.setColor(dma.dmaColor.4)
            busColorS.setColor(dma.dmaColor.5)
            
            // 2D Overlay
            busOverlay.state = dma.dmaOverlay ? .on : .off
            busOverlay.isEnabled = enabled
            busOpacity.integerValue = Int(dma.dmaOpacity)
            busOpacity.isEnabled = enabled
            busDisplayMode.selectItem(withTag: dma.dmaDisplayMode.rawValue)
            busDisplayMode.isEnabled = enabled
        }
        
        busDmaView.update()
    }
    
    //
    // Action methods
    //
    
    @IBAction func busDebugAction(_ sender: NSButton!) {

        emu?.set(.DMA_DEBUG_ENABLE, enable: sender.state == .on)
    }

    @IBAction func busColorAction(_ sender: NSColorWell!) {
        
        let r = Int((sender.color.redComponent * 255.0).rounded())
        let g = Int((sender.color.greenComponent * 255.0).rounded())
        let b = Int((sender.color.blueComponent * 255.0).rounded())
        let bgr = (b << 16) | (g << 8) | (r << 0)

        switch sender.tag {
        case 0:  emu?.set(.DMA_DEBUG_COLOR0, value: bgr)
        case 1:  emu?.set(.DMA_DEBUG_COLOR1, value: bgr)
        case 2:  emu?.set(.DMA_DEBUG_COLOR2, value: bgr)
        case 3:  emu?.set(.DMA_DEBUG_COLOR3, value: bgr)
        case 4:  emu?.set(.DMA_DEBUG_COLOR4, value: bgr)
        case 5:  emu?.set(.DMA_DEBUG_COLOR5, value: bgr)
        default: break
        }
    }

    @IBAction func busChannelAction(_ sender: NSButton!) {

        switch sender.tag {
        case 0:  emu?.set(.DMA_DEBUG_CHANNEL0, enable: sender.state == .on)
        case 1:  emu?.set(.DMA_DEBUG_CHANNEL1, enable: sender.state == .on)
        case 2:  emu?.set(.DMA_DEBUG_CHANNEL2, enable: sender.state == .on)
        case 3:  emu?.set(.DMA_DEBUG_CHANNEL3, enable: sender.state == .on)
        case 4:  emu?.set(.DMA_DEBUG_CHANNEL4, enable: sender.state == .on)
        case 5:  emu?.set(.DMA_DEBUG_CHANNEL5, enable: sender.state == .on)
        default: break
        }
    }
    
    @IBAction func busOverlayAction(_ sender: NSButton!) {

        emu?.set(.DMA_DEBUG_OVERLAY, enable: sender.state == .on)
    }

    @IBAction func busDisplayModeAction(_ sender: NSPopUpButton!) {
        
        emu?.set(.DMA_DEBUG_MODE, value: sender.selectedTag())
    }
    
    @IBAction func busOpacityAction(_ sender: NSSlider!) {
        
        emu?.set(.DMA_DEBUG_OPACITY, value: sender.integerValue)
    }
}

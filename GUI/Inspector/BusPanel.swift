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
            
            let enabled = dma.mode == XRayMode.XRAY_DMA

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

        emu?.set(.XRAY_MODE, value: (sender.state == .on ? XRayMode.XRAY_DMA : XRayMode.XRAY_NONE).rawValue)
    }

    @IBAction func busColorAction(_ sender: NSColorWell!) {
        
        let r = Int((sender.color.redComponent * 255.0).rounded())
        let g = Int((sender.color.greenComponent * 255.0).rounded())
        let b = Int((sender.color.blueComponent * 255.0).rounded())
        let bgr = (b << 16) | (g << 8) | (r << 0)

        switch sender.tag {
        case 0:  emu?.set(.XRAY_COLOR0, value: bgr)
        case 1:  emu?.set(.XRAY_COLOR1, value: bgr)
        case 2:  emu?.set(.XRAY_COLOR2, value: bgr)
        case 3:  emu?.set(.XRAY_COLOR3, value: bgr)
        case 4:  emu?.set(.XRAY_COLOR4, value: bgr)
        case 5:  emu?.set(.XRAY_COLOR5, value: bgr)
        default: break
        }
    }

    @IBAction func busChannelAction(_ sender: NSButton!) {

        switch sender.tag {
        case 0:  emu?.set(.XRAY_DMA_CHANNEL0, enable: sender.state == .on)
        case 1:  emu?.set(.XRAY_DMA_CHANNEL1, enable: sender.state == .on)
        case 2:  emu?.set(.XRAY_DMA_CHANNEL2, enable: sender.state == .on)
        case 3:  emu?.set(.XRAY_DMA_CHANNEL3, enable: sender.state == .on)
        case 4:  emu?.set(.XRAY_DMA_CHANNEL4, enable: sender.state == .on)
        case 5:  emu?.set(.XRAY_DMA_CHANNEL5, enable: sender.state == .on)
        default: break
        }
    }
    
    @IBAction func busOverlayAction(_ sender: NSButton!) {

        emu?.set(.XRAY_OVERLAY, enable: sender.state == .on)
    }

    @IBAction func busDisplayModeAction(_ sender: NSPopUpButton!) {
        
        emu?.set(.XRAY_OVERLAY_STYLE, value: sender.selectedTag())
    }
    
    @IBAction func busOpacityAction(_ sender: NSSlider!) {
        
        emu?.set(.XRAY_OVERLAY_OPACITY, value: sender.integerValue)
    }
}

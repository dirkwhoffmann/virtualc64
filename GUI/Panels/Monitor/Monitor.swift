// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class Monitor: DialogController {
    
    var config: Configuration { return parent.config }
    
    // Bus debugger
    @IBOutlet weak var busDebug: NSButton!
    @IBOutlet weak var busChannelR: NSButton!
    @IBOutlet weak var busChannelI: NSButton!
    @IBOutlet weak var busChannelC: NSButton!
    @IBOutlet weak var busChannelG: NSButton!
    @IBOutlet weak var busChannelP: NSButton!
    @IBOutlet weak var busChannelS: NSButton!
    
    @IBOutlet weak var busColorR: NSColorWell!
    @IBOutlet weak var busColorI: NSColorWell!
    @IBOutlet weak var busColorC: NSColorWell!
    @IBOutlet weak var busColorG: NSColorWell!
    @IBOutlet weak var busColorP: NSColorWell!
    @IBOutlet weak var busColorS: NSColorWell!

    @IBOutlet weak var busOpacity: NSSlider!
    @IBOutlet weak var busDisplayMode: NSPopUpButton!
        
    // Stencils
    @IBOutlet weak var cutEnable: NSButton!
    @IBOutlet weak var cutBorder: NSButton!
    @IBOutlet weak var cutForeground: NSButton!
    @IBOutlet weak var cutBackground: NSButton!
    @IBOutlet weak var cutSprites: NSButton!
    @IBOutlet weak var cutSprite0: NSButton!
    @IBOutlet weak var cutSprite1: NSButton!
    @IBOutlet weak var cutSprite2: NSButton!
    @IBOutlet weak var cutSprite3: NSButton!
    @IBOutlet weak var cutSprite4: NSButton!
    @IBOutlet weak var cutSprite5: NSButton!
    @IBOutlet weak var cutSprite6: NSButton!
    @IBOutlet weak var cutSprite7: NSButton!
    @IBOutlet weak var cutOpacity: NSSlider!
    
    // Sprites
    @IBOutlet weak var hideSprites: NSButton!

    override func awakeFromNib() {
        
        super.awakeFromNib()
        refresh()
    }
    
    override func showWindow(_ sender: Any?) {

        super.showWindow(self)

        print("Monitor: showWindow")

        // Update colors
        if let dma = emu?.dmaDebugger.getConfig() {

            busColorR.setColor(dma.dmaColor.0)
            busColorI.setColor(dma.dmaColor.1)
            busColorC.setColor(dma.dmaColor.2)
            busColorG.setColor(dma.dmaColor.3)
            busColorP.setColor(dma.dmaColor.4)
            busColorS.setColor(dma.dmaColor.5)
        }
    }

    func refresh() {
                
        if let dma = emu?.dmaDebugger.getConfig() {

            // DMA debugger
            busDebug.state = dma.dmaDebug ? .on : .off
            busChannelR.state = dma.dmaChannel.0 ? .on : .off
            busChannelI.state = dma.dmaChannel.1 ? .on : .off
            busChannelC.state = dma.dmaChannel.2 ? .on : .off
            busChannelG.state = dma.dmaChannel.3 ? .on : .off
            busChannelP.state = dma.dmaChannel.4 ? .on : .off
            busChannelS.state = dma.dmaChannel.5 ? .on : .off
            busOpacity.integerValue = Int(dma.dmaOpacity)
            busDisplayMode.selectItem(withTag: dma.dmaDisplayMode.rawValue)

            // Layer debugger
            let layers = config.vicCutLayers
            cutEnable.state = (layers & 0x1000) != 0 ? .on : .off
            cutBackground.state = (layers & 0x800) != 0 ? .on : .off
            cutForeground.state = (layers & 0x400) != 0 ? .on : .off
            cutBackground.state = (layers & 0x200) != 0 ? .on : .off
            cutSprites.state = (layers & 0x100) != 0 ? .on : .off
            cutSprite7.state = (layers & 0x80) != 0 ? .on : .off
            cutSprite6.state = (layers & 0x40) != 0 ? .on : .off
            cutSprite5.state = (layers & 0x20) != 0 ? .on : .off
            cutSprite4.state = (layers & 0x10) != 0 ? .on : .off
            cutSprite3.state = (layers & 0x08) != 0 ? .on : .off
            cutSprite2.state = (layers & 0x04) != 0 ? .on : .off
            cutSprite1.state = (layers & 0x02) != 0 ? .on : .off
            cutSprite0.state = (layers & 0x01) != 0 ? .on : .off
            cutOpacity.integerValue = config.vicCutOpacity

            cutSprite7.isEnabled = (layers & 0x100) != 0
            cutSprite6.isEnabled = (layers & 0x100) != 0
            cutSprite5.isEnabled = (layers & 0x100) != 0
            cutSprite4.isEnabled = (layers & 0x100) != 0
            cutSprite3.isEnabled = (layers & 0x100) != 0
            cutSprite2.isEnabled = (layers & 0x100) != 0
            cutSprite1.isEnabled = (layers & 0x100) != 0
            cutSprite0.isEnabled = (layers & 0x100) != 0

            // Sprites
            hideSprites.state = config.vicHideSprites ? .on : .off
        }
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

    @IBAction func busDisplayModeAction(_ sender: NSPopUpButton!) {
        
        emu?.set(.DMA_DEBUG_MODE, value: sender.selectedTag())
    }
    
    @IBAction func busOpacityAction(_ sender: NSSlider!) {
        
        emu?.set(.DMA_DEBUG_OPACITY, value: sender.integerValue)
    }
    
    @IBAction func hideSpritesAction(_ sender: NSButton!) {
        
        config.vicHideSprites = sender.state == .on
    }
    
    @IBAction func cutOpacityAction(_ sender: NSSlider!) {
        
        config.vicCutOpacity = sender.integerValue
    }
    
    func addLayer(_ mask: Int) {
        config.vicCutLayers = config.vicCutLayers | mask
    }
    func removeLayer(_ mask: Int) {
        config.vicCutLayers = config.vicCutLayers & ~mask
    }

    @IBAction func cutEnableAction(_ sender: NSButton!) {

        if sender.state == .on { addLayer(0x1000) } else { removeLayer(0x1000) }
    }
    
    @IBAction func cutBorderAction(_ sender: NSButton!) {
    
        if sender.state == .on { addLayer(0x800) } else { removeLayer(0x800) }
    }

    @IBAction func cutForegroundAction(_ sender: NSButton!) {
    
        if sender.state == .on { addLayer(0x400) } else { removeLayer(0x400) }
    }

    @IBAction func cutBackgroundAction(_ sender: NSButton!) {
    
        if sender.state == .on { addLayer(0x200) } else { removeLayer(0x200) }
    }

    @IBAction func cutSpritesAction(_ sender: NSButton!) {
    
        if sender.state == .on { addLayer(0x100) } else { removeLayer(0x100) }
    }

    @IBAction func cutSingleSpriteAction(_ sender: NSButton!) {
    
        let sprite = sender.tag
        if sender.state == .on { addLayer(1 << sprite) } else { removeLayer(1 << sprite) }
    }
}

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
    @IBOutlet weak var ssCollision: NSButton!
    @IBOutlet weak var sbCollision: NSButton!

    override func awakeFromNib() {
        
        track()
        super.awakeFromNib()
        refresh()
    }
    
    func refresh() {
                
        let cnf = c64.vic.getConfig()
        
        // Bus debugger
        /*
        let vicinfo = c64.vic.getInfo()
        vicinfo.
        let bus = c64.getConfig(.DMA_DEBUG_ENABLE) != 0
        let opacity = c64.getConfig(.DMA_DEBUG_OPACITY)
        let mode = c64.getConfig(.DMA_DEBUG_MODE)
        */
                
        busDebug.state = cnf.dmaDebug ? .on : .off
        busChannelR.state = cnf.dmaChannel.0 ? .on : .off
        busChannelI.state = cnf.dmaChannel.1 ? .on : .off
        busChannelC.state = cnf.dmaChannel.2 ? .on : .off
        busChannelG.state = cnf.dmaChannel.3 ? .on : .off
        busChannelP.state = cnf.dmaChannel.4 ? .on : .off
        busChannelS.state = cnf.dmaChannel.5 ? .on : .off
        
        // Colors
        busColorR.setColor(cnf.dmaColor.0)
        busColorI.setColor(cnf.dmaColor.1)
        busColorC.setColor(cnf.dmaColor.2)
        busColorG.setColor(cnf.dmaColor.3)
        busColorP.setColor(cnf.dmaColor.4)
        busColorS.setColor(cnf.dmaColor.5)

        busOpacity.integerValue = Int(cnf.dmaOpacity)
        busDisplayMode.selectItem(withTag: cnf.dmaDisplayMode.rawValue)
        
        // Layer debugger
        let layers = config.vicCutLayers
        cutForeground.state = (layers & 0x800) != 0 ? .on : .off
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
        ssCollision.state = config.vicSSCollisions ? .on : .off
        sbCollision.state = config.vicSBCollisions ? .on : .off
    }
        
    //
    // Action methods
    //
    
    @IBAction func busDebugAction(_ sender: NSButton!) {

        track()
        
        c64.configure(.DMA_DEBUG_ENABLE, enable: sender.state == .on)
        refresh()
    }

    @IBAction func busColorAction(_ sender: NSColorWell!) {
        
        let r = Int(sender.color.redComponent * 255)
        let g = Int(sender.color.greenComponent * 255)
        let b = Int(sender.color.blueComponent * 255)
        let bgr = (b << 16) | (g << 8) | (r << 0)
        
        c64.configure(.DMA_DEBUG_COLOR, id: sender.tag, value: bgr)
        refresh()
    }

    @IBAction func busChannelAction(_ sender: NSButton!) {
        
        track()
        c64.configure(.DMA_DEBUG_ENABLE, id: sender.tag, enable: sender.state == .on)
        refresh()
    }

    @IBAction func busDisplayModeAction(_ sender: NSPopUpButton!) {
        
        track()
        c64.configure(.DMA_DEBUG_MODE, value: sender.selectedTag())
        refresh()
    }
    
    @IBAction func busOpacityAction(_ sender: NSSlider!) {
        
        track()
        c64.configure(.DMA_DEBUG_OPACITY, value: sender.integerValue)
        refresh()
    }
    
    @IBAction func hideSpritesAction(_ sender: NSButton!) {
        
        track()
        config.vicHideSprites = sender.state == .on
        refresh()
    }

    @IBAction func ssCollisionsAction(_ sender: NSButton!) {
        
        track()
        config.vicSSCollisions = sender.state == .on
        refresh()
    }

    @IBAction func sbCollisionsAction(_ sender: NSButton!) {
        
        track()
        config.vicSBCollisions = sender.state == .on
        refresh()
    }
    
    @IBAction func cutOpacityAction(_ sender: NSSlider!) {
        
        config.vicCutOpacity = sender.integerValue
        refresh()
    }
    
    func addLayer(_ mask: Int) {
        config.vicCutLayers = config.vicCutLayers | mask
    }
    func removeLayer(_ mask: Int) {
        config.vicCutLayers = config.vicCutLayers & ~mask
    }

    @IBAction func cutBorderAction(_ sender: NSButton!) {
    
        track()
        sender.state == .on ? addLayer(0x800) : removeLayer(0x800)
        refresh()
    }

    @IBAction func cutForegroundAction(_ sender: NSButton!) {
    
        track()
        sender.state == .on ? addLayer(0x400) : removeLayer(0x400)
        refresh()
    }

    @IBAction func cutBackgroundAction(_ sender: NSButton!) {
    
        track()
        sender.state == .on ? addLayer(0x200) : removeLayer(0x200)
        refresh()
    }

    @IBAction func cutSpritesAction(_ sender: NSButton!) {
    
        track()
        sender.state == .on ? addLayer(0x100) : removeLayer(0x100)
        refresh()
    }

    @IBAction func cutSingleSpriteAction(_ sender: NSButton!) {
    
        let sprite = sender.tag
        track()
        sender.state == .on ? addLayer(1 << sprite) : removeLayer(1 << sprite)
        refresh()
    }
}

extension Monitor: NSWindowDelegate {

    func windowWillClose(_ notification: Notification) {

        track("Closing monitor")
    }
}

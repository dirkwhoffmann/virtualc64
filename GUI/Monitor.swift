// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class Monitor: DialogController {
    
    var config: Configuration { return parent.config }
    
    // Debugging
    @IBOutlet weak var zoomTexture: NSButton!
    @IBOutlet weak var markDmaRasterlines: NSButton!
    @IBOutlet weak var markIrqRasterlines: NSButton!
    @IBOutlet weak var hideSprites: NSButton!

    // Cheating
    @IBOutlet weak var ssCollision: NSButton!
    @IBOutlet weak var sbCollision: NSButton!
        
    // Stencils
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
    
    override func awakeFromNib() {
        
        track()
        super.awakeFromNib()
        refresh()
    }
    
    func refresh() {
        
        zoomTexture.state = parent.renderer.zoom ? .on : .off
        markDmaRasterlines.state = config.vicMarkDmaLines ? .on : .off
        markIrqRasterlines.state = config.vicMarkIrqLines ? .on : .off
        hideSprites.state = config.vicHideSprites ? .on : .off

        ssCollision.state = config.vicSSCollisions ? .on : .off
        sbCollision.state = config.vicSBCollisions ? .on : .off
        
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
    }
        
    //
    // Action methods
    //
    
    @IBAction func zoomTextureAction(_ sender: NSButton!) {

        track()
        
        if sender.state == .on {
            parent.renderer.zoomTextureOut()
        } else {
            parent.renderer.zoomTextureIn()
        }
        refresh()
    }

    @IBAction func markDmaLinesAction(_ sender: NSButton!) {
        
        track()
        config.vicMarkDmaLines = sender.state == .on
        refresh()
    }

    @IBAction func markIrqLinesAction(_ sender: NSButton!) {
        
        track()
        config.vicMarkIrqLines = sender.state == .on
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

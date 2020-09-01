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
    
    var zoom = false
    
    override func awakeFromNib() {
        
        track()
        super.awakeFromNib()
        refresh()
    }
    
    func refresh() {
        
        zoomTexture.state = zoom ? .on : .off
        markDmaRasterlines.state = config.vicMarkDmaLines ? .on : .off
        markIrqRasterlines.state = config.vicMarkIrqLines ? .on : .off
        hideSprites.state = config.vicHideSprites ? .on : .off

        ssCollision.state = config.vicSSCollisions ? .on : .off
        sbCollision.state = config.vicSBCollisions ? .on : .off
    }
        
    //
    // Action methods
    //
    
    @IBAction func zoomTextureAction(_ sender: NSButton!) {

        track()
        zoom = sender.state == .on
        
        zoom ? parent.renderer.zoomTextureOut() : parent.renderer.zoomTextureIn()
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
}

extension Monitor: NSWindowDelegate {

    func windowWillClose(_ notification: Notification) {

        track("Closing monitor")
    }
}

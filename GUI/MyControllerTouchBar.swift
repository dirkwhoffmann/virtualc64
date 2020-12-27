// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class PaddedButton: NSButton {
    
    @IBInspectable var vPad: CGFloat = 4.5
    @IBInspectable var hPad: CGFloat = 0
    
    override func draw(_ dirtyRect: NSRect) {
        
        let origBounds = self.bounds
        defer { self.bounds = origBounds }
        
        self.bounds = origBounds.insetBy(dx: hPad, dy: vPad)
        super.draw(dirtyRect)
    }
}

@available(OSX 10.12.2, *)
extension NSTouchBarItem.Identifier {
    
    static let commodore = NSTouchBarItem.Identifier("com.vc64.TouchBarItem.commodore")
    static let runstop   = NSTouchBarItem.Identifier("com.vc64.TouchBarItem.runstop")
    static let home      = NSTouchBarItem.Identifier("com.vc64.TouchBarItem.home")
    static let del       = NSTouchBarItem.Identifier("com.vc64.TouchBarItem.del")
    static let restore   = NSTouchBarItem.Identifier("com.vc64.TouchBarItem.restore")
    
    static let save      = NSTouchBarItem.Identifier("com.vc64.TouchBarItem.save")
    static let load      = NSTouchBarItem.Identifier("com.vc64.TouchBarItem.load")
    static let browse    = NSTouchBarItem.Identifier("com.vc64.TouchBarItem.browse")
}

extension MyController {
    
    @objc func touchBarHomeKeyAction() {
        if modifierFlags.contains(NSEvent.ModifierFlags.shift) {
            clearKeyAction(self)
        } else {
            homeKeyAction(self)
        }
    }
    
    @objc func touchBarDelKeyAction() {
        
        if modifierFlags.contains(NSEvent.ModifierFlags.shift) {
            insertKeyAction(self)
        } else {
            deleteKeyAction(self)
        }
    }
}

@available(OSX 10.12.2, *)
extension MyController: NSTouchBarDelegate {
    
    override open func makeTouchBar() -> NSTouchBar? {
        
        track()
        
        let touchBar = NSTouchBar()
        touchBar.delegate = self
        
        // Configure items
        touchBar.defaultItemIdentifiers = [
            .commodore, .runstop, .home, .del,
            .save, .load, .browse
        ]
        
        // Make touchbar customizable
        touchBar.customizationIdentifier = NSTouchBar.CustomizationIdentifier("com.virtualc64.touchbar")
        touchBar.customizationAllowedItemIdentifiers = [
            .commodore, .runstop, .home, .del, .restore,
            .save, .load, .browse
        ]
        
        return touchBar
    }
    
    public func touchBar(_ touchBar: NSTouchBar, makeItemForIdentifier identifier: NSTouchBarItem.Identifier) -> NSTouchBarItem? {
        
        switch identifier {
            
        case NSTouchBarItem.Identifier.commodore:
            let item = NSCustomTouchBarItem(identifier: identifier)
            let icon = NSImage(named: NSImage.Name("commodoreTemplate"))!
            item.customizationLabel = "Commodore key"
            item.view = NSButton(image: icon,
                                 target: self,
                                 action: #selector(commodoreKeyAction))
            return item
            
        case NSTouchBarItem.Identifier.runstop:
            let item = NSCustomTouchBarItem(identifier: identifier)
            let icon = NSImage(named: NSImage.Name("runStopTemplate"))!
            item.customizationLabel = "Runstop key"
            item.view = NSButton(image: icon,
                                 target: self,
                                 action: #selector(runstopAction))
            
            return item
            
        case NSTouchBarItem.Identifier.home:
            let item = NSCustomTouchBarItem(identifier: identifier)
            let icon = NSImage(named: NSImage.Name("ClrHomeTemplate"))!
            item.customizationLabel = "Home and Clear key"
            item.view = NSButton(image: icon,
                                 target: self,
                                 action: #selector(touchBarHomeKeyAction))
            return item
            
        case NSTouchBarItem.Identifier.del:
            let item = NSCustomTouchBarItem(identifier: identifier)
            let icon = NSImage(named: NSImage.Name("instDelTemplate"))!
            item.customizationLabel = "Delete and Insert key"
            item.view = NSButton(image: icon,
                                 target: self,
                                 action: #selector(touchBarDelKeyAction))
            return item
            
        case NSTouchBarItem.Identifier.restore:
            let item = NSCustomTouchBarItem(identifier: identifier)
            let icon = NSImage(named: NSImage.Name("restoreTemplate"))!
            item.customizationLabel = "Restore key"
            item.view = PaddedButton(image: icon,
                                     target: self,
                                     action: #selector(restoreAction))
            return item
            
        case NSTouchBarItem.Identifier.save:
            let item = NSCustomTouchBarItem(identifier: identifier)
            let icon = NSImage(named: NSImage.Name("pushTemplate"))!
            item.customizationLabel = "Save"
            item.view = PaddedButton(image: icon,
                                     target: self,
                                     action: #selector(takeSnapshotAction(_:)))
            return item
            
        case NSTouchBarItem.Identifier.load:
            let item = NSCustomTouchBarItem(identifier: identifier)
            let icon = NSImage(named: NSImage.Name("popTemplate"))!
            item.customizationLabel = "Load"
            item.view = PaddedButton(image: icon,
                                     target: self,
                                     action: #selector(restoreSnapshotAction(_:)))
            return item
            
        case NSTouchBarItem.Identifier.browse:
            let item = NSCustomTouchBarItem(identifier: identifier)
            let icon = NSImage(named: NSImage.Name("timeMachineTemplate"))!
            item.customizationLabel = "Browse"
            item.view = PaddedButton(image: icon,
                                     target: self,
                                     action: #selector(browseSnapshotsAction(_:)))
            return item
            
        default:
            return nil
        }
    }
}

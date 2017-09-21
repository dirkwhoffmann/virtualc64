//
//  MyControllerTouchBar.swift
//  V64
//
//  Created by Dirk Hoffmann on 06.06.17.
//
//

import Foundation

@available(OSX 10.12.2, *)
extension NSTouchBarItem.Identifier {
    
    static let commodore  = NSTouchBarItem.Identifier("com.virtualc64.TouchBarItem.commodore")
    static let runstop    = NSTouchBarItem.Identifier("com.virtualc64.TouchBarItem.runstop")
    static let home       = NSTouchBarItem.Identifier("com.virtualc64.TouchBarItem.home")
    static let del        = NSTouchBarItem.Identifier("com.virtualc64.TouchBarItem.del")
    static let restore    = NSTouchBarItem.Identifier("com.virtualc64.TouchBarItem.restore")
    static let ttpopover  = NSTouchBarItem.Identifier("com.virtualc64.TouchBarItem.ttpopover")
}

public extension MyController
{
    @available(OSX 10.12.2, *)
    @objc func rebuildTouchBar() {
        
        let popover = touchBar?.item(forIdentifier:.ttpopover) as? NSPopoverTouchBarItem
        let bar = popover?.popoverTouchBar as? TimeTravelTouchBar
        bar?.rebuild()
    }
    
}

@available(OSX 10.12.2, *)
extension MyController : NSTouchBarDelegate
 {
    @objc func TouchBarHomeKeyAction() {
        if (modifierFlags.contains(NSEvent.ModifierFlags.shift)) {
            clearKeyAction(self)
        } else {
            homeKeyAction(self)
        }
    }

    @objc func TouchBarDelKeyAction() {
        if (modifierFlags.contains(NSEvent.ModifierFlags.shift)) {
            insertKeyAction(self)
        } else {
            deleteKeyAction(self)
        }
    }
    
    // NSTouchBarDelegate

    override open func makeTouchBar() -> NSTouchBar? {
 
        if (c64 == nil) {
            return nil // Only create TouchBar for controllers having a C64 proxy
        }

        NSLog("\(#function)")

        let touchBar = NSTouchBar()
        touchBar.delegate = self

        // Configure items
        touchBar.defaultItemIdentifiers = [
            .commodore,
            .runstop,
            .home,
            .del,
            .restore,
            .ttscrubber,
            .ttpopover
        ]
        
        // Make touchbar customizable
        touchBar.customizationIdentifier = NSTouchBar.CustomizationIdentifier("com.virtualc64.touchbar")
        touchBar.customizationAllowedItemIdentifiers = [
            .commodore,
            .runstop,
            .home,
            .del,
            .restore,
            .ttpopover]
        
            return touchBar
    }
    
    public func touchBar(_ touchBar: NSTouchBar, makeItemForIdentifier identifier: NSTouchBarItem.Identifier) -> NSTouchBarItem? {
        
        switch identifier {
            
        case NSTouchBarItem.Identifier.commodore:
            let item = NSCustomTouchBarItem(identifier: identifier)
            item.customizationLabel = "Commodore key"
            item.view = NSButton(image: NSImage(named: NSImage.Name(rawValue: "commodore"))!,
                                 target: self,
                                 action: #selector(commodoreKeyAction))
            return item

        case NSTouchBarItem.Identifier.runstop:
            let item = NSCustomTouchBarItem(identifier: identifier)
            item.customizationLabel = "Runstop key"
            item.view = NSButton(image:  NSImage(named: NSImage.Name(rawValue: "runstop"))!,
                                 target: self,
                                 action: #selector(runstopAction))
            
            return item

        case NSTouchBarItem.Identifier.home:
            let item = NSCustomTouchBarItem(identifier: identifier)
            item.customizationLabel = "Home and Clear key"
            item.view = NSButton(image:  NSImage(named: NSImage.Name(rawValue: "home"))!,
                                 target: self,
                                 action: #selector(TouchBarHomeKeyAction))
            return item

        case NSTouchBarItem.Identifier.del:
            let item = NSCustomTouchBarItem(identifier: identifier)
            item.customizationLabel = "Delete and Insert key"
            item.view = NSButton(image:  NSImage(named: NSImage.Name(rawValue: "del"))!,
                                 target: self,
                                 action: #selector(TouchBarDelKeyAction))
            return item

        case NSTouchBarItem.Identifier.restore:
            let item = NSCustomTouchBarItem(identifier: identifier)
            item.customizationLabel = "Restore key"
            item.view = NSButton(image:  NSImage(named: NSImage.Name(rawValue: "restore"))!,
                                 target: self,
                                 action: #selector(restoreAction))
            return item

/*
        case NSTouchBarItemIdentifier.larrow:
            let item = NSCustomTouchBarItem(identifier: identifier)
            item.customizationLabel = "Left arrow key"
            item.view = NSButton(title: "←",
                                 target: self,
                                 action: #selector(homeKeyAction))
            return item

        case NSTouchBarItemIdentifier.uarrow:
            let item = NSCustomTouchBarItem(identifier: identifier)
            item.customizationLabel = "Up arrow key"
            item.view = NSButton(title: "↑",
                                 target: self,
                                 action: #selector(homeKeyAction))
            return item
*/
            
        case NSTouchBarItem.Identifier.ttpopover:
            let item = NSPopoverTouchBarItem(identifier: identifier)
            let icon = NSImage(named: NSImage.Name(rawValue: "tb_timetravel2.png"))
            let resizedIcon = icon?.resizeImage(width: 24, height: 24)
            item.customizationLabel = "Time travel"
            item.collapsedRepresentationImage = resizedIcon
            item.popoverTouchBar = TimeTravelTouchBar (parent: item, controller:self)
            return item
            
        default: return nil
        }
    }
}


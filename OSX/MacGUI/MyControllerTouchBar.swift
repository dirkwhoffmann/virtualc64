//
//  MyControllerTouchBar.swift
//  V64
//
//  Created by Dirk Hoffmann on 06.06.17.
//
//

import Foundation

extension NSTouchBarItemIdentifier {
    
    static let commodore  = NSTouchBarItemIdentifier("com.virtualc64.TouchBarItem.commodore")
    static let runstop    = NSTouchBarItemIdentifier("com.virtualc64.TouchBarItem.runstop")
    static let home       = NSTouchBarItemIdentifier("com.virtualc64.TouchBarItem.home")
    static let del        = NSTouchBarItemIdentifier("com.virtualc64.TouchBarItem.del")
    static let restore    = NSTouchBarItemIdentifier("com.virtualc64.TouchBarItem.restore")
    static let larrow     = NSTouchBarItemIdentifier("com.virtualc64.TouchBarItem.larrow")
    static let uarrow     = NSTouchBarItemIdentifier("com.virtualc64.TouchBarItem.uarrow")
    static let ttpopover  = NSTouchBarItemIdentifier("com.virtualc64.TouchBarItem.ttpopover")
}

public extension MyController
{
    @available(OSX 10.12.2, *)
    func rebuildTouchBar() {
        
        let popover = touchBar?.item(forIdentifier:.ttpopover) as? NSPopoverTouchBarItem
        let bar = popover?.popoverTouchBar as? TimeTravelTouchBar
        bar?.rebuild()
    }
    
}

@available(OSX 10.12.2, *)
extension MyController : NSTouchBarDelegate
 {
    func TouchBarHomeKeyAction() {
        if (modifierFlags.contains(NSShiftKeyMask)) {
            clearKeyAction(self)
        } else {
            homeKeyAction(self)
        }
    }

    func TouchBarDelKeyAction() {
        if (modifierFlags.contains(NSShiftKeyMask)) {
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

        print("\(#function)")

        let touchBar = NSTouchBar()
        touchBar.delegate = self
        touchBar.defaultItemIdentifiers = [.commodore,
                                           .runstop,
                                           .home,
                                           .del,
                                           .restore,
                                           /* .larrow, */
                                           /* .uarrow */
                                           .ttscrubber,
                                           .ttpopover
        ]
        
        return touchBar
    }
    
    public func touchBar(_ touchBar: NSTouchBar, makeItemForIdentifier identifier: NSTouchBarItemIdentifier) -> NSTouchBarItem? {
        
        switch identifier {
            
        case NSTouchBarItemIdentifier.commodore:
            let item = NSCustomTouchBarItem(identifier: identifier)
            item.view = NSButton(image: NSImage(named: "commodore")!,
                                 target: self,
                                 action: #selector(commodoreKeyAction))
            return item

        case NSTouchBarItemIdentifier.runstop:
            let item = NSCustomTouchBarItem(identifier: identifier)
            item.view = NSButton(image:  NSImage(named: "runstop")!,
                                 target: self,
                                 action: #selector(runstopAction))
            
            return item

        case NSTouchBarItemIdentifier.home:
            let item = NSCustomTouchBarItem(identifier: identifier)
            item.view = NSButton(image:  NSImage(named: "home")!,
                                 target: self,
                                 action: #selector(TouchBarHomeKeyAction))
            return item

        case NSTouchBarItemIdentifier.del:
            let item = NSCustomTouchBarItem(identifier: identifier)
            item.view = NSButton(image:  NSImage(named: "del")!,
                                 target: self,
                                 action: #selector(TouchBarDelKeyAction))
            return item

        case NSTouchBarItemIdentifier.restore:
            let item = NSCustomTouchBarItem(identifier: identifier)
            item.view = NSButton(image:  NSImage(named: "restore")!,
                                 target: self,
                                 action: #selector(restoreAction))
            return item

        case NSTouchBarItemIdentifier.larrow:
            let item = NSCustomTouchBarItem(identifier: identifier)
            item.view = NSButton(title: "←",
                                 target: self,
                                 action: #selector(homeKeyAction))
            return item

        case NSTouchBarItemIdentifier.uarrow:
            let item = NSCustomTouchBarItem(identifier: identifier)
            item.view = NSButton(title: "↑",
                                 target: self,
                                 action: #selector(homeKeyAction))
            return item
        
        case NSTouchBarItemIdentifier.ttpopover:
            let item = NSPopoverTouchBarItem(identifier: identifier)
            let icon = NSImage(named: "tb_timetravel2.png")
            let resizedIcon = icon?.resizeImage(width: 24, height: 24)
            item.collapsedRepresentationImage = resizedIcon
            item.popoverTouchBar = TimeTravelTouchBar (parent: item, controller:self)
            return item
            
        default: return nil
        }
    }
}


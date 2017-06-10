//
//  MyControllerTouchBar.swift
//  V64
//
//  Created by Dirk Hoffmann on 06.06.17.
//
//

import Foundation

extension NSTouchBarItemIdentifier {
    
    static let commodore = NSTouchBarItemIdentifier("com.virtualc64.TouchBarItem.commodore")
    static let runstop   = NSTouchBarItemIdentifier("com.virtualc64.TouchBarItem.runstop")
    static let home      = NSTouchBarItemIdentifier("com.virtualc64.TouchBarItem.home")
    static let del       = NSTouchBarItemIdentifier("com.virtualc64.TouchBarItem.del")
    static let restore   = NSTouchBarItemIdentifier("com.virtualc64.TouchBarItem.restore")
    static let larrow    = NSTouchBarItemIdentifier("com.virtualc64.TouchBarItem.larrow")
    static let uarrow    = NSTouchBarItemIdentifier("com.virtualc64.TouchBarItem.uarrow")
    static let TBIIdTimeTravel = NSTouchBarItemIdentifier("com.virtualc64.TouchBarItem.timeTravel")
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
                                           .TBIIdTimeTravel // CRASHES
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

        case NSTouchBarItemIdentifier.TBIIdTimeTravel:
            let scrubberItem = TimeTravelScrubberBarItem(identifier: identifier, controller: self)
            
            guard let scrubber = scrubberItem.view as? NSScrubber
                else { return nil }
            
            
            // Configure scrubber appearance
            scrubber.showsArrowButtons = false
            
            // Make sure that lates snapshot is always visible
            if (c64 != nil && c64.historicSnapshots() > 0) {
                scrubber.scrollItem(at: c64.historicSnapshots() - 1,
                                    to: NSScrubberAlignment.trailing)
            }

            return scrubberItem
        
            
        default: return nil
        }
    }
    
}

/*! @brief   Scrubber touch bar item for time travel feature
 *  @details The scrubber lets you pick a stored snapshot from the
 *           time travel archive. 
 */
@available(OSX 10.12.2, *)
class TimeTravelScrubberBarItem : NSCustomTouchBarItem, NSScrubberDelegate, NSScrubberDataSource, NSScrubberFlowLayoutDelegate {
    
    private static let timetravelViewId = "TimeTravelViewId"
    
    var c : MyController?
    
    required init?(coder: NSCoder) {
        super.init(coder: coder)
        c = nil
    }
    
    init(identifier: NSTouchBarItemIdentifier, controller: MyController) {
        
        c = controller
        super.init(identifier: identifier)
        
        let scrubber = NSScrubber()

        // Set view, delegate and data source
        scrubber.register(NSScrubberImageItemView.self,
                          forItemIdentifier: TimeTravelScrubberBarItem.timetravelViewId)
        scrubber.delegate = self
        scrubber.dataSource = self

        // Configure scrubber appearance
        scrubber.mode = .free
        scrubber.selectionOverlayStyle = nil
        scrubber.selectionBackgroundStyle = nil
        scrubber.showsArrowButtons = false
        
        // Scroll to latest snapshot
        let numShots = c!.c64.historicSnapshots()
        if (numShots > 0) {
            scrubber.scrollItem(at: numShots - 1,
                                to: NSScrubberAlignment.trailing)
        }

        
        // controller.timeTravelScrubber = scrubber
        view = scrubber
    }
    
    // NSScrubberDataSource
    public func numberOfItems(for scrubber: NSScrubber) -> Int {
        return c!.c64.historicSnapshots();
    }
    
    public func scrubber(_ scrubber: NSScrubber, viewForItemAt index: Int) -> NSScrubberItemView {
        let itemView = scrubber.makeItem(withIdentifier: TimeTravelScrubberBarItem.timetravelViewId, owner: self) as! NSScrubberImageItemView
        let max = c!.c64.historicSnapshots() - 1
        assert(max >= 0)
        let img = c!.c64.historicSnapshotImage(max - index)
        itemView.image = img!
        return itemView
    }
    
    // NSScrubberFlowLayoutDelegate
    func scrubber(_ scrubber: NSScrubber, layout: NSScrubberFlowLayout, sizeForItemAt itemIndex: Int) -> NSSize {
        let w = c!.c64.historicSnapshotImageWidth(itemIndex)
        let h = c!.c64.historicSnapshotImageHeight(itemIndex)
        return NSSize(width: 1+(36*w/h), height: 36)
    }

    
    // NSScrubberDelegate
    public func didBeginInteracting(with: NSScrubber) {
        // print("\(#function)")
    }
    
    public func didCancelInteracting(with: NSScrubber) {
        // print("\(#function)")
    }
    
    public func didFinishInteracting(with: NSScrubber) {
        // print("\(#function)")
    }
    
    public func scrubber(_ scrubber: NSScrubber, didSelectItemAt index: Int) {
        print("\(#function) at index \(index)")
        c!.c64.restoreHistoricSnapshot(c!.c64.historicSnapshots() - index - 1)
    }

}


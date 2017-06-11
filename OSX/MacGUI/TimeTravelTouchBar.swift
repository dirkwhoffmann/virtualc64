//
//  TimeTravelTouchBar.swift
//  V64
//
//  Created by Dirk Hoffmann on 11.06.17.
//
//

import Foundation

extension NSTouchBarItemIdentifier {
    
    static let testbutton = NSTouchBarItemIdentifier("com.virtualc64.TouchBarItem.testbutton")
    static let ttscrubber = NSTouchBarItemIdentifier("com.virtualc64.TouchBarItem.ttscrubber")
}

/*! @brief   TouchBar for TimeTravel feature
 */
@available(OSX 10.12.2, *)
class TimeTravelTouchBar: NSTouchBar {
    
    var presentingItem: NSPopoverTouchBarItem?
    var c : MyController?
    
    func dismiss(_ sender: Any?) {
        guard let popover = presentingItem else { return }
        popover.dismissPopover(sender)
    }
    
    override init() {
        
        print("\(#function)")
        
        super.init()
        
        delegate = self
        defaultItemIdentifiers = [.testbutton, .ttscrubber]
    }
    
    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    convenience init(presentingItem: NSPopoverTouchBarItem?, controller: MyController?) {

        print("\(#function)")
        self.init()
        self.presentingItem = presentingItem
        self.c = controller
    }

    func testButtonAction() {
        print("\(#function)")
        print ("testButtonAction")
    }
}

@available(OSX 10.12.2, *)
extension TimeTravelTouchBar: NSTouchBarDelegate {

    func touchBar(_ touchBar: NSTouchBar, makeItemForIdentifier identifier: NSTouchBarItemIdentifier) -> NSTouchBarItem? {
        
        print("\(#function)")
        
        switch identifier {
            
        case NSTouchBarItemIdentifier.testbutton:
            
            let item = NSCustomTouchBarItem(identifier: identifier)
            item.view = NSButton(image: NSImage(named: "runstop")!,
                                 target: self,
                                 action: #selector(testButtonAction))
            return item
            
        case NSTouchBarItemIdentifier.ttscrubber:
            let item = TimeTravelScrubberBarItem(identifier: identifier, controller: c!)
            
            guard let scrubber = item.view as? NSScrubber
                else { return nil }
            
            
            // Configure scrubber appearance
            scrubber.showsArrowButtons = false
            
            // Make sure that lates snapshot is always visible
            /*
            if (c64 != nil && c64.historicSnapshots() > 0) {
                scrubber.scrollItem(at: c64.historicSnapshots() - 1,
                                    to: NSScrubberAlignment.trailing)
            }
             */
            return item

        default:
            
            return nil
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


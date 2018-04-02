//
//  TimeTravelScrubber.swift
//  V64
//
//  Created by Dirk Hoffmann on 13.06.17.
//
//

import Foundation

/*! @brief   Scrubber touch bar item for time travel feature
 *  @details The scrubber lets you pick a stored snapshot from the
 *           time travel archive.
 */
@available(OSX 10.12.2, *)
class TimeTravelScrubber : NSCustomTouchBarItem, NSScrubberDelegate, NSScrubberDataSource, NSScrubberFlowLayoutDelegate {
    
    private static let timetravelViewId = "TimeTravelViewId"
    
    var c : MyController?
    
    required init?(coder: NSCoder) {
        super.init(coder: coder)
        c = nil
    }
    
    init(identifier: NSTouchBarItem.Identifier, controller: MyController)
    {
        c = controller
        super.init(identifier: identifier)
        
        let scrubber = NSScrubber()
        
        // Set view, delegate and data source
        scrubber.register(NSScrubberImageItemView.self,
                          forItemIdentifier: NSUserInterfaceItemIdentifier(rawValue: TimeTravelScrubber.timetravelViewId))
        scrubber.delegate = self
        scrubber.dataSource = self
        
        // Configure scrubber appearance
        scrubber.mode = .free
        scrubber.showsArrowButtons = false
        scrubber.selectionOverlayStyle = .outlineOverlay
        
        // Set scrubber width
        /*
        let viewBindings: [String: NSView] = ["scrubber": scrubber]
        let hconstraints = NSLayoutConstraint.constraints(withVisualFormat: "H:[scrubber(400)]",
                                                          options: [],
                                                          metrics: nil,
                                                          views: viewBindings)
        NSLayoutConstraint.activate(hconstraints)
        */
        
        // Assign scrubber
        view = scrubber
    }
    
    func rebuild()
    {
        let scrubber = view as? NSScrubber
        scrubber?.reloadData()
        
        // scroll to rightmost item 
        scrubber?.scrollItem(at: numberOfItems(for: scrubber!) - 1,
                             to: NSScrubber.Alignment.trailing)
    }
    
    // NSScrubberDataSource
    
    public func numberOfItems(for scrubber: NSScrubber) -> Int
    {
        // return min(c!.c64.historicSnapshots(), 10)
        return c!.c64.numAutoSnapshots()
    }
    
    public func scrubber(_ scrubber: NSScrubber, viewForItemAt index: Int) -> NSScrubberItemView {
        let itemView = scrubber.makeItem(withIdentifier: NSUserInterfaceItemIdentifier(rawValue: TimeTravelScrubber.timetravelViewId), owner: self) as! NSScrubberImageItemView
        
        assert(index < c!.c64.numAutoSnapshots())
        
        // determine corresponding snapshot index in time travel buffer
        let ttindex = numberOfItems(for: scrubber) - index - 1
        let image = c!.c64.autoSnapshotImage(ttindex)
        itemView.image = image
        return itemView
    }
    
    // NSScrubberFlowLayoutDelegate
    func scrubber(_ scrubber: NSScrubber, layout: NSScrubberFlowLayout, sizeForItemAt itemIndex: Int) -> NSSize {
        
        assert(itemIndex < c!.c64.numAutoSnapshots())
        let w = c!.c64.autoSnapshotImageWidth(itemIndex)
        let h = c!.c64.autoSnapshotImageHeight(itemIndex)
        // return NSSize(width: 40, height: (40*h/w))
        return NSSize(width: 1+(36*w/h), height: 36)
    }
    
    
    // NSScrubberDelegate
    public func didBeginInteracting(with: NSScrubber) {
    }
    
    public func didCancelInteracting(with: NSScrubber) {
    }
    
    public func didFinishInteracting(with: NSScrubber) {
    }
    
    public func scrubber(_ scrubber: NSScrubber, didSelectItemAt index: Int) {
        let ttindex = numberOfItems(for: scrubber) - index - 1
        c!.c64.restoreAutoSnapshot(ttindex)
    }
    
}

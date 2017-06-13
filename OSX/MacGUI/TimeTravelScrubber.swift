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
    
    init(identifier: NSTouchBarItemIdentifier, controller: MyController)
    {
        c = controller
        super.init(identifier: identifier)
        
        let scrubber = NSScrubber()
        
        // Set view, delegate and data source
        scrubber.register(NSScrubberImageItemView.self,
                          forItemIdentifier: TimeTravelScrubber.timetravelViewId)
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
                             to: NSScrubberAlignment.trailing)
    }
    
    // NSScrubberDataSource
    
    public func numberOfItems(for scrubber: NSScrubber) -> Int
    {
        // return min(c!.c64.historicSnapshots(), 10)
        return c!.c64.historicSnapshots()
    }
    
    public func scrubber(_ scrubber: NSScrubber, viewForItemAt index: Int) -> NSScrubberItemView {
        let itemView = scrubber.makeItem(withIdentifier: TimeTravelScrubber.timetravelViewId, owner: self) as! NSScrubberImageItemView
        
        assert(index < c!.c64.historicSnapshots())
        
        // determine corresponding snapshot index in time travel buffer
        let ttindex = numberOfItems(for: scrubber) - index - 1
        let image = c!.c64.historicSnapshotImage(ttindex)
        itemView.image = image!
        return itemView
    }
    
    // NSScrubberFlowLayoutDelegate
    func scrubber(_ scrubber: NSScrubber, layout: NSScrubberFlowLayout, sizeForItemAt itemIndex: Int) -> NSSize {
        
        assert(itemIndex < c!.c64.historicSnapshots())
        let w = c!.c64.historicSnapshotImageWidth(itemIndex)
        let h = c!.c64.historicSnapshotImageHeight(itemIndex)
        // return NSSize(width: 40, height: (40*h/w))
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
        // determine corresponding snapshot index in time travel buffer
        let ttindex = numberOfItems(for: scrubber) - index - 1
        c!.c64.restoreHistoricSnapshot(ttindex)
    }
    
}

//
//  TimeTravelTouchBar.swift
//  V64
//
//  Created by Dirk Hoffmann on 11.06.17.
//
//

import Foundation

@available(OSX 10.12.2, *)
extension NSTouchBarItem.Identifier {
    
    static let ttscrubber = NSTouchBarItem.Identifier("com.virtualc64.TouchBarItem.ttscrubber")
}

/*! @brief   TimeTravel TouchBar
 *  @details TouchBar is associated with a popup button
 */
@available(OSX 10.12.2, *)
class TimeTravelTouchBar: NSTouchBar {
    
    var parentItem: NSPopoverTouchBarItem?
    var c : MyController?
    
    func rebuild()
    {
        let scrubberItem = item(forIdentifier: .ttscrubber) as? TimeTravelScrubber
        scrubberItem?.rebuild()
    }

    func dismiss(_ sender: Any?)
    {
        guard let popover = parentItem else { return }
        popover.dismissPopover(sender)
    }
    
    override init()
    {
        super.init()
        delegate = self
        defaultItemIdentifiers = [.ttscrubber]
    }
    
    required init?(coder aDecoder: NSCoder)
    {
        fatalError("init(coder:) has not been implemented")
    }
    
    convenience init(parent: NSPopoverTouchBarItem?, controller: MyController?)
    {
        self.init()
        self.parentItem = parent
        self.c = controller
    }
}

@available(OSX 10.12.2, *)
extension TimeTravelTouchBar: NSTouchBarDelegate {

    func touchBar(_ touchBar: NSTouchBar, makeItemForIdentifier identifier: NSTouchBarItem.Identifier) -> NSTouchBarItem? {
        
        NSLog("\(#function)")
        
        switch identifier {
            
        case NSTouchBarItem.Identifier.ttscrubber:
            
            let item = TimeTravelScrubber(identifier: identifier, controller: c!)
            return item

        default:
            
            return nil
        }
    }
}



//
// This file is part of VirtualC64 - A cycle accurate Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
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
    
    /*
    func rebuild()
    {
    }
    */
    
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
        default:
            return nil
        }
    }
}


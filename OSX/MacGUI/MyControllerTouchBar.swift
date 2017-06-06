//
//  MyControllerTouchBar.swift
//  V64
//
//  Created by Dirk Hoffmann on 06.06.17.
//
//

import Foundation

extension NSTouchBarItemIdentifier {
    static let lizzy = NSTouchBarItemIdentifier("virtualc64.lizzy")
    static let lilly = NSTouchBarItemIdentifier("virtualc64.lilly")
    static let luzi = NSTouchBarItemIdentifier("virtualc64.luzi")
}

@available(OSX 10.12.2, *)
extension MyController : NSTouchBarDelegate {
    
    func lizzy () { print("Lizzy") }
    func lilly () { print("Lilly") }
    func luzi () { print("Luzi") }

    override open func makeTouchBar() -> NSTouchBar? {
 
        print("MyController.makeTouchBar");
        
        let touchBar = NSTouchBar()
        touchBar.delegate = self
        touchBar.defaultItemIdentifiers = [.lizzy, .lilly, .luzi]
        return touchBar
    }
    
    public func touchBar(_ touchBar: NSTouchBar, makeItemForIdentifier identifier: NSTouchBarItemIdentifier) -> NSTouchBarItem? {
        
        switch identifier {
            
        case NSTouchBarItemIdentifier.lizzy:
            let item = NSCustomTouchBarItem(identifier: identifier)
            item.view = NSButton(title: "🙈 Lizzy",
                                 target: self,
                                 action: #selector(lizzy))
            return item
            
        case NSTouchBarItemIdentifier.lilly:
            let item = NSCustomTouchBarItem(identifier: identifier)
            item.view = NSButton(title: "🙉 Lilly",
                                 target: self,
                                 action: #selector(lilly))
            return item

        case NSTouchBarItemIdentifier.luzi:
            let item = NSCustomTouchBarItem(identifier: identifier)
            item.view = NSButton(title: "🙊 Luzi",
                                 target: self,
                                 action: #selector(luzi))
            return item

        default: return nil
        }
    }
}

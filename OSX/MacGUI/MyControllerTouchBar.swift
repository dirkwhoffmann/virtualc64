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

    static let commodore = NSTouchBarItemIdentifier("com.virtualc64.TouchBarItem.commodore")
    static let runstop   = NSTouchBarItemIdentifier("com.virtualc64.TouchBarItem.runstop")
    static let restore   = NSTouchBarItemIdentifier("com.virtualc64.TouchBarItem.restore")
    static let home      = NSTouchBarItemIdentifier("com.virtualc64.TouchBarItem.home")
    static let del       = NSTouchBarItemIdentifier("com.virtualc64.TouchBarItem.del")
    static let larrow    = NSTouchBarItemIdentifier("com.virtualc64.TouchBarItem.larrow")
    static let uarrow    = NSTouchBarItemIdentifier("com.virtualc64.TouchBarItem.uarrow")
}

@available(OSX 10.12.2, *)
extension MyController : NSTouchBarDelegate {
    
    func lizzy() { c64.keyboard.typeText("Lizzy") }
    func lilly() { c64.keyboard.typeText("Lilly") }
    func luzi() { c64.keyboard.typeText("Luzi") }
    
    override open func makeTouchBar() -> NSTouchBar? {
 
        print("MyController.makeTouchBar");
        
        let touchBar = NSTouchBar()
        touchBar.delegate = self
        touchBar.defaultItemIdentifiers = [.commodore,
                                           .runstop,
                                           .restore,
                                           .home]
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

        case NSTouchBarItemIdentifier.commodore:
            let item = NSCustomTouchBarItem(identifier: identifier)
            item.view = NSButton(title: "C=",
                                 target: self,
                                 action: #selector(commodoreKeyAction))
            return item

        case NSTouchBarItemIdentifier.runstop:
            let image = NSImage(named: "runstop")
            
            let item = NSCustomTouchBarItem(identifier: identifier)
            item.view = NSButton(// title: "Runstop",
                                 image: image!,
                                 target: self,
                                 action: #selector(runstopAction))
            
            return item

        case NSTouchBarItemIdentifier.restore:
            let item = NSCustomTouchBarItem(identifier: identifier)
            item.view = NSButton(title: "Restore",
                                 target: self,
                                 action: #selector(restoreAction))
            return item

        case NSTouchBarItemIdentifier.home:
            let item = NSCustomTouchBarItem(identifier: identifier)
            item.view = NSButton(title: "Home",
                                 target: self,
                                 action: #selector(homeKeyAction))
            return item
            
        default: return nil
        }
    }
}

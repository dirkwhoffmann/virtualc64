//
//  VirtualKeyboardController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 30.07.18.
//

import Foundation

class VirtualKeyboardController : NSWindowController
{
    /// Reference to the parent controller
    var parent: MyController!
    
    /// Array holding a reference to the view of each key
    var keyView = Array(repeating: nil as NSButton?, count: 66)

    /// Array holding a reference to the image of each key
    var keyImage = Array(repeating: nil as NSImage?, count: 66)

    /// Indicates if the left Shift key is pressed
    var lshift = false

    /// Indicates if the right Shift key is pressed
    var rshift = false

    /// Indicates if the Commodore key is pressed
    var commodore = false

    override open func showWindow(_ sender: Any?) {
        
        parent = sender as! MyController
        super.showWindow(sender)
    }
    
    override func windowDidLoad() {
        
        // Setup key references
        for tag in 0 ... 65 {
            keyView[tag] = window!.contentView!.viewWithTag(tag) as? NSButton
        }

        updateImages()
    }
    
    func updateImages() {
        
        var pressed: Bool
        
        for nr in 0 ... 65 {
            
            switch(nr) {
            case 49: pressed = commodore
            case 50: pressed = lshift
            case 61: pressed = rshift
            default: pressed = false
            }
            keyView[nr]!.image = C64Key(nr).image(pressed: pressed,
                                                  shift: lshift || rshift)
        }
    }
    
    @IBAction func pressVirtualC64Key(_ sender: Any!) {
        
        let tag = (sender as! NSButton).tag
        track("Key = \(tag)")
        
        switch (tag) {
        case 49: commodore = !commodore
        case 50: lshift = !lshift
        case 61: rshift = !rshift
        default: break
        }
        
        updateImages()
    }
        
}

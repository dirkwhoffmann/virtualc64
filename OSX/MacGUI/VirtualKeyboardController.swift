//
//  VirtualKeyboardController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 30.07.18.
//

import Foundation

class VirtualKeyboardController : NSWindowController
{
    var parent: MyController!

    override open func showWindow(_ sender: Any?) {
        
        parent = sender as! MyController
        super.showWindow(sender)
    }
    
    @IBAction func pressVirtualC64Key(_ sender: Any!) {
        
        let tag = (sender as! NSButton).tag
        track("Key = \(tag)")
    }
        
}

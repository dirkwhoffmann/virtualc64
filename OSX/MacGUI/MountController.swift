//
//  MountController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 06.02.18.
//

import Foundation

class MountController : NSWindowController {
    
    var attachment: ContainerProxy!
    var controller: MyController!
    var c64: C64Proxy!
    var parentWindow: NSWindow!
    
    func setParentController(_ controller: MyController) {
        
        let document = controller.document as! MyDocument
        self.controller = controller
        self.attachment = document.attachment
        self.c64 = document.c64
        self.parentWindow = controller.window
    }
    
    func cleanup() {
        track()
        self.controller = nil
        self.attachment = nil
        self.c64 = nil
        self.parentWindow = nil
    }
}


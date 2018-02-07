//
//  MountController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 06.02.18.
//

import Foundation

class MountController : NSWindowController {
    
    var controller: MyController!
    var archive: ArchiveProxy!
    var c64: C64Proxy!
    var parentWindow: NSWindow!
    
    func setParentController(_ controller: MyController) {
        
        let document = controller.document as! MyDocument
        self.controller = controller
        self.archive = document.attachment as!ArchiveProxy
        self.c64 = document.c64
        self.parentWindow = controller.window
    }
    
    func cleanup() {
        track()
        self.controller = nil
        self.archive = nil
        self.c64 = nil
        self.parentWindow = nil
    }
}


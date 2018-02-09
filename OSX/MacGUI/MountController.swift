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
    
    func showSheet(_ controller: MyController) {
        
        track()
        
        let document = controller.document as! MyDocument
        self.controller = controller
        self.attachment = document.attachment
        self.c64 = document.c64
        self.parentWindow = controller.window
        
        parentWindow!.beginSheet(window!, completionHandler: { result in
            if result == NSApplication.ModalResponse.OK {
                controller.rotateBack()
            }
        })
    }
}


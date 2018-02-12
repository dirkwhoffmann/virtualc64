//
//  MountController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 06.02.18.
//

import Foundation

class UserDialogController : NSWindowController {
    
    var parent: MyController!
    var mydocument: MyDocument!
    var parentWindow: NSWindow!
    var c64: C64Proxy!
    
    func showSheet(withParent controller: MyController, completionHandler:(() -> Void)? = nil) {
        
        track()
        parent = controller
        mydocument = parent.document as! MyDocument
        parentWindow = parent.window
        c64 = mydocument.c64

        parentWindow.beginSheet(window!, completionHandler: { result in
            if result == NSApplication.ModalResponse.OK {
                completionHandler?()
            }
        })
    }
    
    func hideSheet() {
    
        window?.orderOut(self)
        parentWindow.endSheet(window!, returnCode: .cancel)
    }
    
    // Default action method for Cancel
    @IBAction func cancelAction(_ sender: Any!) {
        
        hideSheet()
    }
    
}


//
//  MountController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 06.02.18.
//

import Foundation

class UserDialogController : NSWindowController
{
    var parent: MyController!
    var parentWindow: NSWindow!
    var c64: C64Proxy!
    
    func showSheet(withParent controller: MyController, completionHandler:(() -> Void)? = nil) {
        
        track()
        parent = controller
        parentWindow = parent.window
        c64 = parent.mydocument.c64

        parentWindow.beginSheet(window!, completionHandler: { result in
            if result == NSApplication.ModalResponse.OK {
                
                self.cleanup()
                completionHandler?()
            }
        })
    }
    
    func cleanup() {
        // Don't delete this function. Calling cleanup in the sheet's completion handler
        // makes sure that ARC doesn't delete the reference too early.
    }
    
    func hideSheet() {
    
        window?.orderOut(self)
        parentWindow.endSheet(window!, returnCode: .cancel)
    }
    
    // Default action method for Cancel
    @IBAction func cancelAction(_ sender: Any!) {
        
        track()
        hideSheet()
    }
    
}


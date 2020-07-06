// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class UserDialogController: NSWindowController {

    func showSheet(completionHandler handler:(() -> Void)? = nil) {
        
        track()
        
        myWindow?.beginSheet(window!, completionHandler: { result in
            if result == NSApplication.ModalResponse.OK {
                
                self.cleanup()
                handler?()
            }
        })
        
        refresh()
    }
    
    func refresh() { }
    func keyDown(with key: MacKey) { }
    func cleanup() {
        // Don't delete this function. Calling cleanup in the sheet's completion
        // handler makes sure that ARC doesn't delete the reference too early.
    }
    
    func hideSheet() {
    
        if let win = window {
            win.orderOut(self)
            myWindow?.endSheet(win, returnCode: .cancel)
        }
    }
 
    // Default action method for OK
    @IBAction func okAction(_ sender: Any!) {
        
        track()
        hideSheet()
    }
    
    // Default action method for Cancel
    @IBAction func cancelAction(_ sender: Any!) {
        
        track()
        hideSheet()
    }
}

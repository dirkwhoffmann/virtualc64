// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class DialogWindow: NSWindow {

    // Delegation method for ESC and Cmd+.
    override func cancelOperation(_ sender: Any?) {
                
        if let controller = delegate as? DialogController {
            controller.cancelAction(sender)
        }
    }
}

/* Base class for all auxiliary windows.
 * The class extends NSWindowController by a reference to the controller
 * of the connected emulator window (parent) and a reference to the parents
 * proxy object. It also provides some wrappers around showing and hiding the
 * window.
 */
protocol DialogControllerDelegate: class {
    
    // Called before beginSheet() is called
    func sheetWillShow()
    
    // Called after beginSheet() has beed called
    func sheetDidShow()
    
    // Called after the completion handler has been executed
    func cleanup()
}

class DialogController: NSWindowController {
    
    var parent: MyController!
    var c64: C64Proxy!
    
    // Remembers whether awakeFromNib has been called
    var awake = false
    
    static func make(parent: MyController, nibName: NSNib.Name) -> Self? {
        
        let controller = Self.init(windowNibName: nibName)
        controller.parent = parent
        controller.c64 = parent.c64
        
        return controller
    }

    override func windowWillLoad() {
        track()
    }
    
    override func windowDidLoad() {
        track()
    }
    
    override func awakeFromNib() {
        
        track()
        awake = true
        sheetWillShow()
    }
    
    func sheetWillShow() {
        
    }
    
    func sheetDidShow() {
        
    }
    
    func cleanup() {
        
    }
    
    func showSheet(completionHandler handler:(() -> Void)? = nil) {
        
        if awake { sheetWillShow() }
        
        myWindow?.beginSheet(window!, completionHandler: { result in
            if result == NSApplication.ModalResponse.OK {
                
                handler?()
                self.cleanup()
            }
        })
        
        sheetDidShow()
    }
    
    // func refresh() { }
    func keyDown(with key: MacKey) { }
    
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

// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class DialogWindow: NSWindow {

    // Delegation method for ESC and Cmd+
    override func cancelOperation(_ sender: Any?) {

        if let controller = delegate as? DialogController {
            controller.cancelAction(sender)
        }
    }
}

protocol DialogControllerDelegate: AnyObject {

    // Called before showWindow() or beginSheet() is called
    func dialogWillShow()

    // Called after showWindow() or beginSheet() has beed called
    func dialogDidShow()

    // Called after the completion handler has been executed
    func cleanup()
}

/* Base class for all auxiliary windows. The class extends NSWindowController
 * by a reference to the controller of the connected emulator window (parent)
 * and a reference to the parents proxy object. It also provides some wrappers
 * around showing and hiding the window.
 */
class DialogController: NSWindowController, DialogControllerDelegate {
    
    var parent: MyController!
    var emu: EmulatorProxy? { return parent.emu }
    var c64: C64Proxy? { return emu?.c64 }
    var mm: MediaManager { return parent.mydocument.mm }

    // References to all open dialogs (to make ARC happy)
    static var active: [DialogController] = []

    // Remembers whether awakeFromNib has been called
    // var awake = false

    // Lock that is kept during the lifetime of the dialog
    var lock = NSLock()

    // Indicates if this dialog is displayed as a sheet
    var sheet = false
    
    convenience init?(with controller: MyController, nibName: NSNib.Name) {

        self.init(windowNibName: nibName)
        
        lock.lock()
        parent = controller
    }

    func register() {

        DialogController.active.append(self)
        debug(.lifetime, "Register: \(DialogController.active)")
    }

    func unregister() {

        DialogController.active = DialogController.active.filter {$0 != self}
        debug(.lifetime, "Unregister: \(DialogController.active)")
    }

    /*
    override func windowWillLoad() {
    }
    */
    
    override func windowDidLoad() {
        
        debug(.lifetime)

        super.windowDidLoad()
        self.window?.delegate = self
    }

    /*
    override func awakeFromNib() {

        awake = true
        window?.delegate = self
        dialogWillShow()
    }
    */
    
    func dialogWillShow() {

        debug(.lifetime)
    }
    
    func dialogDidShow() {

        debug(.lifetime)
    }
    
    func cleanup() {
     
        debug(.lifetime)
    }

    func showAsWindow() {

        debug(.lifetime)
        
        sheet = false
        register()
        loadWindow()
        dialogWillShow()
        showWindow(self)
        dialogDidShow()
    }
    
    /*
    func showWindow(completionHandler handler: (() -> Void)? = nil) {

        register()
        if awake { dialogWillShow() }

        showWindow(self)
        dialogDidShow()
    }
    */
    
    func showAsSheet(completionHandler handler:(() -> Void)? = nil) {

        debug(.lifetime)
        
        sheet = true
        register()
        loadWindow()
        dialogWillShow()
        parent.window?.beginSheet(window!, completionHandler: { result in handler?() })
        dialogDidShow()
    }
    
    /*
    func showSheet(completionHandler handler: (() -> Void)? = nil) {
        
        register()
        if awake { dialogWillShow() }

        parent.window?.beginSheet(window!, completionHandler: { result in

            handler?()
            self.cleanup()
        })

        dialogDidShow()
    }
    */
    
    func hide() {

        cleanup()

        if sheet {
            if let win = window {
                parent.window?.endSheet(win, returnCode: .cancel)
            }
        } else {
            close()
        }

        unregister()
    }

    func join() {

        debug(.shutdown, "Wait until window is closed...")

        lock.lock()
        lock.unlock()
    }
    
    @IBAction func okAction(_ sender: Any!) {

        hide()
    }

    @IBAction func cancelAction(_ sender: Any!) {

        hide()
    }
}

extension DialogController: NSWindowDelegate {

    func windowDidBecomeKey(_ notification: Notification) {

        debug(.lifetime)
    }
    
    func windowWillClose(_ notification: Notification) {

        unregister()
        lock.unlock()
    }
}

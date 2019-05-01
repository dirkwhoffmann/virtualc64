//
// This file is part of VirtualC64 - A cycle accurate Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Cocoa

/// The delegate object of this application.
/// This variable is global and can be accessed from anywhere in the Swift code.
var myAppDelegate: MyAppDelegate {
    get {
        return NSApp.delegate as! MyAppDelegate
    }
}

/// The document of the currently active emulator instance.
/// This variable is global and can be accessed from anywhere in the Swift code.
var myDocument: MyDocument? {
    get {
        if let doc = NSApplication.shared.orderedDocuments.first as? MyDocument {
            return doc
        } else {
            track("No document object found. Returning nil.")
            return nil
        }
    }
}

/// The window controller of the currently active emulator instance.
/// This variable is global and can be accessed from anywhere in the Swift code.
var myController: MyController? {
    get {
        return myDocument?.windowControllers.first as? MyController
    }
}

/// The window of the currently active emulator instance.
/// This variable is global and can be accessed from anywhere in the Swift code.
var myWindow: NSWindow? {
    get {
        return myController?.window
    }
}

// The C64 proxy of the currently active emulator instance.
/// This variable is global and can be accessed from anywhere in the Swift code.
var proxy: C64Proxy? {
    get {
        return myDocument?.c64
    }
}


@NSApplicationMain
@objc public class MyAppDelegate: NSObject, NSApplicationDelegate {
    
    /// Virtual C64 keyboard (opened as a separate window)
    var virtualKeyboard: VirtualKeyboardController? = nil
    
    public func applicationDidFinishLaunching(_ aNotification: Notification) {
        
        track()
        
        // Make touch bar customizable
        if #available(OSX 10.12.2, *) {
            NSApplication.shared.isAutomaticCustomizeTouchBarMenuItemEnabled = true
        }
        
        // Add observers
        let dc = DistributedNotificationCenter.default
        dc.addObserver(self, selector: #selector(vc64ResetCommand(_:)),
                       name: Notification.Name("VC64Reset"),
                       object: nil)
        dc.addObserver(self, selector: #selector(vc64ConfigureCommand(_:)),
                       name: Notification.Name("VC64Configure"),
                       object: nil)
        dc.addObserver(self, selector: #selector(vc64MountCommand(_:)),
                       name: Notification.Name("VC64Mount"),
                       object: nil)
        dc.addObserver(self, selector: #selector(vc64TypeTextCommand(_:)),
                       name: Notification.Name("VC64TypeText"),
                       object: nil)
        dc.addObserver(self, selector: #selector(vc64TakeScreenshotCommand(_:)),
                       name: Notification.Name("VC64TakeScreenshot"),
                       object: nil)
        dc.addObserver(self, selector: #selector(vc64QuitCommand(_:)),
                       name: Notification.Name("VC64Quit"),
                       object: nil)
    }
    
    public func applicationWillTerminate(_ aNotification: Notification) {

        // Close virtual keyboard
        virtualKeyboard?.close()
        
        track()
    }
}

/// Personal delegation methods
extension MyAppDelegate {
    
    func windowDidBecomeMain(_ window: NSWindow) {
        
        // Iterate through all controllers
        for case let document as MyDocument in NSApplication.shared.orderedDocuments {
            if let controller = document.windowControllers.first as? MyController {
                
                let audioEngine = controller.audioEngine!
                if window == controller.window {
                    
                    // Turn on audio
                    // track("Turning on audio for window \(controller.window)")
                    if !audioEngine.isRunning {
                        audioEngine.rampUpFromZero()
                        audioEngine.startPlayback()
                    }

                } else {
                    
                    // Turn off audio
                    // track("Turning off audio for window \(controller.window)")
                    if audioEngine.isRunning {
                        audioEngine.stopPlayback()
                    }
                }
            }
        }
    }
}

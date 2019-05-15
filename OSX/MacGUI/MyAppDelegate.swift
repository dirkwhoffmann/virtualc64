//
// This file is part of VirtualC64 - A cycle accurate Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Cocoa

/* The delegate object of this application.
 * This variable is global and can be accessed from anywhere in the Swift code.
 */
var myAppDelegate: MyAppDelegate {
    let delegate = NSApp.delegate as? MyAppDelegate
    return delegate!
}

/* The document of the currently active emulator instance.
 * This variable is global and can be accessed from anywhere in the Swift code.
 */
var myDocument: MyDocument? {
    if let doc = NSApplication.shared.orderedDocuments.first as? MyDocument {
        return doc
    } else {
        track("No document object found. Returning nil.")
        return nil
    }
}

/* The window controller of the currently active emulator instance.
 * This variable is global and can be accessed from anywhere in the Swift code.
 */
var myController: MyController? {
    return myDocument?.windowControllers.first as? MyController
}

/* The window of the currently active emulator instance.
 * This variable is global and can be accessed from anywhere in the Swift code.
 */
var myWindow: NSWindow? {
    return myController?.window
}

/* The C64 proxy of the currently active emulator instance.
 * This variable is global and can be accessed from anywhere in the Swift code.
 */
var proxy: C64Proxy? {
    return myDocument?.c64
}

@NSApplicationMain
@objc public class MyAppDelegate: NSObject, NSApplicationDelegate {
    
    /// Virtual C64 keyboard (opened as a separate window)
    var virtualKeyboard: VirtualKeyboardController? = nil
    
    /// The list of recently inserted disk URLs.
    var recentlyInsertedDiskURLs: [URL] = []
    
    /// The list of recently exported disk URLs for drive 1.
    var recentlyExportedDisk1URLs: [URL] = []
    
    /// The list of recently exported disk URLs for drive 2.
    var recentlyExportedDisk2URLs: [URL] = []
    
    /// The list of recently inserted tape URLs.
    var recentlyInsertedTapeURLs: [URL] = []
    
    /// The list of recently atached cartridge URLs.
    var recentlyAttachedCartridgeURLs: [URL] = []
    
    
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
    
    //
    // Handling the lists of recently used URLs
    //
    
    func noteRecentlyUsedURL(_ url: URL, to list: inout [URL], size: Int) {
        if !list.contains(url) {
            if list.count == size {
                list.remove(at: size - 1)
            }
            list.insert(url, at: 0)
        }
    }
    
    func getRecentlyUsedURL(_ pos: Int, from list: [URL]) -> URL? {
        return (pos < list.count) ? list[pos] : nil
    }
    
    func noteNewRecentlyInsertedDiskURL(_ url: URL) {
        noteRecentlyUsedURL(url, to: &recentlyInsertedDiskURLs, size: 10)
    }
    
    func getRecentlyInsertedDiskURL(_ pos: Int) -> URL? {
        return getRecentlyUsedURL(pos, from: recentlyInsertedDiskURLs)
    }
    
    func noteNewRecentlyExportedDiskURL(_ url: URL, drive nr: Int) {
        
        precondition(nr == 1 || nr == 2)
        
        if (nr == 1) {
            noteRecentlyUsedURL(url, to: &recentlyExportedDisk1URLs, size: 1)
        } else {
            noteRecentlyUsedURL(url, to: &recentlyExportedDisk2URLs, size: 1)
        }
    }
    
    func getRecentlyExportedDiskURL(_ pos: Int, drive nr: Int) -> URL? {
        
        precondition(nr == 1 || nr == 2)
        
        if (nr == 1) {
            return getRecentlyUsedURL(pos, from: recentlyExportedDisk1URLs)
        } else {
            return getRecentlyUsedURL(pos, from: recentlyExportedDisk2URLs)
        }
    }
    
    func clearRecentlyExportedDiskURLs(drive nr: Int) {
        
        precondition(nr == 1 || nr == 2)
        
        if (nr == 1) {
            recentlyExportedDisk1URLs = []
        } else {
            recentlyExportedDisk2URLs = []
        }
    }
    
    func noteNewRecentlyInsertedTapeURL(_ url: URL) {
        noteRecentlyUsedURL(url, to: &recentlyInsertedTapeURLs, size: 10)
    }
    
    func getRecentlyInsertedTapeURL(_ pos: Int) -> URL? {
        return getRecentlyUsedURL(pos, from: recentlyInsertedTapeURLs)
    }
    
    func noteNewRecentlyAtachedCartridgeURL(_ url: URL) {
        noteRecentlyUsedURL(url, to: &recentlyAttachedCartridgeURLs, size: 10)
    }
    
    func getRecentlyAtachedCartridgeURL(_ pos: Int) -> URL? {
        return getRecentlyUsedURL(pos, from: recentlyAttachedCartridgeURLs)
    }
    
    func noteNewRecentlyUsedURL(_ url: URL) {
        
        switch (url.pathExtension.uppercased()) {
            
        case "D64", "T64", "G64", "PRG", "P00":
            noteNewRecentlyInsertedDiskURL(url)
            
        case "TAP":
            noteNewRecentlyInsertedTapeURL(url)
            
        case "CRT":
            noteNewRecentlyAtachedCartridgeURL(url)
            
        default:
            break
        }
    }
}


//
// Personal delegation methods
//

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

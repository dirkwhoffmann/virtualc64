//
// This file is part of VirtualC64 - A cycle accurate Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Cocoa

@NSApplicationMain
@objc public class AppDelegate: NSObject, NSApplicationDelegate {
    
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
        dc.addObserver(self, selector: #selector(vc64DragInCommand(_:)),
                       name: Notification.Name("VC64DragIn"),
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

        track()
    }
}

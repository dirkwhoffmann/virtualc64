//
// This file is part of VirtualC64 - A user-friendly Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Cocoa

@NSApplicationMain
public class AppDelegate: NSObject, NSApplicationDelegate {
    
    public func applicationDidFinishLaunching(_ aNotification: Notification) {
        
        track()
        
        // Make touch bar customizable
        if #available(OSX 10.12.2, *) {
            NSApplication.shared.isAutomaticCustomizeTouchBarMenuItemEnabled = true
        }
    }
    
    public func applicationWillTerminate(_ aNotification: Notification) {

        track()
    }
    
    /*
    public func application(_ application: NSApplication, willPresentError error: Error) -> Error {

        track()
        
        let nserror = error as NSError
        
        if (nserror.domain == "VirtualC64") {

            switch (nserror.code) {
                
            case 1:
                return NSError.snapshotVersionError(filename: "The provided snapshot")

            case 2:
                return NSError.unsupportedFormatError(filename: "The provided file")

            case 3:
                return NSError.corruptedFileError(filename: "The provided file")
                
            default:
                assert(false)
            }
        }
        
        return error
    }
    */

    /*
    public func applicationShouldHandleReopen(_ sender: NSApplication,
                                              hasVisibleWindows flag: Bool) -> Bool {
        print("\(#function)")
        assert(false)
        return false
    }
    
    public func applicationShouldOpenUntitledFile(_ sender: NSApplication) -> Bool {
        print("\(#function)")
        assert(false)
        return false
    }
 */
}

/*
extension AppDelegate: NSOpenSavePanelDelegate {
    
    public func panel(_ sender: Any, shouldEnable url: URL) -> Bool {
        
        track()
        return false
    }
}
*/

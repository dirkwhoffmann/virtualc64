// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import Cocoa

var myAppDelegate: MyAppDelegate { return NSApp.delegate as! MyAppDelegate }

@NSApplicationMain
@objc public class MyAppDelegate: NSObject, NSApplicationDelegate {
           
    @IBOutlet weak var drive8Menu: NSMenuItem!
    @IBOutlet weak var drive9Menu: NSMenuItem!
    @IBOutlet weak var datasetteMenu: NSMenuItem!

    // Replace the old document controller by instantiating a custom controller
    let myDocumentController = MyDocumentController()
    
    // Preferences
    var pref: Preferences!
    var prefController: PreferencesController?
    
    // Information provider for connected HID devices
    var database = DeviceDatabase()

    // User activity token obtained in applicationDidFinishLaunching()
    var token: NSObjectProtocol!

    override init() {
        
        super.init()
        pref = Preferences()
    }
    
    public func applicationDidFinishLaunching(_ aNotification: Notification) {
        
        debug(.lifetime)

        token = ProcessInfo.processInfo.beginActivity(options: [ .userInitiated ],
                                                      reason: "Running VirtualC64")
    }
    
    public func applicationWillTerminate(_ aNotification: Notification) {
        
        debug(.lifetime)

        ProcessInfo.processInfo.endActivity(token)
    }
}

//
// Personal delegation methods
//

extension MyAppDelegate {
    
    var documents: [MyDocument] {
        return NSDocumentController.shared.documents as? [MyDocument] ?? []
    }
    var windows: [NSWindow] {
        return documents.compactMap({ $0.windowForSheet })
    }
    var controllers: [MyController] {
        return documents.compactMap({ $0.windowForSheet?.windowController as? MyController })
    }
    var proxies: [C64Proxy] {
        return documents.map({ $0.c64 })
    }
    
    func windowDidBecomeMain(_ window: NSWindow) {
        
        for c in controllers {
            
            if c.window == window {
                
                // Start playback
                if !c.macAudio!.isRunning {
                    c.macAudio!.startPlayback()
                    if !c.c64.warpMode { c.c64.sid.rampUpFromZero() }
                }
                
                // Update the visibility of all drive menus
                c.hideOrShowDriveMenus()
                
            } else {
                
                // Stop playback
                if c.macAudio!.isRunning {
                    c.macAudio!.stopPlayback()
                    c.c64.sid.rampDown()
                }
            }
        }
    }
    
    // Callen when a HID device has been added
    func deviceAdded() {
        prefController?.refresh()
    }
    
    // Callen when a HID device has been removed
    func deviceRemoved() {
        prefController?.refresh()
    }

    // Callen when a HID device has been pulled
    func devicePulled(events: [GamePadAction]) {
        prefController?.refreshDeviceEvents(events: events)
    }
}

// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import Cocoa

@objc(MyApplication)
class MyApplication: NSApplication {

    /* Set this variable to true to take away the control of the Command key
     * from the application. This becomes necessary once the command key
     * is meant to be operate the C64 and not the Mac. If control is not taken
     * away, pressing the Command key in combination with another key will
     * trigger unwanted actions (e.g., Cmd+Q will quit the application).
     *
     * Like for all other secondary keys (Control, Option, etc.), function
     * 'flagsChanged' is invoked when the Command key is pressed or released.
     * However, this method is called too late in the command chain to intercept,
     * i.e., menu actions will already be carried out.
     *
     * The solution taken here is to override function sendEvent in the
     * Application class. This delegation function is called early enough in
     * the command chain to hide any Command key events from the Mac app.
     */
    var disableCmdKey = false

    override func sendEvent(_ event: NSEvent) {

        if disableCmdKey {

            if event.type == NSEvent.EventType.keyUp {

                if event.modifierFlags.contains(.command) {

                    debug(.events, "keyUp: Removing CMD flag")
                    event.cgEvent!.flags.remove(.maskCommand)
                    super.sendEvent(NSEvent(cgEvent: event.cgEvent!)!)
                    return
                }
            }
            if event.type == NSEvent.EventType.keyDown {

                if event.modifierFlags.contains(.command) {

                    debug(.events, "keyDown: Removing CMD flag")
                    event.cgEvent!.flags.remove(.maskCommand)
                    super.sendEvent(NSEvent(cgEvent: event.cgEvent!)!)
                    return
                }
            }
        }
        super.sendEvent(event)
    }
}

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

var myApp: MyApplication { return NSApp as! MyApplication }
var myAppDelegate: MyAppDelegate { return NSApp.delegate as! MyAppDelegate }

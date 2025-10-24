// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import Cocoa

@MainActor @objc(MyApplication)
class MyApplication: NSApplication {
    
}

@main @objc
public class MyAppDelegate: NSObject, NSApplicationDelegate {
    
    @IBOutlet weak var drive8Menu: NSMenuItem!
    @IBOutlet weak var drive8InsertRecent: NSMenuItem!
    @IBOutlet weak var drive8ExportRecent: NSMenuItem!
    
    @IBOutlet weak var drive9Menu: NSMenuItem!
    @IBOutlet weak var drive9InsertRecent: NSMenuItem!
    @IBOutlet weak var drive9ExportRecent: NSMenuItem!
    
    @IBOutlet weak var datasetteMenu: NSMenuItem!
    
    // Replace the old document controller by instantiating a custom controller
    let myDocumentController = MyDocumentController()
    
    // Indicates if the CapsLock key should control warp mode
    var mapCapsLockWarp = true
    
    // Preferences
    var pref: Preferences!
    var settingsController: SettingsWindowController?
    
    // Information provider for connected HID devices
    var database = DeviceDatabase()
    
    // Command line arguments
    var argv: [String] = []
    
    // User activity token obtained in applicationDidFinishLaunching()
    var token: NSObjectProtocol!
    
    override init() {
        
        super.init()
        pref = Preferences()
    }
    
    public func applicationSupportsSecureRestorableState(_ app: NSApplication) -> Bool {
        
        return true
    }
    
    public func application(_ application: NSApplication, open urls: [URL]) {
        
        debug(.lifetime, "application(open urls: \(urls))")
    }
    
    public func applicationDidFinishLaunching(_ aNotification: Notification) {
        
        token = ProcessInfo.processInfo.beginActivity(options: [ .idleSystemSleepDisabled, .suddenTerminationDisabled ], reason: "Running an emulator")
        argv = Array(CommandLine.arguments.dropFirst())
        
        debug(.lifetime, "Launched with arguments \(argv)")
    }
    
    public func applicationShouldTerminate(_ sender: NSApplication) -> NSApplication.TerminateReply {
        
        debug(.shutdown, "Delay a bit to let audio fade out...")
        usleep(250000)
        debug(.shutdown, "OK...")
        
        return .terminateNow
    }
    
    public func applicationWillTerminate(_ aNotification: Notification) {
        
        debug(.lifetime)
        ProcessInfo.processInfo.endActivity(token)
    }
}

//
// Personal delegation methods
//

@MainActor
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
    var proxies: [EmulatorProxy?] {
        return documents.map({ $0.emu })
    }
    
    static var currentController: MyController? {
        didSet {
            if currentController !== oldValue {
                oldValue?.emu?.put(.FOCUS, value: 0)
                currentController?.emu?.put(.FOCUS, value: 1)
            }
        }
    }
    
    // Callen when a HID device has been added
    func deviceAdded() {
        settingsController?.refresh()
    }
    
    // Callen when a HID device has been removed
    func deviceRemoved() {
        settingsController?.refresh()
    }
}

var myApp: MyApplication { return NSApp as! MyApplication }
var myAppDelegate: MyAppDelegate { return NSApp.delegate as! MyAppDelegate }

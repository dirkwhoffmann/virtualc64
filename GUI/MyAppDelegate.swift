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

    // Replace the old document controller by instantiating a custom controller
    let myDocumentController = MyDocumentController()
    
    // Preferences
    var pref: Preferences!
    var prefController: PreferencesController?
    
    // Information provider for connected HID devices
    var database = DeviceDatabase()
    
    // List of recently inserted floppy disks (all drives share the same list)
    var insertedFloppyDisks: [URL] = []
    
    // List of recently exported floppy disks (one list for each drive)
    var exportedFloppyDisks: [[URL]] = [[URL]](repeating: [URL](), count: 2)

    // List of recently inserted tapes
    var insertedTapes: [URL] = []

    // List of recently attached cartridges
    var attachedCartridges: [URL] = []

    override init() {
        
        super.init()
        pref = Preferences()
    }
    
    public func applicationDidFinishLaunching(_ aNotification: Notification) {
        
        debug()
    }
    
    public func applicationWillTerminate(_ aNotification: Notification) {
        
        debug()
    }
    
    //
    // Handling lists of recently used URLs
    //
    
    func noteRecentlyUsedURL(_ url: URL, to list: inout [URL], size: Int) {
        
        if !list.contains(url) {
            
            // Shorten the list if it is too large
            if list.count == size { list.remove(at: size - 1) }
            
            // Add new item at the beginning
            list.insert(url, at: 0)
        }
    }
    
    func getRecentlyUsedURL(_ pos: Int, from list: [URL]) -> URL? {
        return (pos < list.count) ? list[pos] : nil
    }
    
    func noteNewRecentlyInsertedDiskURL(_ url: URL) {
        noteRecentlyUsedURL(url, to: &insertedFloppyDisks, size: 10)
    }
    
    func getRecentlyInsertedDiskURL(_ pos: Int) -> URL? {
        return getRecentlyUsedURL(pos, from: insertedFloppyDisks)
    }
    
    func noteNewRecentlyExportedDiskURL(_ url: URL, drive: Int) {
        
        precondition(drive == DRIVE8 || drive == DRIVE9)
        let n = drive == DRIVE8 ? 0 : 1
        noteRecentlyUsedURL(url, to: &exportedFloppyDisks[n], size: 1)
    }
    
    func getRecentlyExportedDiskURL(_ pos: Int, drive: Int) -> URL? {
        
        precondition(drive == DRIVE8 || drive == DRIVE9)
        let n = drive == DRIVE8 ? 0 : 1
        return getRecentlyUsedURL(pos, from: exportedFloppyDisks[n])
    }
    
    func clearRecentlyExportedDiskURLs(drive: Int) {
        
        precondition(drive == DRIVE8 || drive == DRIVE9)
        let n = drive == DRIVE8 ? 0 : 1
        exportedFloppyDisks[n] = [URL]()
    }
    
    func noteNewRecentlyInsertedTapeURL(_ url: URL) {
        noteRecentlyUsedURL(url, to: &insertedTapes, size: 10)
    }
    
    func getRecentlyInsertedTapeURL(_ pos: Int) -> URL? {
        return getRecentlyUsedURL(pos, from: insertedTapes)
    }
    
    func noteNewRecentlyAtachedCartridgeURL(_ url: URL) {
        noteRecentlyUsedURL(url, to: &attachedCartridges, size: 10)
    }
    
    func getRecentlyAtachedCartridgeURL(_ pos: Int) -> URL? {
        return getRecentlyUsedURL(pos, from: attachedCartridges)
    }
    
    func noteNewRecentlyUsedURL(_ url: URL) {
        
        switch url.pathExtension.uppercased() {
            
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

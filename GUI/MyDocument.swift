// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import UniformTypeIdentifiers

extension UTType {

    static let workspace = UTType("de.dirkwhoffmann.retro.vc64")!
    static let snapshot = UTType("de.dirkwhoffmann.retro.vcsnap")!
    static let retrosh = UTType("de.dirkwhoffmann.retro.retrosh")!
    static let d64 = UTType("public.retro.d64")!
    static let g64 = UTType("public.retro.g64")!
    static let t64 = UTType("public.retro.t64")!
    static let prg = UTType("public.retro.prg")!
    static let p00 = UTType("public.retro.p00")!
    static let crt = UTType("public.retro.crt")!
    static let tap = UTType("public.retro.tap")!
}

class MyDocument: NSDocument {

    var controller: MyController { return windowControllers.first as! MyController }
    var console: Console { return controller.renderer.console }
    var canvas: Canvas { return controller.renderer.canvas }
    var pref: Preferences { return myAppDelegate.pref }

    // Optional media URL provided on app launch
    var launchURL: URL?

    // The media manager for this document
    var mm: MediaManager!

    // Gateway to the core emulator
    var emu: EmulatorProxy?

    // Snapshot storage
    static let maxSnapshots: Int = 16
    private(set) var snapshots = ManagedArray<MediaFileProxy>(maxCount: maxSnapshots)

    //
    // Initializing
    //

    override init() {

        debug(.lifetime)

        super.init()
        // Check for OS compatibility
        /*
        if #available(macOS 26, *) {

            showAlert(.unsupportedOSVersion)
            NSApp.terminate(self)
            return
        }
         */

        // Check for Metal support
        if MTLCreateSystemDefaultDevice() == nil {

            showAlert(.noMetalSupport)
            NSApp.terminate(self)
            return
        }

        // Create the media manager
        mm = MediaManager(with: self)

        // Register all GUI related user defaults
        EmulatorProxy.defaults.registerUserDefaults()

        // Load the user default settings
        EmulatorProxy.defaults.load()

        // Create an emulator instance
        emu = EmulatorProxy()
    }

    override class var autosavesInPlace: Bool {
        
        return false
    }
    
    override open func makeWindowControllers() {

        debug(.lifetime)

        let controller = MyController(windowNibName: "MyDocument")
        self.addWindowController(controller)
    }

    func shutDown() {

        debug(.shutdown, "Remove proxy...")

        emu?.kill()
        emu = nil

        debug(.shutdown, "Done")
    }

    //
    // Loading
    //

    override open func read(from url: URL, ofType typeName: String) throws {

        debug(.media)

        launchURL = url
        /*
         do {
         try mm.addMedia(url: url, allowedTypes: vc64.FileType.draggable)

         } catch let error as AppError {

         throw NSError(error: error)
         }
         */
    }

    override open func revert(toContentsOf url: URL, ofType typeName: String) throws {

        debug(.media)

        do {
            try mm.addMedia(url: url, allowedTypes: [.SNAPSHOT])

        } catch let error as AppError {

            throw NSError(error: error)
        }
    }

    //
    // Saving
    //

    override func write(to url: URL, ofType typeName: String) throws {

        debug(.media)

        if typeName == UTType.snapshot.identifier {

            if let snapshot = MediaFileProxy.make(withC64: emu) {

                do {
                    try snapshot.writeToFile(url: url)

                } catch let error as AppError {

                    throw NSError(error: error)
                }
            }
        }
    }

    //
    // Handling snapshots
    //

    /*
    func processSnapshotFile(url: URL) throws {

        let file = try createMediaFileProxy(from: url, allowedTypes: [.SNAPSHOT])
        try processSnapshotFile(file: file)
    }

    func processSnapshotFile(file: MediaFileProxy) throws {

        try emu?.amiga.loadSnapshot(file)
        appendSnapshot(file: file)
    }

    @discardableResult
    func appendSnapshot(file: MediaFileProxy) -> Bool {

        // Remove the oldest entry if applicable
        if snapshots.full && pref.snapshotAutoDelete { snapshots.remove(at: 0) }

        // Only proceed if there is space left
        if snapshots.full { return false }

        // Append the snapshot
        snapshots.append(file, size: file.size)
        return true
    }
    */
}

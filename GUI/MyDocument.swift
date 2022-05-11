// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class MyDocument: NSDocument {

    var pref: Preferences { return myAppDelegate.pref }

    // The window controller for this document
    var parent: MyController { return windowControllers.first as! MyController }

    // The media manager for this document
    var mm: MediaManager!

    // Gateway to the core emulator
    var c64: C64Proxy!

    // Snapshot storage
    private(set) var snapshots = ManagedArray<SnapshotProxy>(capacity: 32)

    //
    // Initializing
    //
    
    override init() {

        debug(.lifetime)

        super.init()

        // Check for Metal support
        if MTLCreateSystemDefaultDevice() == nil {

            showAlert(.noMetalSupport)
            NSApp.terminate(self)
            return
        }
        
        // Create the media manager
        mm = MediaManager(with: self)

        // Register all GUI related user defaults
        C64Proxy.defaults.registerUserDefaults()

        // Load the user default settings
        C64Proxy.defaults.load()

        // Create an emulator instance
        c64 = C64Proxy()
    }

    override open func makeWindowControllers() {
        
        debug(.lifetime)

        // Create the window controller
        let controller = MyController(windowNibName: "MyDocument")
        controller.c64 = c64
        self.addWindowController(controller)
    }

    //
    // Loading
    //

    override open func read(from url: URL, ofType typeName: String) throws {

        debug(.media)

        do {

            try mm.addMedia(url: url, allowedTypes: FileType.draggable)

        } catch let error as VC64Error {

            throw NSError(error: error)
        }
    }

    override open func revert(toContentsOf url: URL, ofType typeName: String) throws {

        debug(.media)

        do {
            try mm.addMedia(url: url, allowedTypes: [.SNAPSHOT])

        } catch let error as VC64Error {

            throw NSError(error: error)
        }
    }

    //
    // Saving
    //

    override func write(to url: URL, ofType typeName: String) throws {

        debug(.media)

        if typeName == "VC64" {

            if let snapshot = SnapshotProxy.make(withC64: c64) {

                do {
                    try snapshot.writeToFile(url: url)

                } catch let error as VC64Error {

                    throw NSError(error: error)
                }
            }
        }
    }
}

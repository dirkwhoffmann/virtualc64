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

    static let vc64 = UTType("de.dirkwhoffmann.retro.vc64")!
    static let ini = UTType("de.dirkwhoffmann.retro.ini")!
    static let d64 = UTType("de.dirkwhoffmann.retro.d64")!
    static let g64 = UTType("de.dirkwhoffmann.retro.g64")!
    static let t64 = UTType("de.dirkwhoffmann.retro.t64")!
    static let prg = UTType("de.dirkwhoffmann.retro.prg")!
    static let p00 = UTType("de.dirkwhoffmann.retro.p00")!
    static let crt = UTType("de.dirkwhoffmann.retro.crt")!
    static let tap = UTType("de.dirkwhoffmann.retro.tap")!
}

class MyDocument: NSDocument {

    var pref: Preferences { return myAppDelegate.pref }

    // The window controller for this document
    var parent: MyController { return windowControllers.first as! MyController }

    // Optional media URL provided on app launch
    var launchUrl: URL?

    // The media manager for this document
    var mm: MediaManager!

    // Gateway to the core emulator
    var emu: EmulatorProxy?

    // Snapshot storage
    private(set) var snapshots = ManagedArray<MediaFileProxy>(maxSize: 512 * 1024 * 1024)

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
        EmulatorProxy.defaults.registerUserDefaults()

        // Load the user default settings
        EmulatorProxy.defaults.load()

        // Create an emulator instance
        emu = EmulatorProxy()
    }

    override open func makeWindowControllers() {
        
        debug(.lifetime)

        // Create the window controller
        let controller = MyController(windowNibName: "MyDocument")
        self.addWindowController(controller)
    }

    func shutDown() {

        debug(.shutdown, "Remove proxy...")

        emu!.kill()
        emu = nil

        debug(.shutdown, "Done")
    }

    //
    // Loading
    //

    override open func read(from url: URL, ofType typeName: String) throws {

        debug(.media)

        launchUrl = url
        /*
        do {
            try mm.addMedia(url: url, allowedTypes: vc64.FileType.draggable)

        } catch let error as VC64Error {

            throw NSError(error: error)
        }
        */
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

        if typeName == UTType.vc64.identifier {

            if let snapshot = MediaFileProxy.make(withC64: emu) {

                do {
                    try snapshot.writeToFile(url: url)

                } catch let error as VC64Error {

                    throw NSError(error: error)
                }
            }
        }
    }
}

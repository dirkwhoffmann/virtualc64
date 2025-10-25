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

    // The window controller of this document
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

    // Snapshots
    static let maxSnapshots: Int = 16
    private(set) var snapshots = ManagedArray<MediaFileProxy>(maxCount: maxSnapshots)

    //
    // Initializing
    //

    override init() {

        debug(.lifetime)

        super.init()

        // Check for OS compatibility
        if #available(macOS 27, *) {

            showAlert(.unsupportedOSVersion)
            NSApp.terminate(self)
            return
        }

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
    }

    override open func revert(toContentsOf url: URL, ofType typeName: String) throws {

        debug(.media)

        do {
            try mm.mount(url: url, allowedTypes: [.WORKSPACE])

        } catch let error as AppError {

            throw NSError(error: error)
        }
    }

    //
    // Saving
    //

    override func save(to url: URL, ofType typeName: String, for saveOperation: NSDocument.SaveOperationType) async throws {

        debug(.media, "url = \(url)")

        if typeName == "de.dirkwhoffmann.retro.vc64" {

            do {
                // Save the workspace
                try emu?.c64.saveWorkspace(url: url)

                // Add a screenshot to the workspace bundle
                if let image = canvas.screenshot(source: .emulator, cutout: .visible) {

                    // Convert to target format
                    let data = image.representation(using: .png)

                    // Save to file
                    try data?.write(to: url.appendingPathComponent("preview.png"))
                }

                // Save a plist file containing the machine properties
                saveMachineDescription(to: url.appendingPathComponent("machine.plist"))

                // Update the document's title and save status
                self.fileURL = url
                self.windowForSheet?.title = url.deletingPathExtension().lastPathComponent
                self.updateChangeCount(.changeCleared)

            } catch let error as AppError {

                throw NSError(error: error)
            }
        }
    }

    func saveMachineDescription(to url: URL) {

        guard let emu = emu else { return }

        var dictionary: [String: Any] = [:]

        let vicRev = VICIIRev(rawValue: emu.get(.VICII_REVISION))
        let basic = emu.c64.getRomTraits(.BASIC)
        let kernal = emu.c64.getRomTraits(.KERNAL)
        let char = emu.c64.getRomTraits(.CHAR)
        let vc1541 = emu.c64.getRomTraits(.VC1541)

        // Collect some info about the emulated machine
        dictionary["VICII"] = vicRev
        dictionary["BasicRom"] = String(cString: basic.title)
        dictionary["KernalRom"] = String(cString: kernal.title)
        dictionary["CharRom"] = String(cString: char.title)
        dictionary["VC1541Rom"] = String(cString: vc1541.title)
        dictionary["Version"] = EmulatorProxy.version()
        do {

            let data = try PropertyListSerialization.data(fromPropertyList: dictionary, format: .xml, options: 0)
            try data.write(to: url)

        } catch { }
    }

    //
    // Handling workspaces
    //

    func processWorkspaceFile(url: URL, force: Bool = false) throws {

        // Load workspace
        try emu?.c64.loadWorkspace(url: url)

        // Update the document's title and save status
        self.fileURL = url
        self.windowForSheet?.title = url.deletingPathExtension().lastPathComponent
        self.updateChangeCount(.changeCleared)

        // Scan directory for additional media files
        let supportedTypes: [String : FileType] =
        ["d64": .D64, "g64": .G64, "crt": .CRT, "tap": .TAP]
        let exclude = ["drive8", "drive9", "basic", "kernal", "char", "vc1541"]

        let contents = try FileManager.default.contentsOfDirectory(at: url, includingPropertiesForKeys: nil)
        for file in contents {
            if !exclude.contains(url.deletingPathExtension().lastPathComponent) {
                if let type = supportedTypes[file.pathExtension.lowercased()] {
                    mm.noteNewRecentlyOpenedURL(file, type: type)
                }
            }
        }
    }

    //
    // Handling snapshots
    //

    func processSnapshotFile(url: URL) throws {

        let file = try MediaManager.createFileProxy(from: url, type: .SNAPSHOT)
        try processSnapshotFile(file: file)
    }

    func processSnapshotFile(file: MediaFileProxy) throws {

        try emu?.c64.loadSnapshot(file)
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

    //
    // Handling scripts
    //

    func processScriptFile(url: URL, force: Bool = false) throws {

        let file = try MediaManager.createFileProxy(from: url, type: .SCRIPT)
        try processScriptFile(file: file, force: force)
    }

    func processScriptFile(file: MediaFileProxy, force: Bool = false) throws {

        console.runScript(script: file)
    }
}

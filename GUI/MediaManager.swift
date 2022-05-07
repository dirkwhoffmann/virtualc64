// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class MediaManager {

    // References to other objects
    var controller: MyController!
    var document: MyDocument!
    var c64: C64Proxy!

    // Shared list of recently inserted tapes
    static var insertedTapes: [URL] = []

    // Shared list of recently attached cartridges
    static var attachedCartridges: [URL] = []

    // Shared list of recently inserted floppy disks
    static var insertedFloppyDisks: [URL] = []

    // Unshared list of recently exported floppy disks
    var exportedFloppyDisks: [[URL]] = [[URL]](repeating: [URL](), count: 2)

    //
    // Initializing
    //

    init(with controller: MyController) {

        debug(.lifetime, "Creating media manager")
        
        self.controller = controller
        self.document = controller.mydocument
        self.c64 = controller.c64
    }

    //
    // Handling lists of recently used URLs
    //

    static func noteRecentlyUsedURL(_ url: URL, to list: inout [URL], size: Int) {

        if !list.contains(url) {

            // Shorten the list if it is too large
            if list.count == size { list.remove(at: size - 1) }

            // Add new item at the beginning
            list.insert(url, at: 0)
        }
    }

    static func getRecentlyUsedURL(_ pos: Int, from list: [URL]) -> URL? {
        return (pos < list.count) ? list[pos] : nil
    }

    // Import URLs (shared among all emulator instances and drives)

    static func noteNewRecentlyInsertedTapeURL(_ url: URL) {
        noteRecentlyUsedURL(url, to: &insertedTapes, size: 10)
    }

    static func getRecentlyInsertedTapeURL(_ pos: Int) -> URL? {
        return getRecentlyUsedURL(pos, from: insertedTapes)
    }

    static func noteNewRecentlyAtachedCartridgeURL(_ url: URL) {
        noteRecentlyUsedURL(url, to: &attachedCartridges, size: 10)
    }

    static func getRecentlyAtachedCartridgeURL(_ pos: Int) -> URL? {
        return getRecentlyUsedURL(pos, from: attachedCartridges)
    }

    static func noteNewRecentlyInsertedDiskURL(_ url: URL) {
        noteRecentlyUsedURL(url, to: &insertedFloppyDisks, size: 10)
    }

    static func getRecentlyInsertedDiskURL(_ pos: Int) -> URL? {
        return getRecentlyUsedURL(pos, from: insertedFloppyDisks)
    }

    static func noteNewRecentlyUsedURL(_ url: URL) {

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

    // Export URLs (not shared)

    func noteNewRecentlyExportedDiskURL(_ url: URL, drive: Int) {

        precondition(drive == DRIVE8 || drive == DRIVE9)
        let n = drive == DRIVE8 ? 0 : 1
        MediaManager.noteRecentlyUsedURL(url, to: &exportedFloppyDisks[n], size: 1)
    }

    func getRecentlyExportedDiskURL(_ pos: Int, drive: Int) -> URL? {

        precondition(drive == DRIVE8 || drive == DRIVE9)
        let n = drive == DRIVE8 ? 0 : 1
        return MediaManager.getRecentlyUsedURL(pos, from: exportedFloppyDisks[n])
    }

    func clearRecentlyExportedDiskURLs(drive: Int) {

        precondition(drive == DRIVE8 || drive == DRIVE9)
        let n = drive == DRIVE8 ? 0 : 1
        exportedFloppyDisks[n] = [URL]()
    }
}

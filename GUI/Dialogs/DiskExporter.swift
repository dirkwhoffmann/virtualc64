// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class DiskExporter: DialogController {

    enum Format {

        static let d64 = 0
        static let t64 = 1
        static let prg = 2
        static let p00 = 3
        static let vol = 4
    }

    var myDocument: MyDocument { return parent.mydocument! }

    @IBOutlet weak var icon: NSImageView!
    @IBOutlet weak var title: NSTextField!
    @IBOutlet weak var info1: NSTextField!
    @IBOutlet weak var info2: NSTextField!
    @IBOutlet weak var formatPopup: NSPopUpButton!
    @IBOutlet weak var exportButton: NSButton!

    // Panel for exporting files
    var savePanel: NSSavePanel!

    // Panel for exporting directories
    var openPanel: NSOpenPanel!

    // Reference to the export drive
    var drive: DriveProxy?

    // The disk to export
    var disk: DiskProxy?

    // Result of the file system decoder
    var volume: FileSystemProxy?

    func showSheet(diskDrive nr: DriveID) {

        drive = c64.drive(nr)

        // Get the disk from the specified drive
        disk = drive?.disk

        // Try to extract the file system
        if disk != nil { volume = try? FileSystemProxy.make(disk: disk!) }

        super.showSheet()
    }

    func updateFormatPopup() {

        func addItem(_ title: String, tag: Int) {

            formatPopup.addItem(withTitle: title)
            formatPopup.lastItem?.tag = tag
            formatPopup.isEnabled = volume != nil
        }

        formatPopup.autoenablesItems = false
        formatPopup.removeAllItems()
        addItem("D64", tag: Format.d64)
        addItem("T64", tag: Format.t64)
        addItem("PRG", tag: Format.prg)
        addItem("P00", tag: Format.p00)
        addItem("Folder", tag: Format.vol)
    }

    override public func awakeFromNib() {

        super.awakeFromNib()
        updateFormatPopup()
        update()
    }

    override func windowDidLoad() {

    }

    override func sheetDidShow() {

    }

    func update() {

        // Update icons
        updateIcon()

        // Update disk description
        updateTitleText()
        updateInfo()
    }

    func updateIcon() {

    /*
        switch formatPopup.selectedTag() {

        case Format.vol:

            icon.image = NSImage(named: "NSFolder")

        default:

            let wp = drive!.hasProtectedDisk
        TODO: GET DISK ICON FROM PROXY
            icon.image =
            adf?.icon(protected: wp) ??
            img?.icon(protected: wp) ??
            ext?.icon(protected: wp) ?? nil

        case Format.vol:

            icon.image = NSImage(named: "NSFolder")

        default:

            icon.image = nil
        }

        if icon.image == nil {
            icon.image = NSImage(named: "biohazard")
        }
     */
    }

    func updateTitleText() {

        /*
        title.stringValue =
        hdf != nil ? "Amiga Hard Drive" :
        adf != nil ? "Amiga Floppy Disk" :
        ext != nil ? "Extended Amiga Disk" :
        img != nil ? "PC Disk" : "Unrecognized device"
         */
    }

    func updateInfo() {

        /*
        if adf != nil {
            info1.stringValue = adf!.typeInfo + ", " + adf!.layoutInfo
        } else {
            info1.stringValue = ""
        }
        if vol != nil {
            info2.stringValue = vol!.dos.description
        } else {
            info2.stringValue = "No compatible file system"
        }
        */
    }

    //
    // Action methods
    //

    @IBAction func formatAction(_ sender: NSPopUpButton!) {

        update()
    }

    @IBAction func exportAction(_ sender: NSButton!) {

        switch formatPopup.selectedTag() {

        case Format.d64: openExportToFilePanel(allowedTypes: ["d64", "D64"])
        case Format.t64: openExportToFilePanel(allowedTypes: ["t64", "T64"])
        case Format.prg: openExportToFilePanel(allowedTypes: ["prg", "PRG"])
        case Format.p00: openExportToFilePanel(allowedTypes: ["p00", "P00"])
        case Format.vol: openExportToFolderPanel()

        default: fatalError()
        }
    }

    //
    // Exporting
    //

    func openExportToFilePanel(allowedTypes: [String]) {

        savePanel = NSSavePanel()
        savePanel.prompt = "Export"
        savePanel.title = "Export"
        savePanel.nameFieldLabel = "Export As:"
        savePanel.nameFieldStringValue = "Untitled." + allowedTypes.first!
        savePanel.canCreateDirectories = true

        savePanel.beginSheetModal(for: window!, completionHandler: { result in
            if result == .OK {
                if let url = self.savePanel.url {
                    self.export(url: url)
                }
            }
        })
    }

    func openExportToFolderPanel() {

        openPanel = NSOpenPanel()
        openPanel.prompt = "Export"
        openPanel.title = "Export"
        openPanel.nameFieldLabel = "Export As:"
        openPanel.canChooseDirectories = true
        openPanel.canCreateDirectories = true

        openPanel.beginSheetModal(for: window!, completionHandler: { result in
            if result == .OK {
                if let url = self.openPanel.url {
                    self.export(url: url)
                }
            }
        })
    }

    func export(url: URL) {

        /*
        do {

            switch formatPopup.selectedTag() {

            case Format.d64:

                log("Exporting D64")
                try parent.mydocument.export(fileProxy: adf!, to: url)

            case Format.ext:

                log("Exporting Extended ADF")
                try parent.mydocument.export(fileProxy: ext!, to: url)

            case Format.img:

                log("Exporting IMG")
                try parent.mydocument.export(fileProxy: img!, to: url)

            case Format.ima:

                log("Exporting IMA")
                try parent.mydocument.export(fileProxy: img!, to: url)

            case Format.vol:

                log("Exporting file system")
                try vol!.export(url: url)

            default:
                fatalError()
            }

            dfn!.markDiskAsUnmodified()
            myAppDelegate.noteNewRecentlyExportedDiskURL(url, df: dfn!.nr)

            hideSheet()

        } catch {
            parent.showAlert(.cantExport(url: url), error: error, async: true, window: window)
        }
        */

        parent.refreshStatusBar()
    }
}

//
// Protocols
//

extension DiskExporter: NSFilePromiseProviderDelegate {

    func filePromiseProvider(_ filePromiseProvider: NSFilePromiseProvider, fileNameForType fileType: String) -> String {

        var name: String

        switch formatPopup.selectedTag() {

        case Format.d64: name = "Untitled.d64"
        case Format.t64: name = "Untitled.t64"
        case Format.prg: name = "Untitled.prg"
        case Format.p00: name = "Untitled.p00"
        case Format.vol: name = "Untitled"

        default: fatalError()
        }

        return name
    }

    func filePromiseProvider(_ filePromiseProvider: NSFilePromiseProvider, writePromiseTo url: URL, completionHandler: @escaping (Error?) -> Void) {

        export(url: url)
        completionHandler(nil)
    }
}

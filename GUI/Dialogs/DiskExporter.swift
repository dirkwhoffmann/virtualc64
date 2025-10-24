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
        static let vol = 3
    }

    var myDocument: MyDocument { return parent.mydocument! }

    @IBOutlet weak var icon: NSImageView!
    @IBOutlet weak var title: NSTextField!
    @IBOutlet weak var info1: NSTextField!
    @IBOutlet weak var info2: NSTextField!
    @IBOutlet weak var info3: NSTextField!
    @IBOutlet weak var formatPopup: NSPopUpButton!
    @IBOutlet weak var exportButton: NSButton!

    // Panel for exporting files
    var savePanel: NSSavePanel!

    // Panel for exporting directories
    var openPanel: NSOpenPanel!

    // Reference to the export drive
    var drive: DriveProxy!

    // Results of the different decoders
    var d64: MediaFileProxy?
    var t64: MediaFileProxy?
    var prg: MediaFileProxy?
    var vol: FileSystemProxy?

    func showSheet(diskDrive nr: Int) {

        guard let emu = emu else { return }

        drive = emu.drive(nr)

        if drive.info.hasDisk {

            // Try to extract the file system
            vol = try? FileSystemProxy.make(with: drive)

            if vol != nil {

                // Try to run the D64 encoder
                d64 = try? MediaFileProxy.make(with: vol!, type: .D64)

                // Try to run the T64 encoder
                t64 = try? MediaFileProxy.make(with: vol!, type: .T64)

                if vol!.numFiles > 0 {

                    // Try to run the PRG encoder
                    prg = try? MediaFileProxy.make(with: vol!, type: .PRG)
                }
            }
        }

        super.showAsSheet()
    }

    func updateFormatPopup() {

        func addItem(_ title: String, tag: Int, enabled: Bool) {

            formatPopup.addItem(withTitle: title)
            formatPopup.lastItem?.tag = tag
            formatPopup.lastItem?.isEnabled = enabled
        }

        formatPopup.autoenablesItems = false
        formatPopup.removeAllItems()
        addItem("D64", tag: Format.d64, enabled: d64 != nil)
        addItem("T64", tag: Format.t64, enabled: t64 != nil)
        addItem("PRG", tag: Format.prg, enabled: prg != nil)
        addItem("Folder", tag: Format.vol, enabled: vol != nil)
    }

    override func dialogWillShow() {

        super.dialogWillShow()
        updateFormatPopup()
        update()
    }

    override func windowDidLoad() {

    }

    override func dialogDidShow() {

    }

    func update() {

        // Update icons
        updateIcon()

        // Update disk description
        updateTitleText()
        updateInfo()
    }

    func updateIcon() {

        let wp = drive!.info.hasProtectedDisk

        switch formatPopup.selectedTag() {

        case Format.d64: icon.image = d64?.icon(protected: wp)
        case Format.t64: icon.image = t64?.icon(protected: wp)
        case Format.prg: icon.image = prg?.icon(protected: wp)
        case Format.vol: icon.image = NSImage(named: "NSFolder")

        default:
            fatalError()
        }

        if icon.image == nil {
            icon.image = NSImage(named: "biohazard")
        }
    }

    func updateTitleText() {

        title.stringValue = "Commodore Floppy Disk"
    }

    func updateInfo() {

        info1.stringValue = vol?.layoutInfo ?? ""
        info2.stringValue = vol?.dosInfo ?? "No compatible file system"
        info3.stringValue = vol?.usageInfo ?? ""
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
                    if self.export(url: url) {
                        self.hide()
                    }                }
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
                    if self.export(url: url) {
                        self.hide()
                    }
                }
            }
        })
    }

    @discardableResult
    func export(url: URL) -> Bool {

        var rememberUrl = true

        do {

            switch formatPopup.selectedTag() {

            case Format.d64:

                debug(.media, "Exporting D64")
                try mm.export(file: d64!, to: url)

            case Format.t64:

                debug(.media, "Exporting T64")
                try mm.export(file: t64!, to: url)

            case Format.prg:

                debug(.media, "Exporting PRG")
                try mm.export(file: prg!, to: url)

            case Format.vol:

                debug(.media, "Exporting file system")
                try vol!.export(url: url)
                rememberUrl = false

            default:
                fatalError()
            }

            emu?.put(.DSK_UNMODIFIED, value: drive.info.id)
            if rememberUrl {
                mm.noteNewRecentlyExportedDiskURL(url, drive: drive.info.id)
            }

        } catch {

            parent.showAlert(.cantExport(url: url), error: error, async: true, window: window)
            return false
        }

        parent.refreshStatusBar()
        return true
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

// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import Darwin

class DiskCreator: DialogController {

    @IBOutlet weak var icon: NSImageView!
    @IBOutlet weak var capacity: NSPopUpButton!
    @IBOutlet weak var fileSystem: NSPopUpButton!
    @IBOutlet weak var nameLabel: NSTextField!
    @IBOutlet weak var nameField: NSTextField!

    var dos: vc64.DOSType { return fileSystem.selectedTag() == 1 ? .CBM : .NODOS }
    var name: String { return nameField.stringValue }

    var nr = DRIVE8
    var diskType: String!
    var drive: DriveProxy? { emu?.drive(nr) }

    //
    // Starting up
    //

    func showSheet(forDrive nr: Int) {

        self.nr = nr
        super.showAsSheet()
    }

    override public func awakeFromNib() {

        super.awakeFromNib()

        if let emu = emu {

            let type = emu.get(.DRV_TYPE, drive: nr)
            switch vc64.DriveType(rawValue: type) {

            case .VC1541, .VC1541C, .VC1541II:
                capacity.lastItem?.title = "5.25\" SD"

            default:
                fatalError()
            }

            fileSystem.selectItem(withTag: 1)
            update()
        }
    }

    override func windowDidLoad() {

    }

    override func dialogDidShow() {

    }

    //
    // Updating the displayed information
    //

    func update() {

        let nodos = fileSystem.selectedTag() == 0

        // Hide some controls
        let controls: [NSControl: Bool] = [

            nameField: nodos,
            nameLabel: nodos
        ]

        for (control, hidden) in controls {
            control.isHidden = hidden
        }
    }

    //
    // Action methods
    //

    @IBAction func capacityAction(_ sender: NSPopUpButton!) {

        update()
    }

    @IBAction func fileSystemAction(_ sender: NSPopUpButton!) {

        update()
    }

    @IBAction func insertAction(_ sender: Any!) {

        drive?.insertBlankDisk(dos, name: name)
        parent.mm.clearRecentlyExportedDiskURLs(drive: nr)
        hide()
    }
}

extension DiskCreator: NSFilePromiseProviderDelegate {

    func filePromiseProvider(_ filePromiseProvider: NSFilePromiseProvider, fileNameForType fileType: String) -> String {

        return "Empty.d64"
    }

    func filePromiseProvider(_ filePromiseProvider: NSFilePromiseProvider, writePromiseTo url: URL, completionHandler: @escaping (Error?) -> Void) {

        if let fs = FileSystemProxy.make(with: .SS_SD, dosType: dos) {

            do {
                fs.name = name
                let d64 = try MediaFileProxy.make(with: fs, type: .D64)
                try d64.writeToFile(url: url)
            } catch {
                warn("Can't export file to \(url)")
            }
        }
        completionHandler(nil)
    }
}

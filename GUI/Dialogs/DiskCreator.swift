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

    @IBOutlet weak var diskIcon: NSImageView!

    @IBOutlet weak var capacity: NSPopUpButton!
    @IBOutlet weak var fileSystem: NSPopUpButton!
    @IBOutlet weak var nameLabel: NSTextField!
    @IBOutlet weak var nameField: NSTextField!

    var nr = DriveID.DRIVE8
    var diskType: String!
    var drive: DriveProxy? { c64.drive(nr) }

    //
    // Starting up
    //

    func showSheet(forDrive nr: DriveID) {

        self.nr = nr
        super.showSheet()
    }

    override public func awakeFromNib() {

        super.awakeFromNib()

        let type = c64.getConfig(.DRV_TYPE, drive: nr)
        switch DriveType(rawValue: type) {

        case .VC1541, .VC1541C, .VC1541II:
            capacity.lastItem?.title = "5.25\" SD"

        default:
            fatalError()
        }

        fileSystem.selectItem(withTag: 1)
        update()
    }

    override func windowDidLoad() {

    }

    override func sheetDidShow() {

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

        let dos: DOSType =
        fileSystem.selectedTag() == 1 ? .CBM : .NODOS

        let name = nameField.stringValue
        log("Dos = \(dos) Name = \(name)")

        drive?.insertNewDisk(dos, name: name)
        myAppDelegate.clearRecentlyExportedDiskURLs(drive: nr)
        hideSheet()
    }
}

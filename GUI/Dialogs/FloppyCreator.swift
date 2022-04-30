// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import Darwin

class FloppyCreator: DialogController {

    /*
    @IBOutlet weak var diskIcon: NSImageView!
    @IBOutlet weak var virusIcon: NSImageView!

    @IBOutlet weak var capacity: NSPopUpButton!
    @IBOutlet weak var fileSystem: NSPopUpButton!
    @IBOutlet weak var bootBlock: NSPopUpButton!
    @IBOutlet weak var bootBlockLabel: NSTextField!
    @IBOutlet weak var nameLabel: NSTextField!
    @IBOutlet weak var nameField: NSTextField!

    @IBOutlet weak var cylinderField: NSTextField!
    @IBOutlet weak var headField: NSTextField!
    @IBOutlet weak var sectorField: NSTextField!

    var nr = 0
    var diskType: String!

    var drive: FloppyDriveProxy? { amiga.df(nr) }
    var hasVirus: Bool { return bootBlock.selectedTag() >= 3 }

    //
    // Starting up
    //

    func showSheet(forDrive nr: Int) {

        self.nr = nr
        super.showSheet()
    }

    override public func awakeFromNib() {

        super.awakeFromNib()

        let type = amiga.getConfig(.DRIVE_TYPE, drive: nr)
        switch FloppyDriveType(rawValue: type) {

        case .DD_35:

            capacity.lastItem?.title = "3.5\" DD"
            cylinderField.integerValue = 80
            headField.integerValue = 2
            sectorField.integerValue = 11

        case .HD_35:

            capacity.lastItem?.title = "3.5\" HD"
            cylinderField.integerValue = 80
            headField.integerValue = 2
            sectorField.integerValue = 22

        case .DD_525:

            capacity.lastItem?.title = "5.25\" DD"
            cylinderField.integerValue = 80
            headField.integerValue = 2
            sectorField.integerValue = 11

        default:
            fatalError()
        }

        // Preselect OFS as default file system
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

            virusIcon: !hasVirus,
            bootBlock: nodos,
            bootBlockLabel: nodos,
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

    @IBAction func bootBlockAction(_ sender: NSPopUpButton!) {

        update()
    }

    @IBAction func insertAction(_ sender: Any!) {

        let fs: FSVolumeType =
        fileSystem.selectedTag() == 1 ? .OFS :
        fileSystem.selectedTag() == 2 ? .FFS : .NODOS

        let bb: BootBlockId =
        bootBlock.selectedTag() == 1 ? .AMIGADOS_13 :
        bootBlock.selectedTag() == 2 ? .AMIGADOS_20 :
        bootBlock.selectedTag() == 3 ? .SCA :
        bootBlock.selectedTag() == 4 ? .BYTE_BANDIT : .NONE

        let name = nameField.stringValue
        log("Dos = \(fs) Boot = \(bb) Name = \(name)")

        do {

            try drive?.insertNew(fileSystem: fs, bootBlock: bb, name: name)
            myAppDelegate.clearRecentlyExportedDiskURLs(df: nr)
            hideSheet()

        } catch {

            parent.showAlert(.cantInsert, error: error, window: window)
        }
    }
    */
}

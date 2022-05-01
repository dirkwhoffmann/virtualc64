// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension NSTextStorage {

    var fullRange: NSRange { return NSRange(location: 0, length: self.length) }

    func addAttr(_ name: NSAttributedString.Key, value: Any, range: NSRange?) {

        if let intersectedRange = range?.intersection(fullRange) {
            self.addAttribute(name, value: value, range: intersectedRange)
        }
    }

    func remAttr(_ name: NSAttributedString.Key, range: NSRange?) {

        if let intersectedRange = range?.intersection(fullRange) {
            self.removeAttribute(name, range: intersectedRange)
        }
    }
}

class DiskInspector: DialogController {

    @IBOutlet weak var trackPopup: NSPopUpButton!
    @IBOutlet weak var sectorPopup: NSPopUpButton!
    @IBOutlet weak var formatPopup: NSPopUpButton!
    @IBOutlet weak var trackTableView: TrackTableView!
    @IBOutlet weak var sectorTableView: SectorTableView!
    @IBOutlet weak var diskDataView: DiskDataView!
    @IBOutlet weak var warningText: NSTextField!
    @IBOutlet weak var warningButton: NSButton!

    var analyzer: DiskAnalyzerProxy?

    var drive: DriveProxy!

    var showTracks: Bool { return trackPopup.selectedTag() == 0 }
    var showHalftracks: Bool { return trackPopup.selectedTag() == 1 }
    var showGcr: Bool { return formatPopup.selectedTag() == 0 }
    var showBytes: Bool { return formatPopup.selectedTag() == 1 }

    var selectedHalftrack = -1
    var halftrack: Halftrack? {
        return isHalftrackNumber(selectedHalftrack) ? Halftrack(selectedHalftrack) : nil
    }

    var selectedSector = -1
    var sector: Sector? {
        return isSectorNumber(selectedSector) ? Sector(selectedSector) : nil
    }

    var hex = true

    //
    // Starting up
    //

    func show(diskDrive nr: DriveID) throws {

        log()

        drive = nr == .DRIVE8 ? c64.drive8 : c64.drive9
        showWindow()
    }

    override func sheetWillShow() {

        log()

        warningText.isHidden = true
        warningButton.isHidden = true
        trackPopup.autoenablesItems = false
        sectorPopup.autoenablesItems = false
        formatPopup.autoenablesItems = false
        analyzeDisk()

        refresh()
    }

    func refresh() {

        if analyzer == nil {
            selectedHalftrack = -1
            selectedSector = -1
        }

        // Warn if this track contains errors
        if let ht = halftrack {
            let trackIsValid = analyzer!.numErrors(ht) == 0
            warningText.isHidden = trackIsValid
            warningButton.isHidden = trackIsValid
        }

        // Grey out some choices
        if sector != nil {
            formatPopup.item(at: 1)!.isEnabled = true
        } else {
            formatPopup.item(at: 1)!.isEnabled = false
            formatPopup.selectItem(at: 0)
        }

        // Refresh sub views
        diskDataView.hex = hex
        trackTableView.refresh()
        sectorTableView.isDirty = true
        sectorTableView.refresh()
        diskDataView.refresh()

        // Grey out more some choices
        sectorPopup.item(at: 0)!.isEnabled = sectorTableView.numberOfRows > 0
    }

    func analyzeDisk() {

        if let disk = drive.disk {

            log("Analyzing disk")
            analyzer = DiskAnalyzerProxy(disk: disk)

        } else {
            log("Can't analyze disk (no disk present)")
            analyzer = nil
        }
    }

    func setSelectedTrack(_ nr: Int) {

        setSelectedHalftrack(2 * nr - 1)
    }

    func setSelectedHalftrack(_ nr: Int) {

        if selectedHalftrack != nr {

            selectedHalftrack = nr
            refresh()
        }
    }

    func setSelectedSector(_ nr: Int) {

        if selectedSector != nr {

            selectedSector = nr
            refresh()
        }
    }

    @IBAction func trackPopupAction(_ sender: NSPopUpButton!) {

        refresh()
    }

    @IBAction func sectorPopupAction(_ sender: NSPopUpButton!) {

        refresh()
    }

    @IBAction func formatPopupAction(_ sender: NSPopUpButton!) {

        refresh()
    }

    @IBAction func drvWarningAction(_ sender: NSButton!) {

        let controller = DiskErrorController(windowNibName: "DiskErrors")
        controller.parent = self

        window!.beginSheet(controller.window!, completionHandler: { result in
            if result == NSApplication.ModalResponse.OK {
                controller.cleanup()
            }
        })
    }
}

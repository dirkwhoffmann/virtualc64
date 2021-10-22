// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class ExportDialog: DialogController {
        
    @IBOutlet weak var diskIcon: NSImageView!
    @IBOutlet weak var title: NSTextField!
    @IBOutlet weak var layoutInfo: NSTextField!
    @IBOutlet weak var volumeInfo: NSTextField!
    @IBOutlet weak var fileInfo: NSTextField!

    @IBOutlet weak var disclosureButton: NSButton!
    @IBOutlet weak var disclosureText: NSTextField!
    @IBOutlet weak var previewScrollView: NSScrollView!
    @IBOutlet weak var previewTable: NSTableView!
    @IBOutlet weak var cylinderText: NSTextField!
    @IBOutlet weak var cylinderField: NSTextField!
    @IBOutlet weak var cylinderStepper: NSStepper!
    @IBOutlet weak var headText: NSTextField!
    @IBOutlet weak var headField: NSTextField!
    @IBOutlet weak var headStepper: NSStepper!
    @IBOutlet weak var trackText: NSTextField!
    @IBOutlet weak var trackField: NSTextField!
    @IBOutlet weak var trackStepper: NSStepper!
    @IBOutlet weak var sectorText: NSTextField!
    @IBOutlet weak var sectorField: NSTextField!
    @IBOutlet weak var sectorStepper: NSStepper!
    @IBOutlet weak var blockText: NSTextField!
    @IBOutlet weak var blockField: NSTextField!
    @IBOutlet weak var blockStepper: NSStepper!
    @IBOutlet weak var corruptionText: NSTextField!
    @IBOutlet weak var corruptionStepper: NSStepper!

    @IBOutlet weak var info1: NSTextField!
    @IBOutlet weak var info2: NSTextField!

    @IBOutlet weak var strictButton: NSButton!
    @IBOutlet weak var formatPopup: NSPopUpButton!
    @IBOutlet weak var exportButton: NSButton!
    
    var savePanel: NSSavePanel!  // Used to export to files
    var openPanel: NSOpenPanel!  // Used to export to directories

    let shrinkedHeight = CGFloat(196)
    let expandedHeight = CGFloat(476)

    var driveID: DriveID?
    var drive: DriveProxy? { return driveID == nil ? nil : c64.drive(driveID!) }
    var disk: DiskProxy?
    var volume: FSDeviceProxy?

    var errorReport: FSErrorReport?
    
    var selection: Int?
    var selectedRow: Int? { return selection == nil ? nil : selection! / 16 }
    var selectedCol: Int? { return selection == nil ? nil : selection! % 16 }
    var strict: Bool { return strictButton.state == .on }
        
    var myDocument: MyDocument { return parent.mydocument! }
    var size: CGSize { return window!.frame.size }
    var shrinked: Bool { return size.height < 300 }
        
    var numCyls: Int { return volume?.numCyls ?? 0 }
    var numSides: Int { return volume?.numHeads ?? 0 }
    var numTracks: Int { return volume?.numTracks ?? 0 }
    var numSectors: Int { return volume?.numSectors(trackNr) ?? 0 }
    var numBlocks: Int { return volume?.numBlocks ?? 0 }
    
    // Block preview
    var cylNr = 1
    var headNr = 0
    var trackNr = 1
    var sectorNr = 0
    var blockNr = 0
    let bytesPerRow = 16
    
    //
    // Selecting a block
    //
    
    func setCylinder(_ newValue: Int) {
                
        if let v = volume, newValue != cylNr {

            let value = newValue.clamped(1, numCyls)

            cylNr    = value
            trackNr  = v.trackNr(cylNr, head: headNr)
            sectorNr = sectorNr.clamped(0, numSectors - 1)
            blockNr  = v.blockNr(trackNr, sector: sectorNr)
            
            selection = nil
            update()
        }
    }
    
    func setHead(_ newValue: Int) {
        
        if let v = volume, newValue != headNr {
                        
            let value = newValue.clamped(0, numSides - 1)
            
            headNr   = value
            trackNr  = v.trackNr(blockNr)
            sectorNr = sectorNr.clamped(0, numSectors - 1)
            blockNr  = v.blockNr(trackNr, sector: sectorNr)

            selection = nil
            update()
        }
    }
    
    func setTrack(_ newValue: Int) {
        
        if let v = volume, newValue != trackNr {
                   
            let value = newValue.clamped(1, numTracks)
            
            trackNr  = value
            sectorNr = sectorNr.clamped(0, numSectors - 1)
            cylNr    = v.cylNr(trackNr)
            headNr   = v.headNr(trackNr)
            blockNr  = v.blockNr(trackNr, sector: sectorNr)

            selection = nil
            update()
        }
    }

    func setSector(_ newValue: Int) {
        
        if let v = volume, newValue != sectorNr {
                  
            let value = newValue.clamped(0, numSectors - 1)
            
            sectorNr = value
            blockNr  = v.blockNr(trackNr, sector: sectorNr)
            
            selection = nil
            update()
        }
    }

    func setBlock(_ newValue: Int) {
        
        if let v = volume, newValue != blockNr {
                        
            let value = newValue.clamped(0, numBlocks - 1)

            blockNr  = value
            trackNr  = v.trackNr(blockNr)
            sectorNr = v.sectorNr(blockNr)
            cylNr    = v.cylNr(trackNr)
            headNr   = v.headNr(trackNr)
            
            selection = nil
            update()
        }
    }

    func setCorruptedBlock(_ newValue: Int) {
                
        var jump: Int
         
        if newValue > blockNr {
            jump = volume!.nextCorrupted(blockNr)
        } else {
            jump = volume!.prevCorrupted(blockNr)
        }
        
        // track("Current: \(blockNr) Stepper: \(newValue) Jump: \(jump)")
        corruptionStepper.integerValue = jump
        setBlock(jump)
    }
    
    //
    // Starting up
    //
    
    func showSheet(forDrive nr: DriveID, completionHandler handler:(() -> Void)? = nil) {
        
        track()
        
        driveID = nr
        
        // Get the disk from the specified drive
        disk = c64.drive(nr).disk
                
        // Try to extract the file system
        if disk != nil { volume = try? FSDeviceProxy.make(disk: disk!) }
        
        // volume?.printDirectory()
                        
        super.showSheet(completionHandler: handler)
    }

    /*
    func showSheet(forVolume vol: FSDeviceProxy) {
        
        volume = vol
        super.showSheet()
    }
    */
    
    override public func awakeFromNib() {
        
        track()
        super.awakeFromNib()
        
        // Register to receive mouse click events
        previewTable.action = #selector(clickAction(_:))
        
        // Configure elements
        sectorStepper.maxValue = .greatestFiniteMagnitude
        blockStepper.maxValue = .greatestFiniteMagnitude
        
        // Start with a shrinked window
        var rect = window!.contentRect(forFrameRect: window!.frame)
        rect.size = CGSize(width: 606, height: shrinkedHeight)
        let frame = window!.frameRect(forContentRect: rect)
        window!.setFrame(frame, display: true)
        
        // Run a file system check
        errorReport = volume?.check(strict)
        
        update()
    }
    
    override func windowDidLoad() {
                    
        // Enable compatible file formats in the format selector popup
        formatPopup.autoenablesItems = false
        formatPopup.item(at: 0)!.isEnabled = volume != nil  // D64
        formatPopup.item(at: 1)!.isEnabled = volume != nil  // T64
        formatPopup.item(at: 2)!.isEnabled = volume != nil  // PRG
        formatPopup.item(at: 3)!.isEnabled = volume != nil  // P00
        formatPopup.item(at: 4)!.isEnabled = volume != nil  // DIR
        
        // Preselect an available export format and enable the Export button
        let enabled = [0, 1, 2, 3].filter { formatPopup.item(at: $0)!.isEnabled }
        if enabled.isEmpty {
            exportButton.isEnabled = false
        } else {
            exportButton.isEnabled = true
            formatPopup.selectItem(at: enabled.first!)
        }
        
        // Jump to the first corrupted block if an error was found
        if errorReport != nil && errorReport!.corruptedBlocks > 0 {
            setCorruptedBlock(1)
        } else {
            update()
        }
    }
    
    override func sheetDidShow() {
        
    }
    
    //
    // Expanding and shrinking the window
    //
    
    func shrink() { setHeight(shrinkedHeight) }
    func expand() { setHeight(expandedHeight) }
    
    func setHeight(_ newHeight: CGFloat) {
                
        var rect = window!.frame
        rect.origin.y += rect.size.height - newHeight
        rect.size.height = newHeight
        
        window!.setFrame(rect, display: true)
        
        // Force the preview table to appear at the correct vertical position
        var r = previewScrollView.frame
        r.origin.y = 96
        previewScrollView.frame = r

        exportButton.keyEquivalent = shrinked ? "\r" : ""

        update()
    }

    //
    // Updating the displayed information
    //
    
    func update() {
          
        // Update icons
        updateDiskIcon()

        // Update disk description
        updateTitleText()
        updateTrackAndSectorInfo()
        updateVolumeInfo()
        updateFileInfo()

        // Update the disclosure button state
        disclosureButton.state = shrinked ? .off : .on
        disclosureButton.isHidden = volume == nil
        disclosureText.isHidden = volume == nil

        // Hide some elements if the window is shrinked
        let items: [NSView] = [
            previewScrollView,
            cylinderText, cylinderField, cylinderStepper,
            headText, headField, headStepper,
            trackText, trackField, trackStepper,
            sectorText, sectorField, sectorStepper,
            blockText, blockField, blockStepper,
            corruptionText, corruptionStepper,
            info1, info2
        ]
        for item in items { item.isHidden = shrinked }
        
        // Hide more elements
        strictButton.isHidden = volume == nil
        
        // Only proceed if the window is expanded
        if shrinked { return }
                
        // Update all elements
        cylinderField.stringValue      = String(format: "%d", cylNr)
        cylinderStepper.integerValue   = cylNr
        headField.stringValue          = String(format: "%d", headNr)
        headStepper.integerValue       = headNr
        trackField.stringValue         = String(format: "%d", trackNr)
        trackStepper.integerValue      = trackNr
        sectorField.stringValue        = String(format: "%d", sectorNr)
        sectorStepper.integerValue     = sectorNr
        blockField.stringValue         = String(format: "%d", blockNr)
        blockStepper.integerValue      = blockNr
        corruptionStepper.integerValue = blockNr
        
        if let total = errorReport?.corruptedBlocks, total > 0 {
                     
            if let corr = volume?.getCorrupted(blockNr), corr > 0 {
                track("total = \(total) corr = \(corr)")
                corruptionText.stringValue = "Corrupted block \(corr) out of \(total)"
            } else {
                let blocks = total == 1 ? "block" : "blocks"
                corruptionText.stringValue = "\(total) corrupted \(blocks)"
            }
            
            corruptionText.textColor = .labelColor
            corruptionText.textColor = .warningColor
            corruptionStepper.isHidden = false
        
        } else {
            corruptionText.stringValue = ""
            corruptionStepper.isHidden = true
        }
        
        updateBlockInfo()
        previewTable.reloadData()
    }
    
    func updateDiskIcon() {
        
        let wp = drive!.hasWriteProtectedDisk()
        diskIcon.image = AnyFileProxy.diskIcon(protected: wp)
    }
    
    func updateTitleText() {
        
        var text = "This disk cannot be exported"
        var color = NSColor.textColor
        
        if volume != nil {
            text = "Commodore 64 Floppy Disk"
            color = NSColor.textColor
        }
        
        title.stringValue = text
        title.textColor = color
    }

    func updateTrackAndSectorInfo() {
        
        var text = "Unknown track and sector format"
        var color = NSColor.warningColor
        
        if volume != nil {
            text = volume!.layoutInfo
            color = .secondaryLabelColor
        }

        layoutInfo.stringValue = text
        layoutInfo.textColor = color
    }
    
    func updateVolumeInfo() {
        
        var text = "No compatible file system"
        var color = NSColor.warningColor
        
        if volume != nil {
            
            text = volume!.dos.description
            color = .secondaryLabelColor
            
            if let errors = errorReport?.corruptedBlocks, errors > 0 {
                
                let blocks = errors == 1 ? "block" : "blocks"
                text += " with \(errors) corrupted \(blocks)"
                color = .warningColor
            }
        }
        
        volumeInfo.stringValue = text
        volumeInfo.textColor = color
    }
    
    func updateFileInfo() {
        
        var text = ""
        let color = NSColor.secondaryLabelColor

        if volume != nil {
            
            let num = volume!.numFiles
            let free = volume!.numUsedBlocks
            
            let files = num == 1 ? "file" : "files"
            text = "\(num) \(files), \(free) blocks used"
        }
        
        fileInfo.stringValue = text
        fileInfo.textColor = color
    }
    
    func updateBlockInfo() {
        
        if volume == nil {
            info1.stringValue = ""
            info2.stringValue = ""
            return
        }
        
        if selection == nil {
            updateBlockInfoUnselected()
            updateErrorInfoUnselected()
        } else {
            updateBlockInfoSelected()
            updateErrorInfoSelected()
        }
    }
    
    func updateBlockInfoUnselected() {
        
        let type = volume!.blockType(blockNr)
        info1.stringValue = type.description
    }
    
    func updateBlockInfoSelected() {
        
        let usage = volume!.itemType(blockNr, pos: selection!)
        info1.stringValue = usage.description
    }

    func updateErrorInfoUnselected() {

        info2.stringValue = ""
    }

    func updateErrorInfoSelected() {
        
        var exp = UInt8(0)
        let error = volume!.check(blockNr, pos: selection!, expected: &exp, strict: strict)
        info2.stringValue = error.description(expected: Int(exp))
    }
        
    //
    // Exporting the disk
    //
    
    func exportToFile(allowedTypes: [String]) {
     
        track("allowedTypes = \(allowedTypes)")

        savePanel = NSSavePanel()
        savePanel.prompt = "Export"
        savePanel.title = "Export"
        savePanel.nameFieldLabel = "Export As:"
        savePanel.canCreateDirectories = true
        savePanel.allowsOtherFileTypes = false
        savePanel.nameFieldStringValue = "Untitled." + allowedTypes.first!
        savePanel.beginSheetModal(for: window!, completionHandler: { result in
            if result == .OK {
                if let url = self.savePanel.url {
                    self.exportToFile(url: url)
                }
            }
        })
    }

    func exportToFile(url: URL) {
        
        track("url: \(url)")

        do {
            try parent.mydocument.export(drive: driveID!, to: url)
            myAppDelegate.noteNewRecentlyExportedDiskURL(url, drive: driveID!)
            drive?.setModifiedDisk(false)
            hideSheet()

        } catch let error as VC64Error {
            error.warning("Cannot export disk")
        } catch {
            fatalError()
        }
    }

    func exportToDirectory() {

        openPanel = NSOpenPanel()
        openPanel.prompt = "Export"
        openPanel.title = "Export"
        openPanel.nameFieldLabel = "Export As:"
        openPanel.canChooseDirectories = true
        openPanel.canCreateDirectories = true
        
        openPanel.beginSheetModal(for: window!, completionHandler: { result in
            if result == .OK {
                if let url = self.openPanel.url {
                    track("url = \(url)")
                    self.openPanel.close()
                    self.exportToDirectory(url: url)
                }
            }
        })
    }
    
    func exportToDirectory(url: URL) {
        
        do {
            try volume!.exportDirectory(url: url)
            hideSheet()

        } catch let error as VC64Error {
            error.warning("Cannot export to directory")
        } catch {
            fatalError()
        }
    }
    
    //
    // Action methods
    //

    @IBAction func disclosureAction(_ sender: NSButton!) {
        
        shrinked ? expand() : shrink()
    }
    
    @IBAction func clickAction(_ sender: NSTableView!) {
        
        if sender.clickedColumn >= 1 && sender.clickedRow >= 0 {
            
            let newValue = 16 * sender.clickedRow + sender.clickedColumn - 1
            selection = selection != newValue ? newValue : nil
            update()
        }
    }

    @IBAction func cylinderAction(_ sender: NSTextField!) {
        
        setCylinder(sender.integerValue)
    }
    
    @IBAction func cylinderStepperAction(_ sender: NSStepper!) {
        
        setCylinder(sender.integerValue)
    }
    
    @IBAction func headAction(_ sender: NSTextField!) {
        
        setHead(sender.integerValue)
    }
    
    @IBAction func headStepperAction(_ sender: NSStepper!) {
        
        setHead(sender.integerValue)
    }
    
    @IBAction func trackAction(_ sender: NSTextField!) {
        
        setTrack(sender.integerValue)
    }
    
    @IBAction func trackStepperAction(_ sender: NSStepper!) {
        
        setTrack(sender.integerValue)
    }
    
    @IBAction func sectorAction(_ sender: NSTextField!) {
        
        setSector(sender.integerValue)
    }
    
    @IBAction func sectorStepperAction(_ sender: NSStepper!) {
        
        setSector(sender.integerValue)
    }
    
    @IBAction func blockAction(_ sender: NSTextField!) {
        
        setBlock(sender.integerValue)
    }
    
    @IBAction func blockStepperAction(_ sender: NSStepper!) {
        
        setBlock(sender.integerValue)
    }

    @IBAction func corruptedBlockStepperAction(_ sender: NSStepper!) {
    
        setCorruptedBlock(sender.integerValue)
    }

    @IBAction func strictAction(_ sender: NSButton!) {
        
        track()

        // Repeat the integrity check
        errorReport = volume?.check(strict)

        update()
    }
    
    @IBAction func exportAction(_ sender: NSButton!) {
        
        switch formatPopup.indexOfSelectedItem {
        case 0: exportToFile(allowedTypes: ["d64", "D64"])
        case 1: exportToFile(allowedTypes: ["t64", "T64"])
        case 2: exportToFile(allowedTypes: ["prg", "PRG"])
        case 3: exportToFile(allowedTypes: ["p00", "P00"])
        case 4: exportToFile(allowedTypes: ["g64", "G64"])
        case 5: exportToDirectory()
        default: fatalError()
        }
    }
        
    @IBAction override func cancelAction(_ sender: Any!) {
         
         track()
         hideSheet()
     }
}

//
// Extensions
//

extension ExportDialog: NSWindowDelegate {
    
    func windowDidResize(_ notification: Notification) {
        
        track()
    }
    
    func windowWillStartLiveResize(_ notification: Notification) {
         
         track()
     }
     
     func windowDidEndLiveResize(_ notification: Notification) {
        
        track()
     }
}

extension ExportDialog: NSTableViewDataSource {
    
    func columnNr(_ column: NSTableColumn?) -> Int? {
        
        return column == nil ? nil : Int(column!.identifier.rawValue)
    }
        
    func numberOfRows(in tableView: NSTableView) -> Int {
        
        return 256 / bytesPerRow
    }
    
    func tableView(_ tableView: NSTableView,
                   objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        
        if let col = columnNr(tableColumn) {

            if let byte = volume?.readByte(blockNr, offset: 16 * row + col) {
                return String(format: "%02X", byte)
            }

        } else {
            return String(format: "%X", row)
        }
        
        return ""
    }
}

extension ExportDialog: NSTableViewDelegate {
    
    func tableView(_ tableView: NSTableView, willDisplayCell cell: Any, for tableColumn: NSTableColumn?, row: Int) {

        var exp = UInt8(0)
        let cell = cell as? NSTextFieldCell

        if let col = columnNr(tableColumn) {
            
            let offset = 16 * row + col
            let error = volume?.check(blockNr,
                                      pos: offset,
                                      expected: &exp,
                                      strict: strict) ?? .OK
            
            if row == selectedRow && col == selectedCol {
                cell?.textColor = .white
                cell?.backgroundColor = error == .OK ? .selectedContentBackgroundColor : .red
            } else {
                cell?.textColor = error == .OK ? .textColor : .warningColor
                cell?.backgroundColor = NSColor.alternatingContentBackgroundColors[row % 2]
            }
        } else {
            cell?.backgroundColor = .windowBackgroundColor
        }
    }
    
    func tableView(_ tableView: NSTableView, shouldSelectRow row: Int) -> Bool {
        return false
    }
}

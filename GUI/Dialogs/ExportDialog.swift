// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#if false

class ExporterDialog: DialogController {
        
    @IBOutlet weak var diskIcon: NSImageView!
    @IBOutlet weak var virusIcon: NSImageView!
    @IBOutlet weak var title: NSTextField!
    @IBOutlet weak var layoutInfo: NSTextField!
    @IBOutlet weak var volumeInfo: NSTextField!
    @IBOutlet weak var bootInfo: NSTextField!
    @IBOutlet weak var decontaminationButton: NSButton!

    @IBOutlet weak var disclosureButton: NSButton!
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

    var driveNr: DriveID?
    var drive: DriveProxy? { return driveNr == nil ? nil : c64.drive(driveNr!) }
    // var disk: DiskFileProxy?
    // var volume: FSDeviceProxy?
    
    var selection: Int?
    var selectedRow: Int? { return selection == nil ? nil : selection! / 16 }
    var selectedCol: Int? { return selection == nil ? nil : selection! % 16 }
        
    var myDocument: MyDocument { return parent.mydocument! }
    var size: CGSize { return window!.frame.size }
    var shrinked: Bool { return size.height < 300 }
        
    var numCyls: Int { return disk?.numCyls ?? volume?.numCyls ?? 0 }
    var numSides: Int { return disk?.numSides ?? volume?.numHeads ?? 0 }
    var numTracks: Int { return disk?.numTracks ?? volume?.numTracks ?? 0 }
    var numSectors: Int { return disk?.numSectors ?? volume?.numSectors ?? 0 }
    var numBlocks: Int { return disk?.numBlocks ?? volume?.numBlocks ?? 0 }
    var isDD: Bool { return disk?.diskDensity == .DISK_DD }
    var isHD: Bool { return disk?.diskDensity == .DISK_HD }
    
    // Block preview
    var cylinderNr = 0
    var headNr = 0
    var trackNr = 0
    var sectorNr = 0
    var blockNr = 0
    
    // var sectorData: [String] = []
    let bytesPerRow = 16
    
    //
    // Selecting a block
    //
    
    func setCylinder(_ newValue: Int) {
        
        if newValue != cylinderNr {

            let value = newValue.clamped(0, numCyls - 1)

            cylinderNr = value
            trackNr    = cylinderNr * 2 + headNr
            blockNr    = trackNr * numSectors + sectorNr
            
            selection = nil
            update()
        }
    }
    
    func setHead(_ newValue: Int) {
        
        if newValue != headNr {
                        
            let value = newValue.clamped(0, numSides - 1)

            headNr     = value
            trackNr    = cylinderNr * 2 + headNr
            blockNr    = trackNr * numSectors + sectorNr
            
            selection = nil
            update()
        }
    }
    
    func setTrack(_ newValue: Int) {
        
        if newValue != trackNr {
                   
            let value = newValue.clamped(0, numTracks - 1)
            
            trackNr    = value
            cylinderNr = trackNr / 2
            headNr     = trackNr % 2
            blockNr    = trackNr * numSectors + sectorNr

            selection = nil
            update()
        }
    }

    func setSector(_ newValue: Int) {
        
        if newValue != sectorNr {
                  
            let value = newValue.clamped(0, numSectors - 1)
            
            sectorNr   = value
            blockNr    = trackNr * numSectors + sectorNr
            
            selection = nil
            update()
        }
    }

    func setBlock(_ newValue: Int) {
        
        if newValue != blockNr {
                        
            let value = newValue.clamped(0, numBlocks - 1)

            blockNr    = value
            trackNr    = blockNr / numSectors
            sectorNr   = blockNr % numSectors
            cylinderNr = trackNr / 2
            headNr     = trackNr % 2
            
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
    
    func showSheet(forDrive nr: Int) {
        
        track()
        
        driveNr = nr
        
        // Try to decode the disk with the ADF decoder
        disk = ADFFileProxy.make(withDrive: drive)
        
        // If it is an ADF, try to extract the file system
        if disk != nil { volume = FSDeviceProxy.make(withADF: disk as? ADFFileProxy) }
        
        // REMOVE ASAP
        // volume?.printDirectory(true)
        
        // If it is not an ADF, try the DOS decoder
        if disk == nil { disk = IMGFileProxy.make(withDrive: drive) }
                
        super.showSheet()
    }
    
    func showSheet(forVolume vol: FSDeviceProxy) {
        
        volume = vol
        
        // REMOVE ASAP
        // volume?.printDirectory(true)
        
        super.showSheet()
    }
        
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
        
        let adf = disk?.type == .FILETYPE_ADF
        let dos = disk?.type == .FILETYPE_IMG
            
        // Enable compatible file formats in the format selector popup
        formatPopup.autoenablesItems = false
        formatPopup.item(at: 0)!.isEnabled = adf
        formatPopup.item(at: 1)!.isEnabled = dos
        formatPopup.item(at: 2)!.isEnabled = dos
        formatPopup.item(at: 3)!.isEnabled = volume != nil
        
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
        r.origin.y = 82
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
        updateBootInfo()

        // Update the disclosure button state
        disclosureButton.state = shrinked ? .off : .on
        
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
        
        // Hide more elements if no errors are present
        if volume == nil || errorReport?.corruptedBlocks == 0 {
            corruptionText.isHidden = true
            corruptionStepper.isHidden = true
        }

        // Update all elements
        cylinderField.stringValue      = String.init(format: "%d", cylinderNr)
        cylinderStepper.integerValue   = cylinderNr
        headField.stringValue          = String.init(format: "%d", headNr)
        headStepper.integerValue       = headNr
        trackField.stringValue         = String.init(format: "%d", trackNr)
        trackStepper.integerValue      = trackNr
        sectorField.stringValue        = String.init(format: "%d", sectorNr)
        sectorStepper.integerValue     = sectorNr
        blockField.stringValue         = String.init(format: "%d", blockNr)
        blockStepper.integerValue      = blockNr
        corruptionStepper.integerValue = blockNr

        if let corr = volume?.getCorrupted(blockNr) {
            
            let total = errorReport!.corruptedBlocks

            if corr > 0 {
                corruptionText.stringValue = "Corrupted block \(corr) out of \(total)"
                corruptionText.textColor = .labelColor
            } else {
                corruptionText.stringValue = ""
                corruptionText.textColor = .secondaryLabelColor
            }
        }
        
        updateBlockInfo()
        previewTable.reloadData()
    }
    
    func updateDiskIcon() {
        
        if driveNr == nil {
            
            diskIcon.image = NSImage.init(named: "hdf")!
            virusIcon.isHidden = true
            decontaminationButton.isHidden = true
            
        } else {
            
            let wp = drive!.hasWriteProtectedDisk()
            diskIcon.image = disk!.icon(protected: wp)
            virusIcon.isHidden = !disk!.hasVirus
            decontaminationButton.isHidden = !disk!.hasVirus
        }
    }
    
    func updateTitleText() {
        
        var text = "This disk contains an unrecognized MFM stream"
        
        if driveNr == nil {
            
            text = "Amiga Hard Drive"
            
        } else {
            
            if disk?.type == .FILETYPE_ADF { text = "Amiga Disk" }
            if disk?.type == .FILETYPE_IMG { text = "PC Disk" }
        }
        
        title.stringValue = text
    }

    func updateTrackAndSectorInfo() {
        
        var text = "This disk contains un unknown track and sector format."
        
        if driveNr == nil {
            
            let blocks = volume!.numBlocks
            let capacity = blocks / 2000
            text = "\(capacity) MB (\(blocks) sectors)"
        
        } else {
            
            if disk != nil {
                text = disk!.layoutInfo
            } else {
                layoutInfo.textColor = .warningColor
            }
        }

        layoutInfo.stringValue = text
    }
    
    func updateVolumeInfo() {
        
        var text = "No compatible file system"
        
        if volume != nil {
            
            text = volume!.dos.description
            
            if let errors = errorReport?.corruptedBlocks, errors > 0 {
                
                let blocks = errors == 1 ? "block" : "blocks"
                let text2 = " with \(errors) corrupted \(blocks)"
                volumeInfo.stringValue = text + text2
                volumeInfo.textColor = .warningColor
                return
            }
            
        } else {
            
            volumeInfo.textColor = .warningColor
        }
        
        volumeInfo.stringValue = text
    }
    
    func updateBootInfo() {
                
        if driveNr == nil {
            bootInfo.stringValue = ""
            return
        }
                    
        bootInfo.stringValue = disk!.bootInfo
        bootInfo.textColor = disk!.hasVirus ? .warningColor : .secondaryLabelColor
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
        var text: String
        
        switch type {
        case .UNKNOWN_BLOCK:    text = "Unknown block type"
        case .EMPTY_BLOCK:      text = "Empty Block"
        case .BOOT_BLOCK:       text = "Boot Block"
        case .ROOT_BLOCK:       text = "Root Block"
        case .BITMAP_BLOCK:     text = "Bitmap Block"
        case .BITMAP_EXT_BLOCK: text = "Bitmap Extension Block"
        case .USERDIR_BLOCK:    text = "User Directory Block"
        case .FILEHEADER_BLOCK: text = "File Header Block"
        case .FILELIST_BLOCK:   text = "File List Block"
        case .DATA_BLOCK_OFS:   text = "Data Block (OFS)"
        case .DATA_BLOCK_FFS:   text = "Data Block (FFS)"
        default: fatalError()
        }
        
        info1.stringValue = text
    }
    
    func updateBlockInfoSelected() {
        
        let usage = volume!.itemType(blockNr, pos: selection!)
        var text: String
        
        switch usage {
        case .FSI_UNKNOWN:
            text = "Unknown"
        case .FSI_UNUSED:
            text = "Unused"
        case .FSI_DOS_HEADER:
            text = "AmigaDOS header signature"
        case .FSI_DOS_VERSION:
            text = "AmigaDOS version number"
        case .FSI_BOOTCODE:
            text = "Boot code instruction"
        case .FSI_TYPE_ID:
            text = "Type identifier"
        case .FSI_SUBTYPE_ID:
            text = "Subtype identifier"
        case .FSI_SELF_REF:
            text = "Block reference to itself"
        case .FSI_CHECKSUM:
            text = "Checksum"
        case .FSI_HASHTABLE_SIZE:
            text = "Hashtable size"
        case .FSI_HASH_REF:
            text = "Hashtable entry"
        case .FSI_PROT_BITS:
            text = "Protection status bits"
        case .FSI_BCPL_STRING_LENGTH:
            text = "BCPL string Length"
        case .FSI_BCPL_DISK_NAME:
            text = "Disk name (BCPL character)"
        case .FSI_BCPL_DIR_NAME:
            text = "Directory name (BCPL character)"
        case .FSI_BCPL_FILE_NAME:
            text = "File name (BCPL character)"
        case .FSI_BCPL_COMMENT:
            text = "Comment (BCPL character)"
        case .FSI_CREATED_DAY:
            text = "Creation date (days)"
        case .FSI_CREATED_MIN:
            text = "Creation date (minutes)"
        case .FSI_CREATED_TICKS:
            text = "Creation date (ticks)"
        case .FSI_MODIFIED_DAY:
            text = "Modification date (day)"
        case .FSI_MODIFIED_MIN:
            text = "Modification date (minutes)"
        case .FSI_MODIFIED_TICKS:
            text = "Modification date (ticks)"
        case .FSI_NEXT_HASH_REF:
            text = "Reference to the next hash block"
        case .FSI_PARENT_DIR_REF:
            text = "Parent directory block reference"
        case .FSI_FILEHEADER_REF:
            text = "File header block reference"
        case .FSI_EXT_BLOCK_REF:
            text = "Next extension block reference"
        case .FSI_BITMAP_BLOCK_REF:
            text = "Bitmap block reference"
        case .FSI_BITMAP_EXT_BLOCK_REF:
            text = "Extension bitmap block reference"
        case .FSI_BITMAP_VALIDITY:
            text = "Bitmap validity bits"
        case .FSI_DATA_BLOCK_REF_COUNT:
            text = "Number of data block references"
        case .FSI_FILESIZE:
            text = "File size"
        case .FSI_DATA_BLOCK_NUMBER:
            text = "Position in the data block chain"
        case .FSI_DATA_BLOCK_REF:
            text = "Data block reference"
        case .FSI_FIRST_DATA_BLOCK_REF:
            text = "Reference to the first data block"
        case .FSI_NEXT_DATA_BLOCK_REF:
            text = "Reference to next data block"
        case .FSI_DATA_COUNT:
            text = "Number of stored data bytes"
        case .FSI_DATA:
            text = "Data byte"
        case .FSI_BITMAP:
            text = "Block allocation table"
        default:
            fatalError()
        }
        
        info1.stringValue = text
    }

    func updateErrorInfoUnselected() {

        info2.stringValue = ""
    }

    func updateErrorInfoSelected() {
        
        var text: String
        var exp = UInt8(0)

        let error = volume!.check(blockNr, pos: selection!, expected: &exp, strict: strict)

        switch error {
        case .OK:
            text = ""
        case .EXPECTED_VALUE:
            text = String.init(format: "Expected $%02X", exp)
        case .EXPECTED_SMALLER_VALUE:
            text = String.init(format: "Expected a value less or equal $%02X", exp)
        case .EXPECTED_DOS_REVISION:
            text = "Expected a value between 0 and 7"
        case .EXPECTED_NO_REF:
            text = "Did not expect a block reference here"
        case .EXPECTED_REF:
            text = "Expected a block reference"
        case .EXPECTED_SELFREF:
            text = "Expected a self-reference"
    
        case .PTR_TO_UNKNOWN_BLOCK:
            text = "This reference points to a block of unknown type"
        case .PTR_TO_EMPTY_BLOCK:
            text = "This reference points to an empty block"
        case .PTR_TO_BOOT_BLOCK:
            text = "This reference points to a boot block"
        case .PTR_TO_ROOT_BLOCK:
            text = "This reference points to the root block"
        case .PTR_TO_BITMAP_BLOCK:
            text = "This reference points to a bitmap block"
        case .PTR_TO_USERDIR_BLOCK:
            text = "This reference points to a user directory block"
        case .PTR_TO_FILEHEADER_BLOCK:
            text = "This reference points to a file header block"
        case .PTR_TO_FILELIST_BLOCK:
            text = "This reference points to a file header block"
        case .PTR_TO_DATA_BLOCK:
            text = "This reference points to a data block"
        case .EXPECTED_DATABLOCK_NR:
            text = "Invalid data block position number"
        case .INVALID_HASHTABLE_SIZE:
            text = "Expected $48 (72 hash table entries)"
        default:
            track("error = \(error)")
            fatalError()
        }
        
        info2.stringValue = text
    }
        
    //
    // Exporting the disk
    //
    
    func exportToFile(allowedTypes: [String]) {
     
        savePanel = NSSavePanel()
        savePanel.prompt = "Export"
        savePanel.title = "Export"
        savePanel.nameFieldLabel = "Export As:"
        savePanel.canCreateDirectories = true
        
        savePanel.beginSheetModal(for: window!, completionHandler: { result in
            if result == .OK {
                if let url = self.savePanel.url {
                    track("url = \(url)")
                    self.exportToFile(url: url)
                }
            }
        })
    }

    func exportToFile(url: URL) {

        track("url = \(url)")
        parent.mydocument.export(drive: driveNr!, to: url, diskFileProxy: disk!)
        hideSheet()
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
        
        track("url = \(url)")
        
        let error = volume!.export(url.path)

        switch error {

        case .DIRECTORY_NOT_EMPTY:
            
            parent.critical("The destination directory is not empty.",
                            "To prevent accidental exports, the disk exporter " +
                            "refuses to work on non-empty folders.")
            
        default:

            hideSheet()
        }
    }
    
    //
    // Action methods
    //

    @IBAction func decontaminationAction(_ sender: NSButton!) {
        
        track()
        disk?.killVirus()
        volume?.killVirus()
        update()
    }

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
    
        track("New value: \(sender.integerValue)")
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
        case 0: exportToFile(allowedTypes: ["adf", "ADF"])
        case 1: exportToFile(allowedTypes: ["img", "IMG"])
        case 2: exportToFile(allowedTypes: ["ima", "IMA"])
        case 3: exportToDirectory()
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

extension ExporterDialog: NSWindowDelegate {
    
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

extension ExporterDialog: NSTableViewDataSource {
    
    func columnNr(_ column: NSTableColumn?) -> Int? {
        
        return column == nil ? nil : Int(column!.identifier.rawValue)
    }
        
    func numberOfRows(in tableView: NSTableView) -> Int {
        
        return 512 / bytesPerRow
    }
    
    func tableView(_ tableView: NSTableView,
                   objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        
        if let col = columnNr(tableColumn) {

            if let byte = volume?.readByte(blockNr, offset: 16 * row + col) {
                return String(format: "%02X", byte)
            }
            if let byte = disk?.readByte(blockNr, offset: 16 * row + col) {
                return String(format: "%02X", byte)
            }
        } else {
            return String(format: "%X", row)
        }
        
        return ""
    }
}

extension ExporterDialog: NSTableViewDelegate {
    
    func tableView(_ tableView: NSTableView, willDisplayCell cell: Any, for tableColumn: NSTableColumn?, row: Int) {

        var exp = UInt8(0)
        let cell = cell as? NSTextFieldCell

        if let col = columnNr(tableColumn) {
            
            let offset = 16 * row + col
            let error = volume?.check(blockNr, pos: offset, expected: &exp, strict: strict) ?? .OK
            
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

#endif

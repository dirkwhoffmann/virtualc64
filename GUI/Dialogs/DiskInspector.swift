// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class DiskInspector: DialogController {

    /*
    @IBOutlet weak var icon: NSImageView!
    @IBOutlet weak var title: NSTextField!
    @IBOutlet weak var subTitle1: NSTextField!
    @IBOutlet weak var subTitle2: NSTextField!
    @IBOutlet weak var subTitle3: NSTextField!
    @IBOutlet weak var cylindersInfo: NSTextField!
    @IBOutlet weak var headsInfo: NSTextField!
    @IBOutlet weak var sectorsInfo: NSTextField!
    @IBOutlet weak var blocksInfo: NSTextField!
    @IBOutlet weak var bsizeInfo: NSTextField!
    @IBOutlet weak var capacityInfo: NSTextField!

    @IBOutlet weak var cylinderField: NSTextField!
    @IBOutlet weak var cylinderStepper: NSStepper!
    @IBOutlet weak var headField: NSTextField!
    @IBOutlet weak var headStepper: NSStepper!
    @IBOutlet weak var trackField: NSTextField!
    @IBOutlet weak var trackStepper: NSStepper!
    @IBOutlet weak var sectorField: NSTextField!
    @IBOutlet weak var sectorStepper: NSStepper!
    @IBOutlet weak var blockField: NSTextField!
    @IBOutlet weak var blockStepper: NSStepper!

    @IBOutlet weak var tabView: NSTabView!
    @IBOutlet weak var blockScrollView: NSScrollView!
    @IBOutlet weak var blockView: NSTableView!
    @IBOutlet weak var mfmView: NSScrollView!
    @IBOutlet weak var syncColorButton: NSButton!

    var myDocument: MyDocument { return parent.mydocument! }

    // Title and icon of the info section
    var titleString = ""
    var image: NSImage?

    // Block data provider
    var decoder: DiskFileProxy?

    // MFM data provider
    var drive: FloppyDriveProxy?

    // Drive geometry
    var numCyls: Int { return decoder?.numCyls ?? 0 }
    var numHeads: Int { return decoder?.numHeads ?? 0 }
    var numSectors: Int { return decoder?.numSectors ?? 0 }
    var numTracks: Int { return decoder?.numTracks ?? 0 }
    var numBlocks: Int { return decoder?.numBlocks ?? 0 }

    var upperCyl: Int { return max(numCyls - 1, 0) }
    var upperHead: Int { return max(numHeads - 1, 0) }
    var upperSector: Int { return max(numSectors - 1, 0) }
    var upperTrack: Int { return max(numTracks - 1, 0) }
    var upperBlock: Int { return max(numBlocks - 1, 0) }

    // Current selection
    var currentCyl = 0
    var currentHead = 0
    var currentTrack = 0
    var currentSector = 0
    var currentBlock = 0

    //
    // Starting up
    //

    func show(diskDrive nr: Int) {

        drive = amiga.df(nr)

        titleString = "Floppy Drive DF\(nr)"

        // Run the ADF decoder
        decoder = try? ADFFileProxy.make(with: drive!)

        if decoder == nil {

            // Run the DOS decoder
            decoder = try? IMGFileProxy.make(with: drive!)
        }
        if decoder == nil {

            // Run the extended ADF decoder
            decoder = try? EXTFileProxy.make(with: drive!)
        }

        let protected = drive!.hasProtectedDisk
        image = (decoder as? FloppyFileProxy)?.icon(protected: protected)
        showWindow()
    }

    func show(hardDrive nr: Int) {

        titleString = "Hard Drive HD\(nr)"

        // Run the HDF decoder
        decoder = try? HDFFileProxy.make(with: amiga.hd(nr)!)

        image = NSImage(named: "hdf")!
        showWindow()
    }

    override func sheetWillShow() {

        cylinderStepper.maxValue = .greatestFiniteMagnitude
        headStepper.maxValue = .greatestFiniteMagnitude
        trackStepper.maxValue = .greatestFiniteMagnitude
        sectorStepper.maxValue = .greatestFiniteMagnitude
        blockStepper.maxValue = .greatestFiniteMagnitude

        // Remove the MFM tab if a hard drive is analyzed
        if decoder is HDFFileProxy {
            tabView.removeTabViewItem(tabView.tabViewItem(at: 1))
        }

        update()
    }

    //
    // Updating the displayed information
    //

    func update() {

        updateInfo()
        updateSelection()
        updateMfm()
        blockView.reloadData()
    }

    func updateInfo() {

        icon.image = image
        title.stringValue = titleString

        switch decoder {

        case is HDFFileProxy: subTitle1.stringValue = "Standard Hard Drive"
        case is ADFFileProxy: subTitle1.stringValue = "Amiga Floppy Disk"
        case is IMGFileProxy: subTitle1.stringValue = "PC Disk"
        case is EXTFileProxy: subTitle1.stringValue = "Amiga Floppy Disk (Ext)"

        default:
            subTitle1.stringValue = "Raw MFM stream"
            subTitle2.stringValue = ""
            subTitle3.stringValue = ""
            cylindersInfo.stringValue = "-"
            headsInfo.stringValue = "-"
            sectorsInfo.stringValue = "-"
            capacityInfo.stringValue = ""
            blocksInfo.stringValue = "-"
            bsizeInfo.stringValue = "-"
            return
        }

        if let hdf = decoder as? HDFFileProxy {

            var num = hdf.numPartitions
            subTitle2.stringValue = "\(num) Partition"
            subTitle2.stringValue += num != 1 ? "s" : ""

            if hdf.hasRDB {
                subTitle3.stringValue = "Rigid Disk Block"
            } else {
                subTitle3.stringValue = "No Rigid Disk Block"
            }

            num = hdf.numDrivers
            subTitle3.stringValue += ", " + (num == 0 ? "no" : "\(num)")
            subTitle3.stringValue += " loadable file system"
            subTitle3.stringValue += num != 1 ? "s" : ""
        }
        if let floppy = decoder as? FloppyFileProxy {

            subTitle2.stringValue = floppy.typeInfo + " " + floppy.layoutInfo
            subTitle3.stringValue = ""
        }

        cylindersInfo.integerValue = decoder!.numCyls
        headsInfo.integerValue = decoder!.numHeads
        sectorsInfo.integerValue = decoder!.numSectors
        blocksInfo.integerValue = decoder!.numBlocks
        bsizeInfo.stringValue = "\(decoder!.bsize) Bytes"
        capacityInfo.stringValue = decoder!.describeCapacity
    }

    func updateSelection() {

        cylinderField.stringValue      = String(format: "%d", currentCyl)
        cylinderStepper.integerValue   = currentCyl
        headField.stringValue          = String(format: "%d", currentHead)
        headStepper.integerValue       = currentHead
        trackField.stringValue         = String(format: "%d", currentTrack)
        trackStepper.integerValue      = currentTrack
        sectorField.stringValue        = String(format: "%d", currentSector)
        sectorStepper.integerValue     = currentSector
        blockField.stringValue         = String(format: "%d", currentBlock)
        blockStepper.integerValue      = currentBlock
    }

    func updateMfm() {

        // Only proceed if the MFM view is present
        if tabView.numberOfTabViewItems == 1 { return }

        let size = NSSize(width: 32, height: 32)
        syncColorButton.image = NSImage(color: .warningColor, size: size)

        // Read a whole MFM encoded track
        let mfm = drive?.readTrackBits(currentTrack) ?? ""

        // Search all SYNC sequences (0x4489 + 0x4489)
        let sync = "0100010010001001"
        let indices = mfm.indicesOf(string: sync + sync)

        // Create a text storage
        let storage = NSTextStorage(string: mfm)
        storage.font = NSFont.monospaced(ofSize: 10.0, weight: .semibold)
        storage.foregroundColor = .labelColor

        // Colorize all SYNC sequences
        for index in indices {

            storage.addAttribute(.backgroundColor,
                                 value: NSColor.red,
                                 range: NSRange(location: index, length: 32))
            storage.addAttribute(.foregroundColor,
                                 value: NSColor.white,
                                 range: NSRange(location: index, length: 32))
        }

        // Assign the text storage to the MFM view
        let textView = mfmView.documentView as? NSTextView
        textView!.layoutManager!.replaceTextStorage(storage)
    }

    //
    // Helper methods
    //

    func setCylinder(_ newValue: Int) {

        if newValue != currentCyl {

            let value = newValue.clamped(0, upperCyl)

            currentCyl      = value
            currentTrack    = currentCyl * 2 + currentHead
            currentBlock    = currentTrack * numSectors + currentSector

            update()
        }
    }

    func setHead(_ newValue: Int) {

        if newValue != currentHead {

            let value = newValue.clamped(0, upperHead)

            currentHead     = value
            currentTrack    = currentCyl * 2 + currentHead
            currentBlock    = currentTrack * numSectors + currentSector

            update()
        }
    }

    func setTrack(_ newValue: Int) {

        if newValue != currentTrack {

            let value = newValue.clamped(0, upperTrack)

            currentTrack    = value
            currentCyl      = currentTrack / 2
            currentHead     = currentTrack % 2
            currentBlock    = currentTrack * numSectors + currentSector

            update()
        }
    }

    func setSector(_ newValue: Int) {

        if newValue != currentSector {

            let value = newValue.clamped(0, upperSector)

            currentSector   = value
            currentBlock    = currentTrack * numSectors + currentSector

            update()
        }
    }

    func setBlock(_ newValue: Int) {

        if newValue != currentBlock {

            let value = newValue.clamped(0, upperBlock)

            currentBlock    = value
            currentTrack    = currentBlock / numSectors
            currentSector   = currentBlock % numSectors
            currentCyl      = currentTrack / 2
            currentHead     = currentTrack % 2

            update()
        }
    }

    //
    // Action methods
    //

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
}

//
// Extensions
//

extension DiskInspector: NSTableViewDataSource {

    func columnNr(_ column: NSTableColumn?) -> Int? {

        return column == nil ? nil : Int(column!.identifier.rawValue)
    }

    func numberOfRows(in tableView: NSTableView) -> Int {

        return 512 / 16
    }

    func tableView(_ tableView: NSTableView,
                   objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {

        switch tableColumn?.identifier.rawValue {

        case "Offset":
            return String(format: "%X", row)

        case "Ascii":
            return decoder?.asciidump(currentBlock, offset: row * 16, len: 16) ?? ""

        default:
            let col = columnNr(tableColumn)!
            let byte = decoder!.readByte(currentBlock, offset: 16 * row + col)
            return String(format: "%02X", byte)
        }
    }
*/
}

extension DiskInspector: NSTableViewDelegate {

    func tableView(_ tableView: NSTableView, shouldSelectRow row: Int) -> Bool {
        return false
    }
}

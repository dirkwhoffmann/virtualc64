// -----------------------------------------------------------------------------
// This file is part of VirtuaC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import Darwin

class VolumeInspector: DialogController {

    @IBOutlet weak var icon: NSImageView!
    @IBOutlet weak var title: NSTextField!
    @IBOutlet weak var nameInfo: NSTextField!
    @IBOutlet weak var idInfo: NSTextField!
    @IBOutlet weak var dosInfo: NSTextField!
    @IBOutlet weak var capacityInfo: NSTextField!
    @IBOutlet weak var blocksInfo: NSTextField!
    @IBOutlet weak var usageInfo: NSTextField!

    @IBOutlet weak var blockImageButton: NSButton!
    @IBOutlet weak var blockSlider: NSSlider!
    @IBOutlet weak var bamButton: NSButton!
    @IBOutlet weak var dirButton: NSButton!
    @IBOutlet weak var dataButton: NSButton!

    @IBOutlet weak var diagnoseImageButton: NSButton!
    @IBOutlet weak var diagnoseSlider: NSSlider!
    @IBOutlet weak var diagnoseInfo: NSTextField!
    @IBOutlet weak var diagnosePassButton: NSButton!
    @IBOutlet weak var diagnoseFailButton: NSButton!
    @IBOutlet weak var diagnoseNextButton: NSButton!
    @IBOutlet weak var diagnoseNextInfo: NSTextField!
    @IBOutlet weak var diagnoseStrictButton: NSButton!

    @IBOutlet weak var previewScrollView: NSScrollView!
    @IBOutlet weak var previewTable: NSTableView!
    @IBOutlet weak var blockField: NSTextField!
    @IBOutlet weak var blockStepper: NSStepper!
    @IBOutlet weak var info1: NSTextField!
    @IBOutlet weak var info2: NSTextField!

    struct Palette {

        static let white = NSColor.white
        static let gray = NSColor.gray
        static let black = NSColor.black
        static let red = NSColor(r: 0xff, g: 0x66, b: 0x66, a: 0xff)
        static let orange = NSColor(r: 0xff, g: 0xb2, b: 0x66, a: 0xff)
        static let yellow = NSColor(r: 0xff, g: 0xff, b: 0x66, a: 0xff)
        static let green = NSColor(r: 0x66, g: 0xff, b: 0x66, a: 0xff)
        static let cyan = NSColor(r: 0x66, g: 0xff, b: 0xff, a: 0xff)
        static let blue = NSColor(r: 0x66, g: 0xb2, b: 0xff, a: 0xff)
        static let purple = NSColor(r: 0xb2, g: 0x66, b: 0xff, a: 0xff)
        static let pink = NSColor(r: 0xff, g: 0x66, b: 0xff, a: 0xff)
    }

    var myDocument: MyDocument { return parent.mydocument! }

    // The analyzed file system
    var vol: FileSystemProxy!

    // Result of the consistency checker
    var errorReport: FSErrorReport?

    var selection: Int?
    var selectedRow: Int? { return selection == nil ? nil : selection! / 16 }
    var selectedCol: Int? { return selection == nil ? nil : selection! % 16 }
    var strict: Bool { return diagnoseStrictButton.state == .on }

    // Block preview
    var blockNr = 0

    let palette: [FSBlockType: NSColor] = [

        .UNKNOWN: Palette.white,
        .EMPTY: Palette.gray,
        .BAM: Palette.red,
        .DIR: Palette.yellow,
        .DATA: Palette.green
    ]

    var layoutImage: NSImage? {

        return createImage(colorize: { (x: Int) -> NSColor in
            return palette[vol.getDisplayType(x)]!
        })
    }

    var diagnoseImage: NSImage? {

        return createImage(colorize: { (x: Int) -> NSColor in
            switch vol.diagnoseImageSlice(x) {
            case 0: return Palette.gray
            case 1: return Palette.green
            case 2: return Palette.red
            default: fatalError()
            }
        })
    }

    func createImage(colorize: (Int) -> NSColor) -> NSImage? {

        // Create image representation in memory
        let width = 1760
        let height = 16
        let size = CGSize(width: width, height: height)
        let cap = Int(size.width) * Int(size.height)
        let mask = calloc(cap, MemoryLayout<UInt32>.size)!
        let ptr = mask.bindMemory(to: UInt32.self, capacity: cap)

        // Create image data
        for x in 0..<width {

            // let color = colors[vol.getDisplayType(x)]!
            let color = colorize(x)
            let ciColor = CIColor(color: color)!

            for y in 0...height-1 {

                var r, g, b, a: Int

                r = Int(ciColor.red * CGFloat(255 - 2*y))
                g = Int(ciColor.green * CGFloat(255 - 2*y))
                b = Int(ciColor.blue * CGFloat(255 - 2*y))
                a = Int(ciColor.alpha)

                let abgr = UInt32(r | g << 8 | b << 16 | a << 24)
                ptr[y*width + x] = abgr
            }
        }

        // Create image
        let image = NSImage.make(data: mask, rect: size)
        let resizedImage = image?.resizeSharp(width: CGFloat(width), height: CGFloat(height))
        return resizedImage
    }

    //
    // Starting up
    //

    func show(diskDrive nr: DriveID) throws {

        let drive = c64.drive(nr)
        vol = try FileSystemProxy.make(withDisk: drive.disk)

        showWindow()
    }

    func showSheet(fs: FileSystemProxy) {

        vol = fs
        showWindow()
    }

    override func windowDidLoad() {

    }

    override func sheetWillShow() {

        // Register to receive mouse click events
        previewTable.action = #selector(clickAction(_:))

        // Configure elements
        blockStepper.maxValue = .greatestFiniteMagnitude
        blockSlider.minValue = 0
        blockSlider.maxValue = Double(vol.numBlocks - 1)
        diagnoseSlider.minValue = 0
        diagnoseSlider.maxValue = Double(vol.numBlocks - 1)

        // Run a file system check
        errorReport = vol.check(strict)

        // Compute images
        updateLayoutImage()
        updateDiagnoseImage()

        update()
    }

    override func sheetDidShow() {

    }

    //
    // Updating the displayed information
    //

    func update() {

        updateVolumeInfo()
        updateDiagnoseInfo()

        // Update elements
        blockField.stringValue         = String(format: "%d", blockNr)
        blockStepper.integerValue      = blockNr
        blockSlider.integerValue       = blockNr
        diagnoseSlider.integerValue    = blockNr

        // Update the block view table
        updateBlockInfo()
        previewTable.reloadData()
    }

    func updateLayoutImage() {

        let size = NSSize(width: 16, height: 16)
        bamButton.image = NSImage(color: palette[.BAM]!, size: size)
        dirButton.image = NSImage(color: palette[.DIR]!, size: size)
        dataButton.image = NSImage(color: palette[.DATA]!, size: size)
        blockImageButton.image = layoutImage
    }

    func updateDiagnoseImage() {

        let size = NSSize(width: 16, height: 16)
        diagnosePassButton.image = NSImage(color: Palette.green, size: size)
        diagnoseFailButton.image = NSImage(color: Palette.red, size: size)
        diagnoseImageButton.image = diagnoseImage
    }

    func updateVolumeInfo() {

        title.stringValue = vol.dos.description
        nameInfo.stringValue = vol.name
        idInfo.stringValue = vol.idString
        dosInfo.stringValue = vol.dos == .CBM ? "CBM" : "Unknown"
        capacityInfo.stringValue = vol.capacityString
        blocksInfo.integerValue = vol.numBlocks
        usageInfo.stringValue = "\(vol.usedBlocks) (" + vol.fillLevelString + ")"
    }

    func updateDiagnoseInfo() {

        let total = errorReport?.corruptedBlocks ?? 0

        if total > 0 {

            let blocks = total == 1 ? "block" : "blocks"
            diagnoseInfo.stringValue = "\(total) corrupted \(blocks) found"
        }

        diagnoseInfo.isHidden = total == 0
        diagnoseNextInfo.isHidden = total == 0
        diagnoseNextButton.isHidden = total == 0
    }

    func updateBlockInfo() {

        if selection == nil {
            updateBlockInfoUnselected()
            updateErrorInfoUnselected()
        } else {
            updateBlockInfoSelected()
            updateErrorInfoSelected()
        }
    }

    func updateBlockInfoUnselected() {

        let type = vol.blockType(blockNr)
        info1.stringValue = type.description
    }

    func updateBlockInfoSelected() {

        let usage = vol.itemType(blockNr, pos: selection!)
        info1.stringValue = usage.description
    }

    func updateErrorInfoUnselected() {

        info2.stringValue = ""
    }

    func updateErrorInfoSelected() {

        var exp = UInt8(0)
        let error = vol.check(blockNr, pos: selection!, expected: &exp, strict: strict)
        info2.stringValue = error.description(expected: Int(exp))
    }

    //
    // Helper methods
    //

    func setBlock(_ newValue: Int) {

        if newValue != blockNr {

            blockNr = newValue.clamped(0, vol.numBlocks - 1)
            selection = nil
            update()
        }
    }

    //
    // Action methods
    //

    @IBAction func sliderAction(_ sender: NSSlider!) {

        setBlock(sender.integerValue)
    }

    @IBAction func blockTypeAction(_ sender: NSButton!) {

        let type = FSBlockType(rawValue: sender.tag)!

        // Goto the next block of the requested type
        let nextBlock = vol.nextBlock(of: type, after: blockNr)
        if nextBlock != -1 { setBlock(nextBlock) }
    }

    @IBAction func blockAction(_ sender: NSTextField!) {

        setBlock(sender.integerValue)
    }

    @IBAction func blockStepperAction(_ sender: NSStepper!) {

        setBlock(sender.integerValue)
    }

    @IBAction func gotoNextCorruptedBlockAction(_ sender: NSButton!) {

        let nextBlock = vol.nextCorruptedBlock(blockNr)
        if nextBlock != -1 { setBlock(nextBlock) }
    }

    @IBAction func strictAction(_ sender: NSButton!) {

        errorReport = vol.check(strict)
        updateDiagnoseImage()
        update()
    }

    @IBAction func clickAction(_ sender: NSTableView!) {

        if sender.clickedColumn >= 1 && sender.clickedRow >= 0 {

            let newValue = 16 * sender.clickedRow + sender.clickedColumn - 1
            selection = selection != newValue ? newValue : nil
            update()
        }
    }
}

extension VolumeInspector: NSTableViewDataSource {

    func columnNr(_ column: NSTableColumn?) -> Int? {

        return column == nil ? nil : Int(column!.identifier.rawValue)
    }

    func numberOfRows(in tableView: NSTableView) -> Int {

        return 512 / 16
    }

    /*

    func tableView(_ tableView: NSTableView,
                   objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {

        switch tableColumn?.identifier.rawValue {

        case "Offset":
            return String(format: "%X", row)

        case "Ascii":
            return vol.ascii(blockNr, offset: 16 * row, length: 16)

        default:
            if let col = columnNr(tableColumn) {

                let byte = vol.readByte(blockNr, offset: 16 * row + col)
                return String(format: "%02X", byte)
            }
        }
        fatalError()
    }
    */
}

extension VolumeInspector: NSTableViewDelegate {

    func tableView(_ tableView: NSTableView, willDisplayCell cell: Any, for tableColumn: NSTableColumn?, row: Int) {

        var exp = UInt8(0)
        let cell = cell as? NSTextFieldCell

        if let col = columnNr(tableColumn) {

            let offset = 16 * row + col
            let error = vol.check(blockNr, pos: offset, expected: &exp, strict: strict)

            if row == selectedRow && col == selectedCol {
                cell?.textColor = .white
                cell?.backgroundColor = error == .OK ? .selectedContentBackgroundColor : .warningColor
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

/*
extension VolumeInspector: NSTabViewDelegate {

    func tabView(_ tabView: NSTabView, didSelect tabViewItem: NSTabViewItem?) {

        update()
    }
}
*/

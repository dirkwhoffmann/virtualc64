// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class DiskInspectorController: DialogController {
    
    let diskImage = NSImage.init(named: "inspect_disk")
    let noDiskImage = NSImage.init(named: "nodisk")
    let monoFont = NSFont.monospacedDigitSystemFont(ofSize: 11.0, weight: .medium)
    let monoLarge = NSFont.monospacedDigitSystemFont(ofSize: 13.0, weight: .medium)

    // Number of the currently analyzed drive
    var driveNr = 1

    // Proxy of the currently analyzed drive
    var drive: DriveProxy!
    
    // Number of the currently displayed halftrack
    var halftrack = UInt32.max

    // The currently displayed head position
    var offset = Int.max
    
    // Remembers the disk insertion status
    var hasDisk = false
    
    // Remembers the drive motor status
    var spinning = false
    
    // Indicates if the disk metadata stuff needs an update
    var diskInfoIsDirty = true
    
    // Indicates if the two data views need an update
    var trackDataIsDirty = true

    // Indicates if the head position stuff needs an update
    var headPositionIsDirty = true
    
    // Highlighted head position in the GCR view
    var headPosition: NSRange? // NSRange.init(location: 0, length: 0)

    // First highlighted bit sequence in the GCR view
    var firstSectorRange: NSRange?

    // Second highlighted bit sequence in the GCR view
    var secondSectorRange: NSRange?

    // Maps table row numbers to sector numbers
    var sectorForRow: [Int: Int] = [:]
    
    // Outlets
    @IBOutlet weak var icon: NSImageView!
    @IBOutlet weak var trackField: NSTextField!
    @IBOutlet weak var halftrackField: NSTextField!
    @IBOutlet weak var headField: NSTextField!
    @IBOutlet weak var valueField: NSTextField!
    @IBOutlet weak var gcrBox: NSBox!
    @IBOutlet weak var gcrView: NSScrollView!
    @IBOutlet weak var sectorView: NSTableView!
    @IBOutlet weak var errorView: NSTableView!
    @IBOutlet weak var drive8: NSButton!
    @IBOutlet weak var drive9: NSButton!

    override public func awakeFromNib() {
        
        driveNr = 1
        drive = proxy!.drive8
    
        // Start receiving messages
        addListener()

        refresh()
    }

    func addListener() {
        
        track()
        
        // Convert 'self' to a void pointer
        let myself = UnsafeRawPointer(Unmanaged.passUnretained(self).toOpaque())
        
        proxy?.addListener(myself) { (ptr, type, data) in
            
            // Convert void pointer back to 'self'
            let myself = Unmanaged<DiskInspectorController>.fromOpaque(ptr!).takeUnretainedValue()
            
            // Process message in the main thread
            DispatchQueue.main.async {
                let mType = MessageType(rawValue: UInt32(type))
                myself.processMessage(Message(type: mType, data: data))
            }
        }
        
        track("Listener is in place")
    }
    
    /// Updates dirty GUI elements
    func refresh() {
        
        drive8.state = (driveNr == DRIVE8.rawValue) ? .on : .off
        drive9.state = (driveNr == DRIVE9.rawValue) ? .on : .off

        // Enable or disable user edition
        if drive.hasDisk() {
            headField.isEnabled = !drive.isRotating()
            valueField.isEnabled = !drive.isRotating()
        } else {
            headField.isEnabled = false
            valueField.isEnabled = false
        }
        
        // Determine the GUI elements that need an update
        if drive.hasDisk() != hasDisk {
            diskInfoIsDirty = true
            trackDataIsDirty = true
            headPositionIsDirty = true
            hasDisk = drive.hasDisk()
        }
        if drive.halftrack() != halftrack {
            trackDataIsDirty = true
            headPositionIsDirty = true
            halftrack = drive.halftrack()
        }
        if drive.offset() != offset {
            headPositionIsDirty = true
            offset = Int(drive.offset())
        }
        
        // Update GUI elements
        if diskInfoIsDirty {
            if hasDisk {
                icon.image = diskImage
            } else {
                icon.image = noDiskImage
            }
            diskInfoIsDirty = false
        }
        
        if trackDataIsDirty {
            halftrackField.integerValue = Int(halftrack)
            trackField.doubleValue = Double(halftrack + 1) / 2.0
            drive.disk.analyzeHalftrack(halftrack)
            sectorForRow = [:]
            var row = 0
            for i in 0 ... Int(maxNumberOfSectors - 1) {
                let info = drive.disk.sectorInfo(Sector(i))
                if info.headerBegin != info.headerEnd {
                    sectorForRow[row] = i
                    row += 1
                }
            }
            refreshPhysicalView()
            refreshLogicalView()
            singleClickAction(sectorView)
            trackDataIsDirty = false
        }
        
        if headPositionIsDirty {
            if hasDisk {
                headField.integerValue = Int(drive.offset())
                valueField.integerValue = Int(drive.readBitFromHead())
            } else {
                headField.stringValue = ""
                valueField.stringValue = ""
            }
            headPositionIsDirty = false
        }
    }
    
    func refreshPhysicalView() {
        
        var gcr: String
        
        if hasDisk {
            gcrBox.title = "GCR Bitstream (\(drive.sizeOfCurrentHalftrack()) Bits)"
            gcr = String(cString: drive.disk.trackBitsAsString())
        } else {
            gcrBox.title = "GCR Bitstream"
            gcr = ""
        }
        
        let textStorage = NSTextStorage.init(string: gcr)
        textStorage.font = NSFont.monospacedDigitSystemFont(ofSize: 10.0, weight: .medium)
        textStorage.foregroundColor = .textColor
        let documentView = gcrView.documentView as? NSTextView
        documentView?.layoutManager?.replaceTextStorage(textStorage)
    }

    func refreshLogicalView() {
        sectorView.reloadData()
        errorView.reloadData()
    }
    
    func removeHeadMarker() {
        
        let storage = (gcrView.documentView as? NSTextView)?.textStorage
        if headPosition != nil {
            storage?.removeAttribute(.backgroundColor, range: headPosition!)
            headPosition = nil
        }
    }
    
    func setHeadMarker() {
        
        removeHeadMarker()
        let storage = (gcrView.documentView as? NSTextView)?.textStorage
        headPosition = NSRange.init(location: Int(drive.offset()), length: 1)
        storage?.addAttribute(.backgroundColor, value: NSColor.red, range: headPosition!)
    }
    
    func scrollToHead() {
        
        let range = NSRange.init(location: offset, length: 1)
        let view = gcrView.documentView as? NSTextView
        view?.scrollRangeToVisible(range)
    }
    
    func removeSectorMarkers() {
        
        let storage = (gcrView.documentView as? NSTextView)?.textStorage
        if firstSectorRange != nil {
            storage?.removeAttribute(.foregroundColor, range: firstSectorRange!)
            firstSectorRange = nil
        }
        if secondSectorRange != nil {
            storage?.removeAttribute(.foregroundColor, range: secondSectorRange!)
            secondSectorRange = nil
        }
    }
    
    func setSectorMarkers(begin: Int, end: Int) {
        
        let length = Int(drive.sizeOfCurrentHalftrack())
        if length == 0 { return }
        
        let left = begin % (length + 1)
        let right = end % (length + 1)
        
        // track("left = \(left) right = \(right)")
        
        removeSectorMarkers()
        
        if left < right {
            firstSectorRange = NSRange.init(location: left, length: right - left)
            secondSectorRange = nil
        } else if left > right {
            firstSectorRange = NSRange.init(location: 0, length: right + 1)
            secondSectorRange = NSRange.init(location: left, length: length - left)
        } else {
            firstSectorRange = nil
            secondSectorRange = nil
        }
    
        let color = NSColor.alternateSelectedControlColor
        let storage = (gcrView.documentView as? NSTextView)?.textStorage
        if firstSectorRange != nil {
            storage?.addAttribute(.foregroundColor, value: color, range: firstSectorRange!)
        }
        if secondSectorRange != nil {
            storage?.addAttribute(.foregroundColor, value: color, range: secondSectorRange!)
        }
    }
    
    func scrollToFirstSectorMarker() {
        
        let view = gcrView.documentView as? NSTextView
        if firstSectorRange != nil {
            view?.scrollRangeToVisible(firstSectorRange!)
        }
    }

    //
    // Action methods
    //

    @IBAction func selectDriveAction(_ sender: NSButton!) {
    
        drive = proxy!.drive(DriveID(sender.tag))
        diskInfoIsDirty = true
        trackDataIsDirty = true
        headPositionIsDirty = true
        refresh()
    }
    
    func _trackAction(_ t: Track) {
        
        let track = (t > maxNumberOfTracks) ? maxNumberOfTracks : (t < 1) ? 1 : t
        drive.setTrack(track)
        trackDataIsDirty = true
        refresh()
        scrollToFirstSectorMarker()
    }
    
    @IBAction func trackAction(_ sender: NSTextField!) {
        
        let ht = sender.doubleValue * 2 - 1
        _halftrackAction(Halftrack(ht))
    }

    @IBAction func trackStepperAction(_ sender: NSStepper!) {
        
        let value = sender.integerValue
        // let t = Int(c64.vc1541.halftrack() + 1 / 2) + (value == 1 ? 1 : -1)
        let t = (Int(drive.halftrack()) + (value == 1 ? 2 : -1) + 1) / 2
        _trackAction(Track(t))
    }
    
    func _halftrackAction(_ ht: Halftrack) {
        
        let htrack = (ht > maxNumberOfHalftracks) ? maxNumberOfHalftracks : (ht < 1) ? 1 : ht
        drive.setHalftrack(htrack)
        trackDataIsDirty = true
        refresh()
        scrollToFirstSectorMarker()
    }
    
    @IBAction func halftrackAction(_ sender: NSTextField!) {
        
        let ht = sender.integerValue
        _halftrackAction(Halftrack(ht))
    }
    
    @IBAction func halftrackStepperAction(_ sender: NSStepper!) {
        
        let value = sender.integerValue
        let ht = Int(drive.halftrack()) + (value == 1 ? 1 : -1)
        _halftrackAction(Halftrack(ht))
    }
    
    @IBAction func headAction(_ sender: NSTextField!) {
        
        var value = sender.integerValue
        let maxValue = Int(drive.sizeOfCurrentHalftrack())
        if value >= maxValue { value = maxValue - 1 }
        if value < 0 { value = 0 }
        
        drive.setOffset(UInt16(value))
        refresh()
        scrollToHead()
    }
    
    @IBAction func headStepperAction(_ sender: NSStepper!) {
        
        let value = sender.integerValue
        if value == 1 {
            drive.rotateDisk()
        } else {
            drive.rotateBack()
        }
        refresh()
        scrollToHead()
    }

    @IBAction func valueAction(_ sender: NSTextField!) {
        
        let value = sender.integerValue
        drive.writeBit(toHead: UInt8(value))
        trackDataIsDirty = true
        refresh()
        scrollToHead()
    }
 
    @IBAction func valueStepperAction(_ sender: Any!) {
        
        let value = drive.readBitFromHead()
        valueField.integerValue = (value == 0) ? 1 : 0
        valueAction(valueField)
    }
    
    @IBAction func markHeadAction(_ sender: NSButton!) {
        
        if sender.integerValue == 1 {
            setHeadMarker()
        } else {
            removeHeadMarker()
        }
        scrollToHead()
    }
    
    @IBAction func singleClickAction(_ sender: NSTableView!) {
        
        var begin = 0
        var end = 0
        
        let row = sender.selectedRow
        
        if sender == sectorView {
            
            let sector = row / 2
            let info = drive.disk.sectorInfo(Sector(sector))
            begin = (row % 2 == 0) ? info.headerBegin : info.dataBegin
            end = (row % 2 == 0) ? info.headerEnd : info.dataEnd
            
        } else if sender == errorView {
            
            begin = (row > 0) ? drive.disk.firstErroneousBit(row - 1) : 0
            end = (row > 0) ? drive.disk.lastErroneousBit(row - 1) : 0
            
        } else {
            
            assert(false)
        }
        
        setSectorMarkers(begin: begin, end: end)
        scrollToFirstSectorMarker()
    }
    
    override func cancelAction(_ sender: Any!) {
        
        // Stop receiving messages
        let myself = UnsafeRawPointer(Unmanaged.passUnretained(self).toOpaque())
        proxy?.removeListener(myself)
        
        super.cancelAction(self)
    }
}

extension DiskInspectorController: MessageReceiver {

    func processMessage(_ msg: Message) {
        
        switch msg.type {
            
        case MSG_DRIVE_CONNECT,
             MSG_DRIVE_DISCONNECT,
             MSG_DRIVE_DISK,
             MSG_DRIVE_NO_DISK,
             MSG_DISK_SAVED,
             MSG_DISK_UNSAVED,
             MSG_DRIVE_MOTOR_ON,
             MSG_DRIVE_MOTOR_OFF,
             MSG_DRIVE_HEAD:
            refresh()
            
        default:
            return
        }
    }
}

extension DiskInspectorController: NSTableViewDataSource {
    
    func numberOfRows(in tableView: NSTableView) -> Int {
        
        if tableView == sectorView {
            return 2 * sectorForRow.count
        } else if tableView == errorView {
            return hasDisk ? drive.disk.numErrors() + 1 : 0
        } else {
            assert(false)
            return 0
        }
    }
    
    func tableView(_ tableView: NSTableView, objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        
        if tableView == sectorView {
            
        guard let sectorNr = sectorForRow[row / 2] else { return nil }
        let headerRow = (row % 2) == 0

            switch tableColumn?.identifier.rawValue {
                
            case "sector":
                return headerRow ? sectorNr : ""
                
            case "type":
                return headerRow ? "Header:" : "Data:"
                
            case "data":
                if headerRow {
                    let cStr = drive.disk.sectorHeaderBytes(asString: Sector(sectorNr))!
                    return String.init(cString: cStr)
                    
                } else {
                    let cStr = drive.disk.sectorDataBytes(asString: Sector(sectorNr))!
                    return String.init(cString: cStr)
                }
                
            default:
                return "???"
            }
            
        } else if tableView == errorView {
            
            if row == 0 {
                let count = drive.disk.numErrors()
                if count == 0 {
                    return "The GCR bitstream has been scanned without errrors."
                } else {
                    let s = (count == 1) ? "" : "s"
                    return "The GCR bitstream contains \(count) error\(s)."
                }
            } else {
                return drive.disk.errorMessage(row - 1)
            }
   
        } else {
            assert(false)
            return nil
        }
    }
}

extension DiskInspectorController: NSTableViewDelegate {
    
    func tableView(_ tableView: NSTableView, willDisplayCell cell: Any, for tableColumn: NSTableColumn?, row: Int) {
        
        let cell = cell as? NSTextFieldCell
        
        if tableView == errorView {
            cell?.textColor = (row == 0) ? .textColor : .red
        }
    }
}

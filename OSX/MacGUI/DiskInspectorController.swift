//
//  DiskInspectorController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 13.02.18.
//

import Foundation

class DiskInspectorController : UserDialogController {
    
    let diskImage = NSImage.init(named: NSImage.Name.init(rawValue: "diskette"))
    let noDiskImage = NSImage.init(named: NSImage.Name.init(rawValue: "diskette_light"))
    let monoFont = NSFont.monospacedDigitSystemFont(ofSize: 11.0, weight: .medium)
    let monoLarge = NSFont.monospacedDigitSystemFont(ofSize: 13.0, weight: .medium)

    // Remembers the currently displayed halftrack number
    var halftrack = UInt32.max

    // Remebers the currently displayed head position
    var offset = Int.max
    
    // Remembers if disk insertion status
    var hasDisk = false
    
    // Remembers the drive motor status
    var spinning = false
    
    // Indicates if the disk metadata stuff needs an update
    var diskInfoIsDirty = true
    
    // Indicates if the two data views need an update
    var trackDataIsDirty = true

    // Indicates if the head position stuff needs an update
    var headPositionIsDirty = true
    
    // Highlighted head position in GCR view
    var headPosition = NSRange.init(location: 0, length: 0)

    // Highlighted sector data in GCR view
    var sectorRange = NSRange.init(location: 0, length: 0)

    // Maps table row numbers to sector numbers
    var sectorForRow: [Int:Int] = [:]

    // Refresh timer
    var timer: Timer!
    
    // Outlets
    @IBOutlet weak var trackField: NSTextField!
    @IBOutlet weak var halftrackField: NSTextField!
    @IBOutlet weak var headField: NSTextField!
    @IBOutlet weak var valueField: NSTextField!
    @IBOutlet weak var physicalViewHeader: NSTextField!
    @IBOutlet weak var physicalView: NSScrollView!
    @IBOutlet weak var logicalView: NSTableView!
    
    @IBOutlet weak var icon: NSImageView!
    
    override public func awakeFromNib() {
        
        refresh()
        
        // Start refresh timer
        if #available(OSX 10.12, *) {
            timer = Timer.scheduledTimer(withTimeInterval: 0.10,
                                         repeats: true,
                                         block: { (t) in self.refresh() })
        }
    }

    /// Updates dirty GUI elements
    func refresh() {
        
        let drive = c64.vc1541!
        
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
            c64.vc1541.disk.analyzeHalftrack(halftrack)
            sectorForRow = [:]
            var row = 0
            for i in 0 ... Int(maxNumberOfSectors - 1) {
                let info = c64.vc1541.disk.sectorInfo(Sector(i))
                if (info.headerBegin != info.headerEnd) {
                    sectorForRow[row] = i
                    row += 1
                }
            }
            refreshPhysicalView()
            refreshLogicalView()
            singleClickAction(logicalView)
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
        
        let drive = c64.vc1541!
        var gcr : String
        
        if hasDisk {
            physicalViewHeader.stringValue = "\(drive.sizeOfCurrentHalftrack()) Bits"
            gcr = String(cString: drive.disk.trackDataAsString())
        } else {
            physicalViewHeader.stringValue = ""
            gcr = ""
        }
        
        let textStorage = NSTextStorage.init(string: gcr)
        textStorage.font = NSFont.monospacedDigitSystemFont(ofSize: 10.0, weight: .medium)
        let documentView = physicalView.documentView as? NSTextView
        documentView?.layoutManager?.replaceTextStorage(textStorage)
    }

    func refreshLogicalView() {        
        logicalView.reloadData()
    }
    
    func removeHeadMarker() {
        
        let storage = (physicalView.documentView as! NSTextView).textStorage
        storage?.removeAttribute(.backgroundColor, range: headPosition)
        headPosition = NSRange.init(location: 0, length: 0)
    }
    
    func setHeadMarker() {
        
        removeHeadMarker()
        let storage = (physicalView.documentView as! NSTextView).textStorage
        headPosition = NSRange.init(location: Int(c64.vc1541.offset()), length: 1)
        storage?.addAttribute(.backgroundColor, value: NSColor.red, range: headPosition)
    }
    
    func scrollToHead() {
        
        let range = NSRange.init(location: offset, length: 1)
        let view = physicalView.documentView as! NSTextView
        view.scrollRangeToVisible(range)
    }
    
    func removeSectorMarker() {
        
        let storage = (physicalView.documentView as! NSTextView).textStorage
        storage?.removeAttribute(.foregroundColor, range: sectorRange)
        sectorRange = NSRange.init(location: 0, length: 0)
    }
    
    func setSectorMarker(begin: Int, end: Int) {
        
        removeSectorMarker()
        if end > begin {
            let color = NSColor.alternateSelectedControlColor
            let storage = (physicalView.documentView as! NSTextView).textStorage
            sectorRange = NSRange.init(location: begin, length: end - begin)
            storage?.addAttribute(.foregroundColor, value: color, range: sectorRange)
        }
    }
    
    func scrollToSectorMarker() {
        
        let view = physicalView.documentView as! NSTextView
        view.scrollRangeToVisible(sectorRange)
    }
    
    
    //
    // Action methods
    //

    @IBAction func trackAction(_ sender: Any!) {
        
        var t = (sender as! NSTextField).integerValue
        if t > maxNumberOfTracks { t = Int(maxNumberOfTracks) }
        if t < 1 { t = 1};
        c64.vc1541.setTrack(Track(t))
        refresh()
        scrollToSectorMarker()
    }

    @IBAction func trackStepperAction(_ sender: Any!) {
        
        let value = (sender as! NSStepper).integerValue
        let t = Int((c64.vc1541.halftrack() + 1) / 2)
        trackField.integerValue = t + (value == 1 ? 1 : -1)
        trackAction(trackField)
    }
    
    @IBAction func halftrackAction(_ sender: Any!) {
        
        var ht = (sender as! NSTextField).integerValue
        if ht > maxNumberOfHalftracks { ht = Int(maxNumberOfHalftracks) }
        if ht < 1 { ht = 1};
        c64.vc1541.setHalftrack(Halftrack(ht))
        refresh()
        scrollToSectorMarker()
    }
    
    @IBAction func halftrackStepperAction(_ sender: Any!) {
        
        let value = (sender as! NSStepper).integerValue
        let t = Int(c64.vc1541.halftrack())
        halftrackField.integerValue = t + (value == 1 ? 1 : -1)
        halftrackAction(halftrackField)
    }
    
    @IBAction func headAction(_ sender: Any!) {
        
        var value = (sender as! NSTextField).integerValue
        let maxValue = Int(c64.vc1541.sizeOfCurrentHalftrack())
        if value >= maxValue { value = maxValue - 1 }
        if value < 0 { value = 0 }
        
        c64.vc1541.setOffset(UInt16(value))
        refresh()
        scrollToHead()
    }
    
    @IBAction func headStepperAction(_ sender: Any!) {
        
        let value = (sender as! NSStepper).integerValue
        if value == 1 {
            c64.vc1541.rotateDisk()
        } else {
            c64.vc1541.rotateBack()
        }
        refresh()
        scrollToHead()
    }

    @IBAction func valueAction(_ sender: Any!) {
        
        let value = (sender as! NSTextField).integerValue
        c64.vc1541.writeBit(toHead: UInt8(value))
        trackDataIsDirty = true
        refresh()
        scrollToHead()
    }
 
    @IBAction func valueStepperAction(_ sender: Any!) {
        
        let value = c64.vc1541.readBitFromHead()
        valueField.integerValue = (value == 0) ? 1 : 0
        valueAction(valueField)
    }
    
    @IBAction func markHeadAction(_ sender: Any!) {
        
        if (sender as! NSButton).integerValue == 1 {
            setHeadMarker()
        } else {
            removeHeadMarker()
        }
        scrollToHead()
    }
    
    @IBAction func singleClickAction(_ sender: Any!) {
        
        let sender = sender as! NSTableView
        let sector = sender.selectedRow / 2
        let headerRow = (sender.selectedRow % 2) == 0
        let info = c64.vc1541.disk.sectorInfo(Sector(sector))
        let begin = headerRow ? info.headerBegin : info.dataBegin
        let end = headerRow ? info.headerEnd : info.dataEnd
        setSectorMarker(begin: begin, end: end)
        scrollToSectorMarker()
    }
    
    override func cancelAction(_ sender: Any!) {
        
        track("Canceling timer")
        timer.invalidate()
        super.cancelAction(self)
    }
}

extension DiskInspectorController : NSTableViewDataSource {
    
    func numberOfRows(in tableView: NSTableView) -> Int {
        
        return 2 * sectorForRow.count
    }
    
    func tableView(_ tableView: NSTableView, objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        
        guard let sectorNr = sectorForRow[row / 2] else { return nil }
        let headerRow = (row % 2) == 0

        switch(tableColumn?.identifier.rawValue) {

        case "sector":
            return headerRow ? sectorNr : ""
            
        case "type":
            return headerRow ? "Header:" : "Data:"
            
        case "data":
            if headerRow {
                let cStr = c64.vc1541.disk.sectorHeader(asString: Sector(sectorNr))!
                return String.init(cString: cStr)

            } else {
                let cStr = c64.vc1541.disk.sectorData(asString: Sector(sectorNr))!
                return String.init(cString: cStr)
            }
            
        default:
            return "???"
        }
    }
}



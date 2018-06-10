//
//  DiskInspectorController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 13.02.18.
//

import Foundation

class DiskInspectorController : UserDialogController
{
    let diskImage = NSImage.init(named: NSImage.Name.init(rawValue: "diskette"))
    let noDiskImage = NSImage.init(named: NSImage.Name.init(rawValue: "diskette_light"))
    let monoFont = NSFont.monospacedDigitSystemFont(ofSize: 11.0, weight: .medium)
    let monoLarge = NSFont.monospacedDigitSystemFont(ofSize: 13.0, weight: .medium)

    // Remembers the currently displayed track and offset.
    // These values are used to determine the items that need to be refreshed
    var currentHalftrack = Int.max
    var currentOffset = Int.max
    
    // Highlighted head position in GCR view
    var headPosition = NSRange.init(location: 0, length: 0)

    // Highlighted sector data in GCR view
    var sectorRange = NSRange.init(location: 0, length: 0)
    
    
    // Outlets
    @IBOutlet weak var trackField: NSTextField!
    @IBOutlet weak var halftrackField: NSTextField!
    @IBOutlet weak var headField: NSTextField!
    @IBOutlet weak var valueField: NSTextField!
    @IBOutlet weak var physicalViewHeader: NSTextField!
    @IBOutlet weak var physicalView: NSScrollView!
    @IBOutlet weak var logicalViewHeader: NSTextField!
    @IBOutlet weak var logicalView: NSTableView!
    
    @IBOutlet weak var icon: NSImageView!
    @IBOutlet weak var iconText: NSTextField!
    @IBOutlet weak var iconSubText: NSTextField!
    
    override public func awakeFromNib()
    {
        track()
        refreshDiskInfo()
        refresh()
    }

    /// Updates the disk icon and it's textual representation
    func refreshDiskInfo() {
        
        let drive = c64.vc1541!
        
        if drive.hasDisk() {
            
            headField.isEnabled = true
            valueField.isEnabled = true
            icon.image = diskImage
            DispatchQueue.main.async {
                let tracks = drive.disk.nonemptyHalftracks()
                let d64Disk = D64Proxy.make(withVC1541: drive)
                self.iconText.stringValue = d64Disk?.name() ?? ""
                self.iconSubText.stringValue = "\(tracks) tracks"
            }
            
        } else {
            
            headField.isEnabled = false
            headField.stringValue = ""
            valueField.isEnabled = false
            valueField.stringValue = ""
            physicalViewHeader.stringValue = ""
            logicalViewHeader.stringValue = ""
            icon.image = noDiskImage
            iconText.stringValue = ""
            iconSubText.stringValue = ""
            valueField.stringValue = ""
        }
    }

    /// Updates all GUI elements
    func refresh() {
        
        let drive = c64.vc1541!
        
        let halftrack = Int(drive.halftrack())
        let track = Double(halftrack + 1) / 2.0
        halftrackField.integerValue = halftrack
        trackField.doubleValue = track
        
        if drive.hasDisk() {

            headField.integerValue = Int(drive.bitOffset())
            valueField.integerValue = Int(drive.readBitFromHead())
            
            // Update track info if necessary
            if halftrack != currentHalftrack {
                
                currentHalftrack = Int(halftrack)
                c64.vc1541.disk.analyzeHalftrack(currentHalftrack)
                refreshDataViews()
            }
            
        }
    }
    
    func refreshDataViews() {

        let drive = c64.vc1541!

        // Physical view
        physicalViewHeader.stringValue = "\(drive.sizeOfCurrentHalftrack()) Bits"
        let gcr = String(cString: drive.disk.trackDataAsString())
        let textStorage = NSTextStorage.init(string: gcr)
        textStorage.font = NSFont.monospacedDigitSystemFont(ofSize: 10.0, weight: .medium)
        let documentView = physicalView.documentView as? NSTextView
        documentView?.layoutManager?.replaceTextStorage(textStorage)

        // Logical view
        logicalView.reloadData()
    }
    
    func removeHeadMarker()
    {
        if c64.vc1541.sizeOfCurrentHalftrack() > 0 {
            let storage = (physicalView.documentView as! NSTextView).textStorage
            // storage?.removeAttribute(.foregroundColor, range: headPosition)
            storage?.removeAttribute(.backgroundColor, range: headPosition)
            headPosition = NSRange.init(location: 0, length: 0)
        }
    }
    
    func setHeadMarker()
    {
        if c64.vc1541.sizeOfCurrentHalftrack() > 0 {
            removeHeadMarker()
            let storage = (physicalView.documentView as! NSTextView).textStorage
            headPosition = NSRange.init(location: Int(c64.vc1541.bitOffset()), length: 1)
            // storage?.addAttribute(.foregroundColor, value: NSColor.white, range: headPosition)
            storage?.addAttribute(.backgroundColor, value: NSColor.red, range: headPosition)
            let view = physicalView.documentView as! NSTextView
            view.scrollRangeToVisible(headPosition)
        }
    }
    
    func removeSectorMarker()
    {
        if c64.vc1541.sizeOfCurrentHalftrack() > 0 {
            let storage = (physicalView.documentView as! NSTextView).textStorage
            storage?.removeAttribute(.foregroundColor, range: sectorRange)
            sectorRange = NSRange.init(location: 0, length: 0)
        }
    }
    
    func setSectorMarker(begin: Int, end: Int)
    {
        if c64.vc1541.sizeOfCurrentHalftrack() > 0 {
            removeSectorMarker()
            if end > begin {
                let color = NSColor.alternateSelectedControlColor
                let storage = (physicalView.documentView as! NSTextView).textStorage
                sectorRange = NSRange.init(location: begin, length: end - begin)
                storage?.addAttribute(.foregroundColor, value: color, range: sectorRange)
                let view = physicalView.documentView as! NSTextView
                view.scrollRangeToVisible(sectorRange)
            }
        }
    }
    
    
    //
    // Action methods
    //

    @IBAction func trackAction(_ sender: Any!)
    {
        var t = (sender as! NSTextField).integerValue
        if t > maxNumberOfTracks { t = Int(maxNumberOfTracks) }
        if t < 1 { t = 1};
        c64.vc1541.setTrack(Track(t))
        refresh()
    }

    @IBAction func trackStepperAction(_ sender: Any!)
    {
        let value = (sender as! NSStepper).integerValue
        let t = Int((c64.vc1541.halftrack() + 1) / 2)
        trackField.integerValue = t + (value == 1 ? 1 : -1)
        trackAction(trackField)
    }
    
    @IBAction func halftrackAction(_ sender: Any!)
    {
        var ht = (sender as! NSTextField).integerValue
        if ht > maxNumberOfHalftracks { ht = Int(maxNumberOfHalftracks) }
        if ht < 1 { ht = 1};
        c64.vc1541.setHalftrack(Halftrack(ht))
        refresh()
    }
    
    @IBAction func halftrackStepperAction(_ sender: Any!)
    {
        let value = (sender as! NSStepper).integerValue
        let t = Int(c64.vc1541.halftrack())
        halftrackField.integerValue = t + (value == 1 ? 1 : -1)
        halftrackAction(halftrackField)
    }
    
    @IBAction func headAction(_ sender: Any!)
    {
        var value = (sender as! NSTextField).integerValue
        let maxValue = Int(c64.vc1541.sizeOfCurrentHalftrack())
        if value >= maxValue { value = maxValue - 1 }
        if value < 0 { value = 0 }
        
        c64.vc1541.setBitOffset(UInt16(value))
        refresh()
    }
    
    @IBAction func headStepperAction(_ sender: Any!)
    {
        let value = (sender as! NSStepper).integerValue
        if value == 1 {
            c64.vc1541.rotateDisk()
        } else {
            c64.vc1541.rotateBack()
        }
        refresh()
    }

    @IBAction func valueAction(_ sender: Any!)
    {
        let value = (sender as! NSTextField).integerValue
        c64.vc1541.writeBit(toHead: UInt8(value))
        refreshDataViews()
        refresh()
    }
 
    @IBAction func valueStepperAction(_ sender: Any!)
    {
        let value = c64.vc1541.readBitFromHead()
        valueField.integerValue = (value == 0) ? 1 : 0
        valueAction(valueField)
    }
    
    func scrollToHead()
    {
        let range = NSRange.init(location: Int(c64.vc1541.bitOffset()), length: 1)
        let view = physicalView.documentView as! NSTextView
        view.scrollRangeToVisible(range)
    }

    @IBAction func markHeadAction(_ sender: Any!)
    {
        track()
        
        if (sender as! NSButton).integerValue == 1 {
            setHeadMarker()
        } else {
            removeHeadMarker()
        }
    }
    
    @IBAction func singleClickAction(_ sender: Any!) {
        
        let sender = sender as! NSTableView
        let sector = sender.selectedRow / 2
        let headerRow = (sender.selectedRow % 2) == 0
        
        track("Highlighting sector \(sector)")
        
        let info = c64.vc1541.disk.sectorInfo(sector)
        let begin = headerRow ? info.headerBegin : info.dataBegin
        let end = headerRow ? info.headerEnd : info.dataEnd
        
        setSectorMarker(begin: begin, end: end)
        
        track("Begin = \(begin) End = \(end)")
        
        if headerRow {
            track("Highlighting header from \(info.headerBegin) to \(info.headerEnd)\n")
        } else {
            track("Highlighting data from \(info.dataBegin) to \(info.dataEnd)\n")
        }
        
        c64.vc1541.disk.dump()
    }
}

extension DiskInspectorController : NSTableViewDataSource {
    
    func numberOfRows(in tableView: NSTableView) -> Int {
        return 2 * Int(maxNumberOfSectors)
    }
    
    func tableView(_ tableView: NSTableView, objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        
        let sectorNr = Sector(row / 2)
        let headerRow = (row % 2) == 0

        switch(tableColumn?.identifier.rawValue) {

        case "sector":
            return headerRow ? sectorNr : ""
            
        case "type":
            return headerRow ? "Header:" : "Data:"
            
        case "data":
            if headerRow {
                let cStr = c64.vc1541.disk.sectorHeader(asString: sectorNr)!
                return String.init(cString: cStr)

            } else {
                let cStr = c64.vc1541.disk.sectorData(asString: sectorNr)!
                return String.init(cString: cStr)
            }
            
        default:
            return "???"
        }
    }
}

/*
extension DiskInspectorController : NSTableViewDelegate {
    
    func tableView(_ tableView: NSTableView, willDisplayCell cell: Any, for tableColumn: NSTableColumn?, row: Int) {
        
        let cell = cell as! NSTextFieldCell
        let headerRow = (row % 5 == 0)
        
        if (headerRow) {
            cell.backgroundColor = NSColor.gridColor
        } else {
            cell.backgroundColor = NSColor.white
        }
    }
}
*/


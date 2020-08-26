// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class DiskDataView: NSScrollView {
    
    @IBOutlet weak var inspector: Inspector!
   
    var c64: C64Proxy { return inspector.parent.c64 }
    var drive: DriveProxy { return inspector.drive }
    var halftrack: Int { return inspector.selectedHalftrack }
    var sector: Int { return inspector.selectedSector }
    
    var textView: NSTextView? { return documentView as? NSTextView }
    var storage: NSTextStorage? { return textView?.textStorage }
    
    // Highlighted head position
    var headPosition: NSRange?

    // Highlighted bit sequences (sector header)
    var firstHeaderSectorRange: NSRange?
    var secondHeaderSectorRange: NSRange?
    
    // Highlighted bit sequences (sector data)
    var firstDataSectorRange: NSRange?
    var secondDataSectorRange: NSRange?
    
    // Indicates which elements needs an update
    var dataIsDirty = false
    var sectionMarksAreDirty = false
    
    override func awakeFromNib() {

    }
     
    func refresh(count: Int = 0, full: Bool = false) {
        
        if full {
            
            if dataIsDirty {
                
                var gcr = ""
                if halftrack >= 0 && drive.hasDisk() {

                    if inspector.rawGcr || sector < 0 {
                        
                        // Show the raw GCR stream
                        gcr = String(cString: drive.disk.trackBitsAsString())
                        
                    } else {
                        
                        // Show the decoded GCR data of the currently selected sector
                        gcr = String(cString: drive.disk.sectorHeaderBytes(asString: Sector(sector))!)
                        gcr.append("\n\n")
                        gcr.append(String(cString: drive.disk.sectorDataBytes(asString: Sector(sector))!))
                    }
                }
                
                // Remove old sector markers
                unmarkSectors()
                
                // Update text storage
                let textStorage = NSTextStorage.init(string: gcr)
                if #available(OSX 10.15, *) {
                    textStorage.font = NSFont.monospacedSystemFont(ofSize: 10.0, weight: .semibold)
                } else {
                    textStorage.font = NSFont.monospacedDigitSystemFont(ofSize: 10.0, weight: .semibold)
                }
                textStorage.foregroundColor = .textColor
                textView?.layoutManager?.replaceTextStorage(textStorage)
                
                dataIsDirty = false
            }
            
            if sectionMarksAreDirty {
                
                unmarkSectors()
                markSectors()
                sectionMarksAreDirty = false
            }
        }
    }
    
    func markHead() {
        
        unmarkHead()
        headPosition = NSRange.init(location: Int(drive.offset()), length: 1)
        storage?.addAttr(.backgroundColor, value: NSColor.red, range: headPosition)
    }
    
    func unmarkHead() {
        
        storage?.remAttr(.backgroundColor, range: headPosition)
        headPosition = nil
    }

    func scrollToHead() {
        
        if !drive.hasDisk() { return }
        
        // Jump to current track
        let current = drive.halftrack()
        inspector.selectedHalftrack = Int(current)
        inspector.selectedSector = -1

        // Highlight drive position inside the current track
        let range = NSRange.init(location: Int(drive.offset()), length: 1)
        textView?.scrollRangeToVisible(range)
    }
            
    func markSectors() {
        
        let halftrack = inspector.selectedHalftrack
        let sector = inspector.selectedSector
        if !inspector.rawGcr || sector < 0 || halftrack < 0 { return }

        let length = Int(drive.size(ofHalftrack: Halftrack(halftrack)))
        if length == 0 { return }

        let info = drive.disk.sectorInfo(Sector(sector))
        let hLeft = info.headerBegin % (length + 1)
        let hRight = info.headerEnd % (length + 1)
        let dLeft = info.dataBegin % (length + 1)
        let dRight = info.dataEnd % (length + 1)
                
        if hLeft < hRight {
            firstHeaderSectorRange = NSRange.init(location: hLeft, length: hRight - hLeft)
            secondHeaderSectorRange = nil
        } else if hLeft > hRight {
            firstHeaderSectorRange = NSRange.init(location: 0, length: hRight + 1)
            secondHeaderSectorRange = NSRange.init(location: hLeft, length: length - hLeft)
        } else {
            firstHeaderSectorRange = nil
            secondHeaderSectorRange = nil
        }

        if dLeft < dRight {
            firstDataSectorRange = NSRange.init(location: dLeft, length: dRight - dLeft)
            secondDataSectorRange = nil
        } else if dLeft > dRight {
            firstDataSectorRange = NSRange.init(location: 0, length: dRight + 1)
            secondDataSectorRange = NSRange.init(location: dLeft, length: length - dLeft)
        } else {
            firstDataSectorRange = nil
            secondDataSectorRange = nil
        }

        // Colorize
        let color = NSColor.alternateSelectedControlColor
        storage?.addAttr(.foregroundColor, value: color, range: firstHeaderSectorRange)
        storage?.addAttr(.foregroundColor, value: color, range: secondHeaderSectorRange)
        storage?.addAttr(.foregroundColor, value: color, range: firstDataSectorRange)
        storage?.addAttr(.foregroundColor, value: color, range: secondDataSectorRange)
        
        scrollToFirstMarkedRange()
    }

    func unmarkSectors() {
        
        storage?.remAttr(.foregroundColor, range: firstHeaderSectorRange)
        storage?.remAttr(.foregroundColor, range: secondHeaderSectorRange)
        storage?.remAttr(.foregroundColor, range: firstDataSectorRange)
        storage?.remAttr(.foregroundColor, range: secondDataSectorRange)
        firstHeaderSectorRange = nil
        secondHeaderSectorRange = nil
        firstDataSectorRange = nil
        secondDataSectorRange = nil
    }
    
    func scrollToFirstMarkedRange() {
        
        if firstHeaderSectorRange != nil {
            textView?.scrollRangeToVisible(firstHeaderSectorRange!)
        }
    }
}

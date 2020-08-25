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

    // Data caches

    // var bankCache: [Int: MemoryType] = [:]

    override func awakeFromNib() {

    }
     
    func updateTrackData() {
     
        track("updateTrackData(\(inspector.selectedHalftrack))")

        var gcr = ""

        // Read track data
        if inspector.selectedHalftrack >= 0 && drive.hasDisk() {
            gcr = String(cString: drive.disk.trackDataAsString())
        }
                  
        // Update text storage
        unmarkSectors()
        let textStorage = NSTextStorage.init(string: gcr)
        textStorage.font = NSFont.monospacedDigitSystemFont(ofSize: 10.0, weight: .medium)
        textStorage.foregroundColor = .textColor
        textView?.layoutManager?.replaceTextStorage(textStorage)
        
        updateSectorData()
    }
    
    func updateSectorData() {
        
        track("updateSectorData(\(inspector.selectedSector))")

        unmarkSectors()
        
        if inspector.selectedSector >= 0 {
            markSectors()
        }
    }

    func cache() {
                
    }
    
    func refresh(count: Int = 0, full: Bool = false) {
        
        if full {
        }
    }
    
    func markHead() {
        
        unmarkHead()
        headPosition = NSRange.init(location: Int(drive.offset()), length: 1)
        storage?.addAttribute(.backgroundColor, value: NSColor.red, range: headPosition!)
    }
    
    func unmarkHead() {
        
        if headPosition != nil {
            storage?.removeAttribute(.backgroundColor, range: headPosition!)
            headPosition = nil
        }
    }

    func scrollToHead() {
        
        let range = NSRange.init(location: Int(drive.offset()), length: 1)
        textView?.scrollRangeToVisible(range)
    }
            
    func markSectors() {
        
        let halftrack = inspector.selectedHalftrack
        let sector = inspector.selectedSector
        if sector < 0 || halftrack < 0 { return }

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
        if firstHeaderSectorRange != nil {
            storage?.addAttribute(.foregroundColor, value: color, range: firstHeaderSectorRange!)
        }
        if secondHeaderSectorRange != nil {
            storage?.addAttribute(.foregroundColor, value: color, range: secondHeaderSectorRange!)
        }
        if firstDataSectorRange != nil {
            storage?.addAttribute(.foregroundColor, value: color, range: firstDataSectorRange!)
        }
        if secondDataSectorRange != nil {
            storage?.addAttribute(.foregroundColor, value: color, range: secondDataSectorRange!)
        }
        
        scrollToFirstMarkedRange()
    }

    func unmarkSectors() {
        
        if firstHeaderSectorRange != nil {
            storage?.removeAttribute(.foregroundColor, range: firstHeaderSectorRange!)
            firstHeaderSectorRange = nil
        }
        if secondHeaderSectorRange != nil {
            storage?.removeAttribute(.foregroundColor, range: secondHeaderSectorRange!)
            secondHeaderSectorRange = nil
        }
        if firstDataSectorRange != nil {
            storage?.removeAttribute(.foregroundColor, range: firstDataSectorRange!)
            firstDataSectorRange = nil
        }
        if secondDataSectorRange != nil {
            storage?.removeAttribute(.foregroundColor, range: secondDataSectorRange!)
            secondDataSectorRange = nil
        }
    }
    
    func scrollToFirstMarkedRange() {
        
        if firstHeaderSectorRange != nil {
            textView?.scrollRangeToVisible(firstHeaderSectorRange!)
        }
    }
}

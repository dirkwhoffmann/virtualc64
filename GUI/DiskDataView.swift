// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class DiskDataView: NSScrollView {
    
    @IBOutlet weak var inspector: Inspector!
   
    // Shortcuts
    // var c64: C64Proxy { return inspector.parent.c64 }
    // var drive: DriveProxy { return inspector.drive }
    // var disk: DiskProxy { return drive.disk }
    var analyzer: DiskAnalyzerProxy? { return inspector.analyzer }
    var halftrack: Halftrack? { return inspector.halftrack }
    var sector: Sector? { return inspector.sector }
    var textView: NSTextView? { return documentView as? NSTextView }
    var storage: NSTextStorage? { return textView?.textStorage }
    
    // Highlighted head position
    // var headPosition: NSRange?

    // Highlighted bit sequences (sector header)
    var firstHeaderRange: NSRange?
    var secondHeaderRange: NSRange?
    
    // Highlighted bit sequences (sector data)
    var firstDataSectorRange: NSRange?
    var secondDataSectorRange: NSRange?
    
    // Indicates which elements needs an update
    var dataIsDirty = false
    var sectionMarksAreDirty = false
    
    // Display format in byte view
    var hex = true { didSet { if hex != oldValue { dataIsDirty = true } } }

    // Display font
    var font: NSFont {
        if #available(OSX 10.15, *) {
            return NSFont.monospacedSystemFont(ofSize: 10.0, weight: .semibold)
        } else {
            return NSFont.monospacedDigitSystemFont(ofSize: 10.0, weight: .semibold)
        }
    }
    
    override func awakeFromNib() {

    }
     
    func refresh(count: Int = 0, full: Bool = false) {
        
        if full {
            
            if dataIsDirty {
                
                var gcr = ""
                if analyzer != nil, let ht = halftrack {

                    track("Displaying ht \(ht)")
                    
                    if inspector.rawGcr || sector == nil {
                        
                        // Show the raw GCR stream
                        gcr = String(cString: analyzer!.trackBits(asString: ht))
                        
                    } else {
                        
                        // Show the decoded GCR data of the currently selected sector
                        gcr = String(cString: analyzer!.sectorHeaderBytes(asString: ht, sector: sector!, hex: hex))
                        gcr.append("\n\n")
                        gcr.append(String(cString: analyzer!.sectorDataBytes(asString: ht, sector: sector!, hex: hex)))
                    }
                }
                
                // Remove old sector markers
                unmarkSectors()
                
                // Update text storage
                let textStorage = NSTextStorage(string: gcr)
                textStorage.font = font
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

    func markSectors() {

        // Only proceed if there is anything to display
        if sector == nil || halftrack == nil || analyzer == nil { return }

        // Only proceed if the GCR view is active
        if !inspector.rawGcr { return }

        // Determine the track size
        let length = analyzer?.length(ofHalftrack: halftrack!) ?? 0
        if length == 0 { return }
        
        let info = analyzer!.sectorInfo(halftrack!, sector: sector!)
        let hLeft = info.headerBegin % (length + 1)
        let hRight = info.headerEnd % (length + 1)
        let dLeft = info.dataBegin % (length + 1)
        let dRight = info.dataEnd % (length + 1)
                
        if hLeft < hRight {
            firstHeaderRange = NSRange(location: hLeft, length: hRight - hLeft)
            secondHeaderRange = nil
        } else if hLeft > hRight {
            firstHeaderRange = NSRange(location: 0, length: hRight + 1)
            secondHeaderRange = NSRange(location: hLeft, length: length - hLeft)
        } else {
            firstHeaderRange = nil
            secondHeaderRange = nil
        }

        if dLeft < dRight {
            firstDataSectorRange = NSRange(location: dLeft, length: dRight - dLeft)
            secondDataSectorRange = nil
        } else if dLeft > dRight {
            firstDataSectorRange = NSRange(location: 0, length: dRight + 1)
            secondDataSectorRange = NSRange(location: dLeft, length: length - dLeft)
        } else {
            firstDataSectorRange = nil
            secondDataSectorRange = nil
        }

        // Colorize
        let color = NSColor.alternateSelectedControlColor
        storage?.addAttr(.foregroundColor, value: color, range: firstHeaderRange)
        storage?.addAttr(.foregroundColor, value: color, range: secondHeaderRange)
        storage?.addAttr(.foregroundColor, value: color, range: firstDataSectorRange)
        storage?.addAttr(.foregroundColor, value: color, range: secondDataSectorRange)
        
        scrollToFirstMarkedRange()
    }

    func unmarkSectors() {
        
        storage?.remAttr(.foregroundColor, range: firstHeaderRange)
        storage?.remAttr(.foregroundColor, range: secondHeaderRange)
        storage?.remAttr(.foregroundColor, range: firstDataSectorRange)
        storage?.remAttr(.foregroundColor, range: secondDataSectorRange)
        firstHeaderRange = nil
        secondHeaderRange = nil
        firstDataSectorRange = nil
        secondDataSectorRange = nil
    }
    
    func scrollToFirstMarkedRange() {
        
        if firstHeaderRange != nil {
            textView?.scrollRangeToVisible(firstHeaderRange!)
        }
    }
}

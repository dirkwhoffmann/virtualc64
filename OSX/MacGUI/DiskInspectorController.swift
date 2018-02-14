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
    var headPosition = NSRange.init(location: 0, length: 0)
    
    // Outlets
    @IBOutlet weak var icon: NSImageView!
    @IBOutlet weak var headTrack: NSTextField!
    @IBOutlet weak var headTrackInfo: NSTextField!
    @IBOutlet weak var headOffset: NSTextField!
    @IBOutlet weak var headValue: NSTextField!
    @IBOutlet weak var headLeft: NSTextField!
    @IBOutlet weak var headRight: NSTextField!
    
    @IBOutlet weak var serialData: NSTextField!
    @IBOutlet weak var serialClock: NSTextField!
    @IBOutlet weak var serialAtn: NSTextField!
    
    @IBOutlet weak var gcrHeader: NSTextField!
    @IBOutlet weak var gcrData: NSScrollView!

    @IBOutlet weak var protect: NSButton!

    
    override public func awakeFromNib()
    {
        track()
        
        headLeft.font = monoLarge
        headRight.font = monoLarge
        headValue.font = monoLarge
        
        refresh()
    }
    
    /// Updates all GUI elements
    func refresh() {
        
        track()
        let drive = c64.vc1541!
        let halftrack = drive.halftrack()
        let offset = drive.bitOffset()

        // Update icon and drive protection
        icon.image = drive.hasDisk() ? diskImage : noDiskImage
        protect.integerValue = drive.writeProtection() ? 1 : 0
        
        // Update serial port
        refreshSerial()
    
        // Update track info if necessary
        if (halftrack != currentHalftrack) {
            track("Track has changed")
            refreshTrack()
        }
        
        // Update head info if necessary
        if (halftrack != currentHalftrack || offset != currentOffset) {
            track("Track or Offset has changed")
            refreshHead()
        }
        
        currentHalftrack = halftrack
        currentOffset = offset
    }
    
    func refreshSerial() {
        let iec = c64.iec!
        serialData.stringValue = iec.dataLine() ? "1" : "0"
        serialClock.stringValue = iec.clockLine() ? "1" : "0"
        serialAtn.stringValue = iec.atnLine() ? "1" : "0"
    }
    
    func refreshTrack() {

        let drive = c64.vc1541!
        let halftrack = drive.halftrack()
        let track = (halftrack + 1) / 2
        let count = drive.numberOfBits()
        
        headTrack.integerValue = halftrack
        headTrackInfo.stringValue = "Track \(track)" + ((halftrack % 2 == 0) ? ".5" : "")
        gcrHeader.stringValue = "GCR data: \(count) Bits (\(count / 8) Bytes)"
        
        // Update gcr data view
        let gcr = String(cString: drive.dataAbs(0))
        let textStorage = NSTextStorage.init(string: gcr)
        textStorage.font = NSFont.monospacedDigitSystemFont(ofSize: 10.0, weight: .medium)
        let documentView = gcrData.documentView as? NSTextView
        documentView?.layoutManager?.replaceTextStorage(textStorage)
    }
    
    func refreshHead()
    {
        let drive = c64.vc1541!
        let offset = drive.bitOffset()
        headOffset.integerValue = offset
        
        if drive.hasDisk() {
            headValue.integerValue = drive.readBitFromHead()
            headLeft.stringValue = String(cString: drive.dataRel(-11, length: 10))
            headRight.stringValue = String(cString: drive.dataRel(1, length: 10))
        } else {
            headValue.stringValue = ""
            headLeft.stringValue = ""
            headRight.stringValue = ""
        }
    }
    
    func removeHeadMarker()
    {
        let storage = (gcrData.documentView as! NSTextView).textStorage
        storage?.removeAttribute(.foregroundColor, range: headPosition)
        storage?.removeAttribute(.backgroundColor, range: headPosition)
        headPosition = NSRange.init(location: 0, length: 0)
    }
    
    func setHeadMarker()
    {
        removeHeadMarker()
        let storage = (gcrData.documentView as! NSTextView).textStorage
        headPosition = NSRange.init(location: c64.vc1541.bitOffset(), length: 1)
        storage?.addAttribute(.foregroundColor, value: NSColor.white, range: headPosition)
        storage?.addAttribute(.backgroundColor, value: NSColor.red, range: headPosition)
    }
    
    //
    // Action methods
    //

    @IBAction func halftrackAction(_ sender: Any!)
    {
        let value = (sender as! NSTextField).integerValue
        c64.vc1541.setHalftrack(value)
        refresh()
        markHeadAction(self)
    }

    @IBAction func halftrackStepperAction(_ sender: Any!)
    {
        let value = (sender as! NSStepper).integerValue
        if value == 1 {
            c64.vc1541.moveHeadUp()
        } else {
            c64.vc1541.moveHeadDown()
        }
        refresh()
        markHeadAction(self)
    }
    
    @IBAction func headAction(_ sender: Any!)
    {
        let value = (sender as! NSTextField).integerValue
        c64.vc1541.setBitOffset(value)
        refresh()
        markHeadAction(self)
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
        markHeadAction(self)
    }

    @IBAction func headWheelAction(_ sender: Any!)
    {
        let value = (sender as! NSSlider).integerValue
        let trackSize = c64.vc1541.numberOfBits()
        let newPosition = (trackSize * value) / 1000
        c64.vc1541.setBitOffset(newPosition)
        refresh()
        scrollToHead(self) // markHead action is too slow
    }

    @IBAction func headValueAction(_ sender: Any!)
    {
        let value = (sender as! NSTextField).integerValue
        c64.vc1541.writeBit(toHead: value)
        refreshTrack() // Because data on track might have changed
        refresh()
        markHeadAction(self)
    }
    
    @IBAction func scrollToHead(_ sender: Any!)
    {
        let range = NSRange.init(location: c64.vc1541.bitOffset(), length: 1)
        let view = gcrData.documentView as! NSTextView
        view.scrollRangeToVisible(range)
    }

    @IBAction func markHeadAction(_ sender: Any!)
    {
        setHeadMarker()
        scrollToHead(self)
    }
    
    @IBAction func writeProtectAction(_ sender: Any!)
    {
        track()
        let value = (sender as! NSButton).integerValue
        c64.vc1541.setWriteProtection(value != 0)
        refresh()
    }
}

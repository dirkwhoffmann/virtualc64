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
    
    // Outlets
    @IBOutlet weak var icon: NSImageView!
    @IBOutlet weak var halftrack: NSTextField!
    @IBOutlet weak var head: NSTextField!
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
        refresh()
    }
    
    func refresh() {
        
        track()
        let drive = c64.vc1541!
        let iec = c64.iec!
        let ht = drive.halftrack()
        let t = (ht + 1) / 2
        let offset = drive.bitOffset()

        icon.image = drive.hasDisk() ? diskImage : noDiskImage
        halftrack.integerValue = ht
        head.integerValue = offset
        headValue.integerValue = drive.readBitFromHead()
        headLeft.stringValue = "???"
        headRight.stringValue = "???"
        
        serialData.stringValue = iec.dataLine() ? "1" : "0"
        serialClock.stringValue = iec.clockLine() ? "1" : "0"
        serialAtn.stringValue = iec.atnLine() ? "1" : "0"
        
        if (ht % 2 == 0) {
            let info = String(format: "GCR data of halftrack %d:", ht)
            gcrHeader.stringValue = String(info)
        } else {
            let info = String(format: "GCR data of halftrack %d (track %d):", ht, t)
            gcrHeader.stringValue = String(info)
        }
        
        let gcr = String(cString: drive.trackAsString())
        let documentView = gcrData.documentView as? NSTextView
        let textStorage = NSTextStorage.init(string: gcr)
        documentView?.layoutManager?.replaceTextStorage(textStorage)
    
        
        // gcrData. stringValue = gcr
        // protect.integerValue = drive.writeProtection() ? 1 : 0
    }
    
    //
    // Action methods
    //

    @IBAction func halftrackAction(_ sender: Any!)
    {
        track()
        let value = (sender as! NSTextField).integerValue
        track("\(value)")

        c64.vc1541.setHalftrack(value)
        refresh()
    }

    @IBAction func halftrackStepperAction(_ sender: Any!)
    {
        track()
        let value = (sender as! NSStepper).integerValue
        if value == 1 {
            track("UP")
            c64.vc1541.moveHeadUp()
        } else {
            track("DOWN")
            c64.vc1541.moveHeadDown()
        }
        refresh()
    }
    
    @IBAction func headAction(_ sender: Any!)
    {
        track()
        let value = (sender as! NSTextField).integerValue
        track("\(value)")
        
        c64.vc1541.setBitOffset(value)
        refresh()
    }

    @IBAction func headStepperAction(_ sender: Any!)
    {
        track()
        let value = (sender as! NSStepper).integerValue
        if value == 1 {
            track("UP")
            c64.vc1541.rotateDisk()
        } else {
            track("DOWN")
            c64.vc1541.rotateBack()
        }
        refresh()
    }

    @IBAction func headWheelAction(_ sender: Any!)
    {
        track()
        let value = (sender as! NSSlider).integerValue
        let trackSize = c64.vc1541.numberOfBits()
        let newPosition = (trackSize * value) / 1000
        c64.vc1541.setBitOffset(newPosition)
        refresh()
    }

    @IBAction func headValueAction(_ sender: Any!)
    {
        track()
        let value = (sender as! NSTextField).integerValue
        track("\(value)")
        
        c64.vc1541.writeBit(toHead: value)
        refresh()
    }
    
    @IBAction func writeProtectAction(_ sender: Any!)
    {
        track()
        let value = (sender as! NSButton).integerValue
        c64.vc1541.setWriteProtection(value != 0)
        refresh()
    }
}

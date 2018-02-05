//
//  DiskMountController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 05.02.18.
//

import Foundation

class DiskMountController : MountController {
    
    let bytesPerRow = 16
    var halftrack = 0
    
    // Outlets
    @IBOutlet weak var header: NSTextField!
    @IBOutlet weak var subheader: NSTextField!
    @IBOutlet weak var subsubheader: NSTextField!
    @IBOutlet weak var trackinfo: NSTextField!
    @IBOutlet weak var trackSizeinfo: NSTextField!
    @IBOutlet weak var tableview: NSTableView!
    @IBOutlet weak var ok: NSButton!
    @IBOutlet weak var cancel: NSButton!
    @IBOutlet weak var diskIcon: NSImageView!
    @IBOutlet weak var stepper: NSStepper!
    
    override public func awakeFromNib() {
        
        track()
        
        // Configure directory window
        // directory.intercellSpacing = NSSize(width: 0, height: 0)
        tableview.reloadData()
        
        // Set icon and title
        switch archive.getType() {
            
        case G64_CONTAINER:
            header.stringValue = "G64 Format"
            subheader.stringValue = "Files in G64 format contain a bit-accurate image of a 1540/1541 diskette."
            break;
            
        case NIB_CONTAINER:
            header.stringValue = "NIB Format"
            subheader.stringValue = "Files in NIB format contain a bit-accurate image of a 1540/1541 diskette."
            break;

        default:
            assert(false)
            break;
        }
        
        diskIcon.image = NSImage.init(named: NSImage.Name(rawValue: "IconD64"))
        subsubheader.stringValue = "Note: Files of this type often contain copy protection mechanisms that are incompatible with the current drive emulation."
    }
    
    func update() {
    
        let track = (halftrack / 2) + 1
        let bytes = archive.getSizeOfItem(halftrack)
        let text1 = String(format: "Track: %02X Halftrack: %02X", track, halftrack)
        let text2 = String(format: "%d Bytes", bytes)
        
        trackinfo.stringValue = String(text1)
        trackSizeinfo.stringValue = String(text2)
        
        tableview.reloadData()
    }
    
    // Action methods
    
    @IBAction func stepperAction(_ sender: Any!) {
        
        track()
        let sender = sender as? NSStepper
        let value = sender?.integerValue
        
        print("\(value!)")
        if value == 1 && halftrack < archive.getNumberOfItems() {
            halftrack += 1
        }
        if value == -1 && halftrack > 0 {
            halftrack -= 1
        }
    }

    @IBAction func cancelAction(_ sender: Any!) {
        
        window?.orderOut(self)
        parentWindow.endSheet(window!, returnCode: .cancel)
    }
    
    @IBAction func okAction(_ sender: Any!) {
        
        NSLog("OK Action")
        
        // Insert archive as disk
        c64.insertDisk(archive)
        
        window?.orderOut(self)
        parentWindow.endSheet(window!, returnCode: .OK)
    }
}

//
// NSTableView delegate and data source
//

/*
extension DiskMountController : NSTableViewDelegate {
    
}
*/
    
extension DiskMountController : NSTableViewDataSource {
    
    func numberOfRows(in tableView: NSTableView) -> Int {
        
        let sizeOfTrack = archive.getSizeOfItem(halftrack)
        return sizeOfTrack / bytesPerRow
    }
    
    func tableView(_ tableView: NSTableView,
                   objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        
        if (tableColumn?.identifier)!.rawValue == "addr" {
            
            let s = String(format:"%04X", row * bytesPerRow)
            return s
        }
        
        if (tableColumn?.identifier)!.rawValue == "data" {
            
            let x = 55
            let s = String(format:"%02X", x)
            return s
        }
        
        return "???"
    }
}


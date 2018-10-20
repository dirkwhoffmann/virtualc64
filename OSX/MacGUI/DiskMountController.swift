//
//  DiskMountController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 05.02.18.
//

import Foundation

class DiskMountController : UserDialogController {
    
    var archive: ArchiveProxy!
    
    let bytesPerRow = 32
    var item = 0          // Selected archive item (track) to display
    var sizeInBytes = 0   // Number of bytes in selected item
    var sizeInBits = 0    // Number of bits in selected item
    
    // Outlets
    @IBOutlet weak var icon: NSImageView!
    @IBOutlet weak var header: NSTextField!
    @IBOutlet weak var subheader: NSTextField!
    @IBOutlet weak var subsubheader: NSTextField!
    @IBOutlet weak var contents: NSTableView!
    @IBOutlet weak var trackinfo: NSTextField!
    @IBOutlet weak var trackSizeinfo: NSTextField!
    @IBOutlet weak var stepper: NSStepper!
    @IBOutlet weak var driveSelector: NSPopUpButton!
    
    override func showSheet(withParent controller: MyController,
                            completionHandler:(() -> Void)? = nil) {
        
        track()
        archive = controller.mydocument.attachment as? ArchiveProxy
        super.showSheet(withParent: controller, completionHandler: completionHandler)
    }
    
    override public func awakeFromNib() {
 
        let numItems = archive.numberOfItems()
  
        stepper.minValue = 0
        stepper.maxValue = Double(numItems) - 1
        stepper.integerValue = item
        
        // Configure directory window
        // directory.intercellSpacing = NSSize(width: 0, height: 0)
        
        // Set icon and title
        switch archive.type() {
            
        case G64_FILE:
            icon.image = NSImage.init(named: NSImage.Name(rawValue: "IconD64"))
            header.stringValue = "G64 Format"
            break;

        default:
            assert(false)
            break;
        }
        
        update()
    }
    
    func update() {
    
        let halftrack = item + 1
        let track = (item / 2) + 1
        
        if (archive.type() == G64_FILE) {
            sizeInBytes = archive.size(ofItem: item)
            sizeInBits = sizeInBytes * 8
            trackSizeinfo.stringValue = String(format: "%d Bytes", sizeInBytes)
        } else {
            sizeInBits = archive.size(ofItem: item)
            sizeInBytes = sizeInBits / 8
            trackSizeinfo.stringValue = String(format: "%d Bits", sizeInBits)
        }
        
        if (halftrack % 2 == 0) {
            let info = String(format: "Contents of halftrack %d:", halftrack)
            trackinfo.stringValue = String(info)
        } else {
            let info = String(format: "Contents of halftrack %d (track %d):", halftrack, track)
            trackinfo.stringValue = String(info)
        }
        
        contents.reloadData()
    }
    
    // Action methods
    
    @IBAction func stepperAction(_ sender: Any!) {
        
        let sender = sender as! NSStepper
        item = sender.integerValue
        update()
    }
    
    @IBAction func okAction(_ sender: Any!) {
        
        track()
        
        if driveSelector.selectedItem!.tag == 1 {
            parent.changeDisk(archive, drive: 1)
        } else {
            parent.changeDisk(archive, drive: 2)
        }
        
        parent.metalScreen.rotateBack()
        hideSheet()
    }
}

//
// NSTableView delegate and data source
//

extension DiskMountController : NSTableViewDelegate {
    
    func tableView(_ tableView: NSTableView,
                   willDisplayCell cell: Any, for tableColumn: NSTableColumn?, row: Int) {
        
        // let c = cell as! NSTextFieldCell
        // c.font = cbmfont
        // c.textColor = .red
    }
}


extension DiskMountController : NSTableViewDataSource {
    
    func numberOfRows(in tableView: NSTableView) -> Int {
        
        return sizeInBytes / bytesPerRow
    }
    
    func tableView(_ tableView: NSTableView,
                   objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        
        if (tableColumn?.identifier)!.rawValue == "addr" {
            
            let s = String(format:"$%04X", row * bytesPerRow)
            return s
        }
        
        if (tableColumn?.identifier)!.rawValue == "data" {
            
            return archive.hexDump(item, offset: (row * bytesPerRow), num: bytesPerRow)
        }
        
        return "???"
    }
}


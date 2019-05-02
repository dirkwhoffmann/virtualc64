//
// This file is part of VirtualC64 - A cycle accurate Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

class ArchiveMountController : UserDialogController {
    
    var archive: AnyArchiveProxy!
    
    // Custom font
    let cbmfont = NSFont.init(name: "C64ProMono", size: 10)
    let cbmfontsmall = NSFont.init(name: "C64ProMono", size: 8)
    
    // Outlets
    @IBOutlet weak var icon: NSImageView!
    @IBOutlet weak var header: NSTextField!
    @IBOutlet weak var subheader: NSTextField!
    @IBOutlet weak var subsubheader: NSTextField!
    @IBOutlet weak var contents: NSTableView!
    @IBOutlet weak var insertButton: NSButton!
    @IBOutlet weak var driveSelector: NSPopUpButton!

    override func showSheet(completionHandler handler:(() -> Void)? = nil) {
        
        if let attachment = myDocument?.attachment as? AnyArchiveProxy {
            
            archive = attachment
            super.showSheet(completionHandler: handler)
        }
    }
    
    override public func awakeFromNib() {
        
        // Configure directory window
        contents.deselectAll(self)
        contents.intercellSpacing = NSSize(width: 0, height: 0)
        contents.selectRowIndexes(IndexSet.init(integer: 0), byExtendingSelection: false)
        contents.reloadData()
        
        // Set icon and title
        if (archive.numberOfItems() == 1) {
            subheader.stringValue = "This file contains the byte stream of a single C64 program."
        } else {
            subheader.stringValue = "This file contains the byte streams of multiple C64 programs."
        }
        subsubheader.stringValue = "Flushing files directly into memory is likely to work."
        
        switch archive.type() {

        case T64_FILE:
            icon.image = NSImage.init(named: "IconT64")
            header.stringValue = "T64 File Archive"
            insertButton.title = "Insert as Disk"
            break
            
        case PRG_FILE:
            icon.image = NSImage.init(named: "IconPRG")
            header.stringValue = "PRG File Container"
            insertButton.title = "Insert as Disk"
            break
            
        case P00_FILE:
            icon.image = NSImage.init(named: "IconP00")
            header.stringValue = "P00 File Container"
            insertButton.title = "Insert as Disk"
            break
            
        case D64_FILE:
            icon.image = NSImage.init(named: "IconD64")
            header.stringValue = "D64 File Archive"
            subheader.stringValue = "This file contains a byte-accurate image of a C64 diskette."
            subsubheader.stringValue = "Flushing files into memory is not recommended for this file type."
            break
            
        default:
            assert(false)
            break
        }
    }
    
    //
    // Action methods
    //

    @IBAction override func okAction(_ sender: Any!) {
        
        let nr = (driveSelector.selectedItem!.tag == 1) ? 1 : 2
        
        myController?.changeDisk(archive, drive: nr)
        myController?.metalScreen.rotateBack()
        
        hideSheet()
    }
    
    @IBAction func performDoubleClick(_ sender: Any!) {
        
        // Only proceed if a row is selected
        if contents.selectedRow == -1 {
            return
        }
        
        // Flash file into memory
        myController?.mydocument?.flashAttachmentIntoMemory()
        myController?.keyboardcontroller.type("RUN\n")
        
        /*
        // Get load address of the selected item
        archive.selectItem(item)
        let loadAddress = archive.destinationAddrOfItem()
        
        // Type RUN or SYS, depending of the load address
        if loadAddress == 0x801 {
            myController?.keyboardcontroller.type("RUN\n")
        } else {
            myController?.keyboardcontroller.type("SYS \(loadAddress)")
        }
        */
        
        hideSheet()
    }
}

//
// NSTableView delegate and data source
//

extension ArchiveMountController : NSTableViewDelegate {
    
    func tableView(_ tableView: NSTableView,
                   willDisplayCell cell: Any, for tableColumn: NSTableColumn?, row: Int) {
        
        let c = cell as! NSTextFieldCell
        
        archive.selectItem(row)
        c.textColor = archive.typeOfItem() == "PRG" ? .textColor : .gray
        c.font = cbmfont
    }
    
    func tableView(_ tableView: NSTableView, shouldSelectRow row: Int) -> Bool {
        
        return true
    }
}

extension ArchiveMountController : NSTableViewDataSource {
    
    func numberOfRows(in tableView: NSTableView) -> Int {

        return archive.numberOfItems()
    }

    func tableView(_ tableView: NSTableView,
                   objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
   
        archive.selectItem(row)
        
        switch (tableColumn?.identifier)!.rawValue {
            
            case "filename":
            return archive.unicodeNameOfItem()
            
            case "filesize":
            return archive.sizeOfItemInBlocks()
            
            case "filetype":
            return archive.typeOfItem()
            
            default:
            return "???"
        }
    }
}


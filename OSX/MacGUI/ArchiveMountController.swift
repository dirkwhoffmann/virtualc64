//
//  MountViewController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 01.02.18.
//

import Foundation

class ArchiveMountController : UserDialogController {
    
    var archive: ArchiveProxy!
    
    // Custom font
    let cbmfont = NSFont.init(name: "C64ProMono", size: 10)
    let cbmfontsmall = NSFont.init(name: "C64ProMono", size: 8)
    
    // Outlets
    @IBOutlet weak var icon: NSImageView!
    @IBOutlet weak var header: NSTextField!
    @IBOutlet weak var subheader: NSTextField!
    @IBOutlet weak var subsubheader: NSTextField!
    @IBOutlet weak var contents: NSTableView!
    @IBOutlet weak var protect: NSButton!

    override func showSheet(withParent controller: MyController,
                            completionHandler:(() -> Void)? = nil) {
        
        track()
        let document = controller.document as! MyDocument
        archive = document.attachment as! ArchiveProxy
        super.showSheet(withParent: controller, completionHandler: completionHandler)
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

        case T64_CONTAINER:
            icon.image = NSImage.init(named: NSImage.Name(rawValue: "IconT64"))
            header.stringValue = "T64 File Archive"
            break
            
        case PRG_CONTAINER:
            icon.image = NSImage.init(named: NSImage.Name(rawValue: "IconPRG"))
            header.stringValue = "PRG File Container"
            break
            
        case P00_CONTAINER:
            icon.image = NSImage.init(named: NSImage.Name(rawValue: "IconP00"))
            header.stringValue = "P00 File Container"
            break
            
        case D64_CONTAINER:
            icon.image = NSImage.init(named: NSImage.Name(rawValue: "IconD64"))
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

    @IBAction func okAction(_ sender: Any!) {
        
        track()
        
        // Insert archive as disk
        c64.insertDisk(archive)
        parent.rotateBack()
        
        // Set write protection
        let value = protect.integerValue
        c64.vc1541.setWriteProtection(value != 0)
        
        hideSheet()
    }
    
    @IBAction func performDoubleClick(_ sender: Any!) {
        
        // Flash file into memory
        c64.flushArchive(archive, item: contents.selectedRow)
        
        // Type "RUN"
        parent.simulateUserTypingText("RUN\n")
        
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
        
        c.font = cbmfont
        c.textColor = archive.type(ofItem: row) == "PRG" ? .black : .gray
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
   
        if (tableColumn?.identifier)!.rawValue == "filename" {
    
            return archive.unicodeName(ofItem: row, maxChars: 16)
        }
        if (tableColumn?.identifier)!.rawValue == "filesize" {
            
            return archive.sizeOfItem(inBlocks: row)
        }
        if (tableColumn?.identifier)!.rawValue == "filetype" {

            return archive.type(ofItem: row)
        }
    return "???"
    }
}


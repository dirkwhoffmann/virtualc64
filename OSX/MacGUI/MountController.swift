//
//  MountViewController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 01.02.18.
//

import Foundation

class MountController : NSWindowController {
    
    // Internal state
    var controller: MyController!
    var archive: ArchiveProxy!
    var c64: C64Proxy!
    var parentWindow: NSWindow!
    
    // Custom font
    let cbmfont = NSFont.init(name: "C64ProMono", size: 10)
    let cbmfontsmall = NSFont.init(name: "C64ProMono", size: 8)
    
    // Outlets
    @IBOutlet weak var directory: NSTableView!
    @IBOutlet weak var ok: NSButton!
    @IBOutlet weak var cancel: NSButton!
    @IBOutlet weak var headerText: NSTextField!
    @IBOutlet weak var diskIcon: NSImageView!
    @IBOutlet weak var diskIconFrame: NSBox!

    func setParentController(_ controller: MyController) {

        let document = controller.document as! MyDocument
        
        self.controller = controller
        self.archive = document.attachedArchive
        self.c64 = document.c64
        self.parentWindow = controller.window
    }
    
    override public func awakeFromNib() {
        
        NSLog("\(#function)")
        
        // Configure directory window
        directory.target = self
        directory.delegate = self
        directory.dataSource = self
        directory.deselectAll(self)
        directory.intercellSpacing = NSSize(width: 0, height: 0)
        directory.doubleAction = #selector(MountController.performDoubleClick(_:))
        directory.selectRowIndexes(IndexSet.init(integer: 0), byExtendingSelection: false)
        directory.reloadData()
        
        // Update header text
        let path = archive.getPath()
        headerText.stringValue = path!
        
        // Set icon and title
        switch archive.getType() {

        case D64_CONTAINER:
            diskIcon.image = NSImage.init(named: NSImage.Name(rawValue: "IconD64"))
            diskIconFrame.title = "D64 archive"
            break
        case T64_CONTAINER:
            diskIcon.image = NSImage.init(named: NSImage.Name(rawValue: "IconT64"))
            diskIconFrame.title = "T64 archive"
            break
        case PRG_CONTAINER:
            diskIcon.image = NSImage.init(named: NSImage.Name(rawValue: "IconPRG"))
            diskIconFrame.title = "PRG archive"
            break
        case P00_CONTAINER:
            diskIcon.image = NSImage.init(named: NSImage.Name(rawValue: "IconP00"))
            diskIconFrame.title = "P00 archive"
            break
        default:
            assert(false)
            break
        }
    }

    func terminate() {
        NSLog("\(#function)")
    }

    
    //
    // Action methods
    //

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
    
    @IBAction func performDoubleClick(_ sender: Any!) {
        
        // Flash file into memory
        c64.flushArchive(archive, item: directory.selectedRow)
        
        // Type "RUN"
        controller.simulateUserTypingText("RUN\n")
        
        window?.orderOut(self)
        parentWindow.endSheet(window!, returnCode: .OK)
    }
}

//
// NSTableView delegate and data source
//

extension MountController : NSTableViewDelegate {
    
    func tableView(_ tableView: NSTableView,
                   willDisplayCell cell: Any, for tableColumn: NSTableColumn?, row: Int) {
        
        let c = cell as! NSTextFieldCell
        
        c.font = cbmfont
        c.textColor = archive.getTypeOfItem(row) == "PRG" ? .black : .gray
    }
    
    func tableView(_ tableView: NSTableView, shouldSelectRow row: Int) -> Bool {
        
        return true
    }
}

extension MountController : NSTableViewDataSource {
    
    func numberOfRows(in tableView: NSTableView) -> Int {

        return archive.getNumberOfItems()
    }

    func tableView(_ tableView: NSTableView,
                   objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
   
        if (tableColumn?.identifier)!.rawValue == "filename" {
    
            return archive.getUnicodeName(ofItem: row, maxChars: 16)
        }
        if (tableColumn?.identifier)!.rawValue == "filesize" {
            
            return archive.getSizeOfItem(inBlocks: row)
        }
        if (tableColumn?.identifier)!.rawValue == "filetype" {

            return archive.getTypeOfItem(row)
        }
    return "???"
    }
}


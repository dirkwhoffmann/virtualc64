//
//  CartridgeMountController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 06.02.18.
//

import Foundation

class CartridgeMountController : MountController {
    
    var cartridge: CRTProxy!
    
    // Outlets
    @IBOutlet weak var header: NSTextField!
    @IBOutlet weak var name: NSTextField!
    @IBOutlet weak var lines: NSTextField!
    @IBOutlet weak var data: NSTableView!
    
    override public func awakeFromNib() {
        
        track()
        
        // cartridge = archive as! CRTContainerProxy
        
        // Configure directory window
        // contents.target = self
        // contents.delegate = self
        // contents.dataSource = self
        // contents.deselectAll(self)
        data.intercellSpacing = NSSize(width: 0, height: 0)
        data.reloadData()
        
        // Set title
        // let archive = self.archive as! CRTContainerProxy
        
        /*
        if cartridge.cartridgeType() != CRT_NORMAL) {
            let str = String(format: "CRT Format (Type %d: %s)",
                             cartridge.getCartridgeType(),
                             cartridge.getCartridgeTypeAsStr())
            header.stringValue = str
        }
        */
    }
    
    //
    // Action methods
    //
    
    @IBAction func okAction(_ sender: Any!) {
        
        NSLog("OK Action")
        
        // Attach cartridge
        c64.attachCartridgeAndReset(cartridge)
        
        window?.orderOut(self)
        parentWindow.endSheet(window!, returnCode: .OK)
    }
    
    @IBAction func cancelAction(_ sender: Any!) {
        
        window?.orderOut(self)
        parentWindow.endSheet(window!, returnCode: .cancel)
    }
}

//
// NSTableView delegate and data source
//

extension CartridgeMountController : NSTableViewDelegate {
    
    /*
    func tableView(_ tableView: NSTableView,
                   willDisplayCell cell: Any, for tableColumn: NSTableColumn?, row: Int) {
        
        let c = cell as! NSTextFieldCell
        
        c.font = cbmfont
        c.textColor = archive.getTypeOfItem(row) == "PRG" ? .black : .gray
    }
 */
    
}

extension CartridgeMountController : NSTableViewDataSource {
    
    func numberOfRows(in tableView: NSTableView) -> Int {
        
        // return cartridge.getNumberOfChips()
        return 1
    }
    
    func tableView(_ tableView: NSTableView,
                   objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        
        if (tableColumn?.identifier)!.rawValue == "chip" {
            
            return String(row)
        }
        /*
        if (tableColumn?.identifier)!.rawValue == "addr" {
            
            let str = String(format: "$%04X - %04X",
                             cartridge.getLoadAddr(row),
                             cartridge.getLoadAddr(row) + cartrifge.size(row))
            return str
        }
        if (tableColumn?.identifier)!.rawValue == "size" {
            
            let str = String(format: "%d KB", cartridge.size(row) / 1024)
            return str
        }
         */
        return "???"
    }
}


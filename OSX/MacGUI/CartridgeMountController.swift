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
    @IBOutlet weak var type: NSTextField!
    @IBOutlet weak var exrom: NSTextField!
    @IBOutlet weak var game: NSTextField!
    @IBOutlet weak var data: NSTableView!
    
    override func showSheet(_ controller: MyController) {
        
        let document = controller.document as! MyDocument
        cartridge = document.attachment as! CRTProxy
        
        super.showSheet(controller)
    }
    
    override public func awakeFromNib() {
        
        track()
        
        // Configure table view
        data.intercellSpacing = NSSize(width: 0, height: 0)
        data.reloadData()
        
        // Set name and type
        let cartName = cartridge.cartridgeName()!
        let cartType = cartridge.cartridgeType().rawValue
        let cartTypeName = cartridge.cartridgeTypeName()!
    
        name.stringValue = (cartName == "") ? "" : "Name: \(cartName)"
        type.stringValue = "Type \(cartType): \(cartTypeName)"
    
        // Set exrom and game line
        let exromLine = cartridge.exromLine()
        let gameLine = cartridge.gameLine()
        exrom.stringValue = "Exrom line: \(exromLine)"
        game.stringValue = "Game line: \(gameLine)"
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
        
        return cartridge.chipCount()
    }
    
    func tableView(_ tableView: NSTableView,
                   objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        
        if (tableColumn?.identifier)!.rawValue == "chip" {
            
            return String(row)
        }
        
        if (tableColumn?.identifier)!.rawValue == "addr" {
            
            let str = String(format: "$%04X - %04X",
                             cartridge.loadAddr(ofChip: row),
                             cartridge.loadAddr(ofChip: row) + cartridge.size(ofChip: row))
            return str
        }
        if (tableColumn?.identifier)!.rawValue == "size" {
            
            let str = String(format: "%d KB", cartridge.size(ofChip: row) / 1024)
            return str
        }

        if (tableColumn?.identifier)!.rawValue == "type" {
            
            switch cartridge.type(ofChip: row) {
            case 0: return "ROM"
            case 1: return "RAM"
            case 2: return "Flash ROM"
            default: return "Unknown"
            }
        }
        
        return "???"
    }
}


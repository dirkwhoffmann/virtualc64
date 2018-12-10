//
// This file is part of VirtualC64 - A cycle accurate Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

class CartridgeMountController : UserDialogController {
    
    var cartridge: CRTFileProxy!
    
    // Outlets
    @IBOutlet weak var header: NSTextField!
    @IBOutlet weak var name: NSTextField!
    @IBOutlet weak var type: NSTextField!
    @IBOutlet weak var exrom: NSTextField!
    @IBOutlet weak var game: NSTextField!
    @IBOutlet weak var data: NSTableView!
    
    override func showSheet(withParent controller: MyController,
                            completionHandler:(() -> Void)? = nil) {
        
        track()
        cartridge = controller.mydocument.attachment as? CRTFileProxy
        super.showSheet(withParent: controller, completionHandler: completionHandler)
    }
        
    override public func awakeFromNib() {
        
        track()
        
        // Configure table view
        data.intercellSpacing = NSSize(width: 0, height: 0)
        data.reloadData()
        
        // Set name and type
        let cartName = cartridge.name()!
        let cartType = cartridge.cartridgeType().rawValue
        let cartTypeName = cartridge.cartridgeTypeName()!
    
        name.stringValue = (cartName == "") ? "" : "Name: \(cartName)"
        type.stringValue = "Type \(cartType): \(cartTypeName)"
    
        // Set exrom and game line
        let exromLine = cartridge.initialExromLine()
        let gameLine = cartridge.initialGameLine()
        exrom.stringValue = "Exrom line: \(exromLine)"
        game.stringValue = "Game line: \(gameLine)"
    }
    
    //
    // Action methods
    //
    
    @IBAction func okAction(_ sender: Any!) {
        
        // Attach cartridge
        c64.expansionport.attachCartridgeAndReset(cartridge)
        parent.metalScreen.rotateBack()
        
        hideSheet()
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
        c.textColor = archive.getTypeOfItem() == "PRG" ? .textColor : .gray
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
            
            let str = String(format: "$%04X - $%04X",
                             cartridge.chipAddr(row),
                             cartridge.chipAddr(row) + cartridge.chipSize(row) - 1)
            return str
        }
        if (tableColumn?.identifier)!.rawValue == "size" {
            
            let str = String(format: "%d KB", cartridge.chipSize(row) / 1024)
            return str
        }

        if (tableColumn?.identifier)!.rawValue == "type" {
            
            switch cartridge.chipType(row) {
            case 0: return "ROM"
            case 1: return "RAM"
            case 2: return "Flash ROM"
            default: return "Unknown"
            }
        }
        
        return "???"
    }
}


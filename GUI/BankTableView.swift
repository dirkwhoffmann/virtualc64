// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class BankTableView: NSTableView, NSTableViewDelegate {
    
    @IBOutlet weak var inspector: Inspector!
   
    var c64: C64Proxy { return inspector.parent.c64 }

    // Displayed memory bank
    var bank = 0

    // Data caches
    var bankCache: [Int: MemoryType] = [:]

    override func awakeFromNib() {

        delegate = self
        dataSource = self
        target = self
        action = #selector(clickAction(_:))
    }
    
    func cache() {

    }
    
    func refresh(count: Int = 0, full: Bool = false) {

        if full {
             
             assignFormatter(inspector.fmt4, column: "bank")
         }
        
        cache()
        reloadData()
    }

    @IBAction func clickAction(_ sender: NSTableView!) {

        inspector.jumpTo(bank: sender.clickedRow)
    }
}

extension BankTableView: NSTableViewDataSource {
    
    func numberOfRows(in tableView: NSTableView) -> Int { return 16; }
    
    func tableView(_ tableView: NSTableView, objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        
        switch tableColumn?.identifier.rawValue {

        case "source":

            switch inspector.bankType[row] {
                
            case .NONE:   return "Unmapped"
            case .RAM:    return "Ram"
            case .PP:     return "Ram"
            case .BASIC:  return "Basic Rom"
            case .CHAR:   return "Character Rom"
            case .KERNAL: return "Kernal Rom"
            case .IO:     return "IO"
            case .CRTLO:  return "Cartridge Lo"
            case .CRTHI:  return "Cartridge Hi"
            default:      return "???"
            }

        default:
            return row
        }
    }
}

/*
 extension BankTableView: NSTableViewDelegate {
    
    func tableView(_ tableView: NSTableView, willDisplayCell cell: Any, for tableColumn: NSTableColumn?, row: Int) {
        
        if let cell = cell as? NSTextFieldCell {

            if inspector.memBank[row] == M_NONE {
                cell.textColor = .gray
            } else {
                cell.textColor = nil
            }
        }
    }
}
*/

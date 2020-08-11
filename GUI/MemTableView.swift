// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class MemTableView: NSTableView {
    
    @IBOutlet weak var inspector: Inspector!

    var c64: C64Proxy { return inspector.parent.c64 }
    var mem: MemoryProxy { return c64.mem }
        
    override func awakeFromNib() {
        
        delegate = self
        dataSource = self
        target = self
    }
    
    func refresh(count: Int = 0, full: Bool = false) {
        
        if full {
            
            assignFormatter(inspector.fmt16, column: "addr")
        }
        
        reloadData()
    }
    
    var bankType: MemoryType {
        return inspector.bankType[inspector.displayedBank]!
    }
}

extension MemTableView: NSTableViewDataSource {
    
    func numberOfRows(in tableView: NSTableView) -> Int {
        
        return 4096 / 16 // Bank size divided by bytes per row
    }
    
    func tableView(_ tableView: NSTableView, objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        
        let base = inspector.displayedBank * 4096
        let addr = UInt16(base + 16 * row)
        
        switch tableColumn?.identifier.rawValue {
            
        case "addr":
            return addr
            
        case "ascii":
            return mem.txtdump(Int(addr), num: 16, src: bankType)
                    
        case "hex0":
            return mem.memdump(Int(addr), num: 16, hex: inspector.hex, src: bankType)
            
        default:
            return "???"
        }        
    }
}

extension MemTableView: NSTableViewDelegate {
    
    func tableView(_ tableView: NSTableView, willDisplayCell cell: Any, for tableColumn: NSTableColumn?, row: Int) {
        
        let cell = cell as? NSTextFieldCell

        if tableColumn?.identifier.rawValue == "bank" {
            cell?.font = NSFont.systemFont(ofSize: 10)
            cell?.textColor = .gray
        } else {
            cell?.textColor = NSColor.textColor
        }
        
        if tableColumn?.identifier.rawValue == "ascii" {
            // cell?.font = cbmfont
        }
    }
}

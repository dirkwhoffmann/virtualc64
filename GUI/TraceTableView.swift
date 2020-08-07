// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class TraceTableView: NSTableView {
    
    @IBOutlet weak var inspector: Inspector!
    var c64: C64Proxy { return inspector.parent.c64 }
    
    // Data caches
    var instrInRow: [Int: DisassembledInstruction] = [:]
    var numRows = 0
    
    override func awakeFromNib() {
        
        dataSource = self
        target = self
    }
    
    private func cache() {
        
        numRows = c64.cpu.loggedInstructions()
        
        for i in 0 ..< numRows {
            instrInRow[i] = c64.cpu.getLoggedInstrInfo(i)
        }
    }
    
    func refresh(count: Int = 0, full: Bool = false) {
        
        if full {
            assignFormatter(inspector.fmt16, column: "addr")
        }
        
        cache()
        reloadData()
        
        // In animation mode, jump to the currently executed instruction
        if count != 0 { scrollRowToVisible(numRows - 1) }
    }
}

extension TraceTableView: NSTableViewDataSource {
    
    func numberOfRows(in tableView: NSTableView) -> Int {
        
        return numRows
    }
    
    func tableView(_ tableView: NSTableView, objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        
        if var info = instrInRow[row] {
            
            switch tableColumn?.identifier.rawValue {
                
            case "addr":
                return info.addr
            case "flags":
                return String(cString: &info.flags.0)
            case "instr":
                return String(cString: &info.command.0)
            default:
                return "???"
            }
        }
    
        return "??"
    }
}

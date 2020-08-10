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
    var cpu: CPUProxy { return c64.cpu }
    
    // Data caches
    var numRows = 0
    var addrInRow: [Int: String] = [:]
    var flagsInRow: [Int: String] = [:]
    var instrInRow: [Int: String] = [:]
    
    override func awakeFromNib() {
        
        dataSource = self
        target = self
    }
    
    private func cache() {
        
        numRows = c64.cpu.loggedInstructions()
        
        for i in 0 ..< numRows {

            var length = 0
            addrInRow[i] = c64.cpu.disassembleRecordedPC(i)
            instrInRow[i] = c64.cpu.disassembleRecordedBytes(i)
            instrInRow[i] = c64.cpu.disassembleRecordedInstr(i, length: &length)
            flagsInRow[i] = c64.cpu.disassembleRecordedFlags(i)
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
        
        switch tableColumn?.identifier.rawValue {
            
        case "addr":
            return addrInRow[row]
        case "flags":
            return flagsInRow[row]
        case "instr":
            return instrInRow[row]
        default:
            return "???"
        }
    }
}

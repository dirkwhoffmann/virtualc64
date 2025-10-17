// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class TraceTableView: NSTableView {
    
    @IBOutlet weak var inspector: Inspector!
    
    var emu: EmulatorProxy? { return inspector.parent.emu }
    var cpu: CPUProxy? { return emu?.cpu }

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
        
        if let cpu = cpu {

            numRows = cpu.loggedInstructions()

            for i in 0 ..< numRows {

                var length = 0
                addrInRow[i] = cpu.disassembleRecorded(i, format: "%p", length: &length)
                instrInRow[i] = cpu.disassembleRecorded(i, format: "%i", length: &length)
                flagsInRow[i] = cpu.disassembleRecorded(i, format: "%f", length: &length)
            }
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

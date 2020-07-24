// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class InstrTableView: NSTableView {
    
    @IBOutlet weak var inspector: Inspector!
    var c64: C64Proxy { return inspector.parent.c64 }
    
    var instructionAtRow: [Int: DisassembledInstruction] = [:]
    var rowForAddress: [UInt16: Int] = [:]
    var hex = true
    
    override func awakeFromNib() {
        
        delegate = self
        dataSource = self
        target = self
        doubleAction = #selector(doubleClickAction(_:))
    }
    
    private func cache() {
         
    }
    
    func refresh(count: Int = 0, full: Bool = false) {
        
        cache()
        reloadData()
    }
    
    @IBAction func doubleClickAction(_ sender: NSTableView!) {
        
        let row = sender.selectedRow
        
        if let instr = instructionAtRow[row] {
            track("Toggling breakpoint at \(instr.addr)")
            c64.cpu.toggleBreakpoint(instr.addr)
            reloadData()
        }
    }
    
    func setHex(_ value: Bool) {
        
        hex = value
        updateDisplayedAddresses()
    }
    
    func updateDisplayedAddresses(startAddr: UInt16) {
                
        var addr = Int(startAddr)
        rowForAddress = [:]
        
        for i in 0...255 {
            if addr <= 0xFFFF {
                instructionAtRow[i] = c64.cpu.disassemble(UInt16(addr), hex: hex)
                rowForAddress[UInt16(addr)] = i
                addr += Int(instructionAtRow[i]!.size)
            } else {
                instructionAtRow[i] = nil
            }
        }
        
        reloadData()
    }
    
    func updateDisplayedAddresses() {
                
        updateDisplayedAddresses(startAddr: c64.cpu.pc())
    }
    
    func refresh() {
                
        if let row = rowForAddress[c64.cpu.pc()] {
            
            // If PC points to an address which is already displayed,
            // we simply select the corresponding row.
            scrollRowToVisible(row)
            selectRowIndexes([row], byExtendingSelection: false)
            
        } else {
            
            // If PC points to an address that is not displayed,
            // we update the whole view and display PC in the first row.
            updateDisplayedAddresses()
            scrollRowToVisible(0)
            selectRowIndexes([0], byExtendingSelection: false)
        }
    }
}

extension InstrTableView: NSTableViewDataSource {
    
    func numberOfRows(in tableView: NSTableView) -> Int {
        return 256
    }
    
    func tableView(_ tableView: NSTableView, objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        
        if var instr = instructionAtRow[row] {
            
            switch tableColumn?.identifier.rawValue {
                
            case "break":
                if c64.cpu.breakpoint(instr.addr) {
                    return "⛔"
                } else {
                    return " "
                }
            case "addr":
                return String.init(utf8String: &instr.pc.0)
            case "data01":
                return String.init(utf8String: &instr.byte1.0)
            case "data02":
                return String.init(utf8String: &instr.byte2.0)
            case "data03":
                return String.init(utf8String: &instr.byte3.0)
            case "ascii":
                return String.init(utf8String: &instr.command.0)
            default:
                return "?"
            }
        }
        return ""
    }
}

extension InstrTableView: NSTableViewDelegate {
    
    func tableView(_ tableView: NSTableView, willDisplayCell cell: Any, for tableColumn: NSTableColumn?, row: Int) {
        
        let cell = cell as? NSTextFieldCell
        
        if  let instr = instructionAtRow[row] {
            
            if c64.cpu.breakpoint(instr.addr) {
                cell?.textColor = NSColor.systemRed
            } else {
                cell?.textColor = NSColor.textColor
            }
        }
    }
}

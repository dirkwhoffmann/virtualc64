// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class InstrTableView: NSTableView {
    
    @IBOutlet weak var inspector: Inspector!
    
    var emu: EmulatorProxy? { return inspector.parent.emu }
    var cpu: CPUProxy? { return emu?.cpu }

    enum BreakpointType {
        
        case none
        case enabled
        case disabled
    }

    // The first address to disassemble
    var addrInFirstRow: Int = 0
    
     // Data caches
    var numRows = 0
    var bpInRow: [Int: BreakpointType] = [:]
    var addrInRow: [Int: Int] = [:]
    var addrStrInRow: [Int: String] = [:]
    var dataInRow: [Int: String] = [:]
    var instrInRow: [Int: String] = [:]
    var rowForAddr: [Int: Int] = [:]

    // Optional address to be highlighted by an alert symbol
    var alertAddr: Int?

    // Number format
    var hex = true
    
    // Saved program counters
    // var breakpointPC = -1
    // var watchpointPC = -1

    override func awakeFromNib() {
        
        delegate = self
        dataSource = self
        target = self
        
        doubleAction = #selector(doubleClickAction(_:))
        action = #selector(clickAction(_:))
    }
    
    private func cache(addrInFirstRow addr: Int) {

        addrInFirstRow = addr
        cache()
    }
    
    private func cache() {
        
        if let cpu = cpu {

            numRows = 256
            rowForAddr = [:]

            var addr = addrInFirstRow

            for i in 0 ..< numRows {

                var length = 0
                addrInRow[i] = addr
                addrStrInRow[i] = cpu.disassemble(addr, format: "%p", length: &length)
                instrInRow[i] = cpu.disassemble(addr, format: "%i", length: &length)
                dataInRow[i] = cpu.disassemble(addr, format: "%b", length: &length)

                if !cpu.hasBreakpoint(atAddr: addr) {
                    bpInRow[i] = BreakpointType.none
                } else if cpu.breakpoint(atAddr: addr).enabled {
                    bpInRow[i] = BreakpointType.enabled
                } else {
                    bpInRow[i] = BreakpointType.disabled
                }

                rowForAddr[addr] = i
                addr += length
            }
        }
    }
    
    func refresh(count: Int = 0, full: Bool = false, addr: Int = 0) {
        
        if full {

            assignFormatter(inspector.fmt16, column: "addr")
            cache()
            reloadData()
            jumpTo(addr: addr)

        } else {

            // In animation mode, jump to the currently executed instruction
            if count != 0 { jumpTo(addr: addr) }
        }
    }
    
    func jumpTo(addr: Int) {
                
        if let row = rowForAddr[addr] {
            
            // If the requested address is already displayed, we simply select
            // the corresponding row.
            reloadData()
            jumpTo(row: row)
            
        } else {
            
            // If the requested address is not displayed, we update the data
            // cache and display the address in row 0.
            cache(addrInFirstRow: addr)
            reloadData()
            jumpTo(row: 0)
        }
    }
    
    func jumpTo(row: Int) {
        
        scrollRowToVisible(row)
        selectRowIndexes([row], byExtendingSelection: false)
    }
    
    @IBAction func clickAction(_ sender: NSTableView!) {
        
        if sender.clickedColumn == 0 {
            
            clickAction(row: sender.clickedRow)
        }
    }
    
    func clickAction(row: Int) {

        if let cpu = cpu {

            if let addr = addrInRow[row] {

                if !cpu.hasBreakpoint(atAddr: addr) {
                    emu?.put(.BP_SET_AT, value: addr)
                } else if cpu.breakpoint(atAddr: addr).enabled {
                    emu?.put(.BP_DISABLE_AT, value: addr)
                } else {
                    emu?.put(.BP_ENABLE_AT, value: addr)
                }

                inspector.fullRefresh()
            }
        }
    }
    
    @IBAction func doubleClickAction(_ sender: NSTableView!) {
        
        if sender.clickedColumn != 0 {
            
            doubleClickAction(row: sender.clickedRow)
        }
    }
    
    func doubleClickAction(row: Int) {
        
        if let cpu = cpu {

            if let addr = addrInRow[row] {

                if cpu.hasBreakpoint(atAddr: addr) {
                    emu?.put(.BP_REMOVE_AT, value: addr)
                } else {
                    emu?.put(.BP_SET_AT, value: addr)
                }

                inspector.fullRefresh()
            }
        }
    }
    
    func setHex(_ value: Bool) {
        
        hex = value
        refresh(full: true)
    }
}

extension InstrTableView: NSTableViewDataSource {
    
    func numberOfRows(in tableView: NSTableView) -> Int {
        
        return numRows
    }
    
    func tableView(_ tableView: NSTableView, objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        
        switch tableColumn?.identifier.rawValue {
            
        case "break" where addrInRow[row] == alertAddr:
            return "⚠️"
        case "break" where bpInRow[row] == .enabled:
            return "\u{26D4}" // "⛔" ("\u{1F534}" // "🔴")
        case "break" where bpInRow[row] == .disabled:
            return "\u{26AA}" // "⚪" ("\u{2B55}" // "⭕")
        case "addr":
            return addrStrInRow[row]
        case "data":
            return dataInRow[row]
        case "instr":
            return instrInRow[row]
        default:
            return ""
        }
    }
}

extension InstrTableView: NSTableViewDelegate {
    
    func tableView(_ tableView: NSTableView, willDisplayCell cell: Any, for tableColumn: NSTableColumn?, row: Int) {
        
        let cell = cell as? NSTextFieldCell
        
        if bpInRow[row] == .enabled {
            cell?.textColor = NSColor.systemRed
        } else if bpInRow[row] == .disabled {
            cell?.textColor = NSColor.disabledControlTextColor
        } else {
            cell?.textColor = NSColor.labelColor
        }
    }
}

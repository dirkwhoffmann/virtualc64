//
//  CPUTableView.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 16.04.18.
//

import Foundation

class CPUTableView : NSTableView {
    
    
    var c : MyController!
    
    var instructionAtRow : [Int:DisassembledInstruction] = [:]
    var rowForAddress : [UInt16:Int] = [:]
    
    var hex = true
    
    override func awakeFromNib() {
        
        delegate = self
        dataSource = self
        target = self
        doubleAction = #selector(doubleClickAction(_:))
        
        refresh()
    }
    
    @IBAction func doubleClickAction(_ sender: Any!) {
    
        let sender = sender as! NSTableView
        let row = sender.selectedRow
        
        if let instr = instructionAtRow[row] {
            c.c64.cpu.toggleHardBreakpoint(instr.addr)
            reloadData()
        }
    }

    func updateDisplayedAddresses(startAddr: UInt16) {
        
        var addr = startAddr
        
        rowForAddress = [:]
        
        for i in 0...255 {
            instructionAtRow[i] = c.c64.cpu.disassemble(addr, hex: hex)
            rowForAddress[addr] = i
            addr += UInt16(instructionAtRow[i]!.size)
        }
    }

    func refresh() {
        
        // let addr = c.c64.cpu.pc()
        
        if let row = rowForAddress[c.c64.cpu.pc()] {
            
            // If PC points to an address which is already displayed,
            // we simply select the corresponding row.
            scrollRowToVisible(row)
            selectRowIndexes([row], byExtendingSelection: false)
            
        } else {
            
            // If PC points to an address that is not displayed,
            // we update the whole view and display PC in the first row.
            updateDisplayedAddresses(startAddr: c.c64.cpu.pc())
            scrollRowToVisible(0)
            selectRowIndexes([0], byExtendingSelection: false)
        }
        
        reloadData()
    }
}

extension CPUTableView : NSTableViewDataSource {
   
    func numberOfRows(in tableView: NSTableView) -> Int {
        return 256;
    }
        
    func tableView(_ tableView: NSTableView, objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        
        let instr = instructionAtRow[row]!
        let addr = instr.addr
        let size = instr.size
        
        switch(tableColumn?.identifier.rawValue) {
            
        case "addr":
            return addr
           
        case "data01":
            return size > 0 ? c.c64.mem.read(addr) : nil

        case "data02":
            return size > 1 ? c.c64.mem.read(addr + 1) : nil

        case "data03":
            return size > 2 ? c.c64.mem.read(addr + 2) : nil

        case "ascii":
            return "HOLLA DIE WALDFEE"
            // return instr.command
            
        default:
            return "?"
        }
    }
}

extension CPUTableView : NSTableViewDelegate {
    
    func tableView(_ tableView: NSTableView, willDisplayCell cell: Any, for tableColumn: NSTableColumn?, row: Int) {
        
        let cell = cell as! NSTextFieldCell
        let instr = instructionAtRow[row]!
        
        if c.c64.cpu.breakpoint(instr.addr) == Int32(HARD_BREAKPOINT.rawValue) {
            cell.textColor = NSColor.red
        } else {
            cell.textColor = NSColor.black
        }
    }
}

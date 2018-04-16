//
//  CPUTableView.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 16.04.18.
//

import Foundation

class CPUTableView : NSTableView {
    
    
    var c : MyController!
    var addressForRow : [Int:UInt16] = [:]
    var rowForAddress : [UInt16:Int] = [:]
    
    override func awakeFromNib() {
        
        delegate = self
        dataSource = self
        target = self
        doubleAction = #selector(doubleClickAction(_:))
        
        reloadData()
    }
    
    @IBAction func doubleClickAction(_ sender: Any!) {
    
        let sender = sender as! NSTableView
        let row = sender.selectedRow
        
        if let addr = addressForRow[row] {
            c.c64.cpu.toggleHardBreakpoint(addr)
            reloadData()
        }
    }

    func updateDisplayedAddresses(startAddr: UInt16) {
    }

    func refresh() {
        
    }
}

extension CPUTableView : NSTableViewDelegate {
    
}

extension CPUTableView : NSTableViewDataSource {
   
    func numberOfRows(in tableView: NSTableView) -> Int {
        return 256;
    }
        
    func tableView(_ tableView: NSTableView, objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        
        let addr = addressForRow[row]!
        let length = c.c64.cpu.length(ofInstruction: c.c64.mem.read(addr))
        
        switch(tableColumn?.identifier.rawValue) {
            
        case "addr":
            return addr
           
        case "data01":
            return length > 0 ? c.c64.mem.read(addr) : nil

        case "data02":
            return length > 1 ? c.c64.mem.read(addr + 1) : nil

        case "data03":
            return length > 2 ? c.c64.mem.read(addr + 2) : nil

        case "ascii":
            
            let instr = c.c64.cpu.disassemble(addr, hex: c.hex)
            return instr.command
            
        default:
            return "?"
        }
    }
    
    func tableView(_ tableView: NSTableView, willDisplayCell cell: Any, for tableColumn: NSTableColumn?, row: Int) {
        
        let cell = cell as! NSTextFieldCell
        let addr = addressForRow[row]!
        
        if c.c64.cpu.breakpoint(addr) == Int32(HARD_BREAKPOINT.rawValue) {
            cell.textColor = NSColor.red
        } else {
            cell.textColor = NSColor.black
        }
    }
}

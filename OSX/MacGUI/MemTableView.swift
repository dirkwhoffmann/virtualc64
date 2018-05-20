//
//  MemTableView.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 20.05.18.
//

import Foundation

struct MemoryView {
    static let cpuView = 0
    static let ramView = 1
    static let romView = 2
    static let ioView = 3
}

struct MemoryHighlighting {
    static let none = 0
    static let basicRom = 1
    static let charRom = 2
    static let kernelRom = 3
    static let vicIO = 4
    static let sidIO = 5
    static let ciaIO = 6
    static let crtLO = 7
    static let crtHI = 8
}

@objc class MEMTableView : NSTableView {
    
    var c : MyController? = nil
    var memView = MemoryView.cpuView
    var highlighting = MemoryHighlighting.kernelRom
    
    override func awakeFromNib() {
        
        delegate = self
        dataSource = self
        target = self
    }
    
    @objc func refresh() {
        
        reloadData()
    }
    
    // Returns the memory source for the specified address
    func source(_ addr: UInt16) -> MemorySource {
        
        switch memView {
        case MemoryView.ramView:
            return M_RAM
        case MemoryView.romView:
            return M_ROM
        case MemoryView.ioView:
            return M_IO
        default:
            return c?.c64.mem.peekSource(addr) ?? M_RAM
        }
    }

    // Return true if the specified memory address should be displayed
    func shouldDisplay(_ addr: UInt16) -> Bool {
        
        let src = source(addr)
        switch (src) {
        case M_IO:
            return addr >= 0xD000 && addr <= 0xDFFF
        case M_ROM:
            return (addr >= 0xA000 && addr <= 0xBFFF) || (addr >= 0xD000)
        default:
            return true
        }
    }
        
    // Return true if the specified memory address should be highlighted
    func shouldHighlight(_ addr: UInt16) -> Bool {
        
        let src = source(addr)
        switch highlighting {
        case MemoryHighlighting.basicRom:
            return src == M_ROM && addr >= 0xA000 && addr <= 0xBFFF
        case MemoryHighlighting.charRom:
            return src == M_ROM && addr >= 0xD000 && addr <= 0xDFFF
        case MemoryHighlighting.kernelRom:
            return src == M_ROM && addr >= 0xE000 && addr <= 0xFFFF
        case MemoryHighlighting.vicIO:
            return src == M_IO && addr >= 0xD000 && addr <= 0xD3FF
        case MemoryHighlighting.sidIO:
            return src == M_IO && addr >= 0xD400 && addr <= 0xD7FF
        case MemoryHighlighting.ciaIO:
            return src == M_IO && addr >= 0xDC00 && addr <= 0xDDFF
        case MemoryHighlighting.crtLO:
            return src == M_CRTLO
        case MemoryHighlighting.crtHI:
            return src == M_CRTHI
        default:
            return false
        }
    }
}

extension MEMTableView : NSTableViewDataSource {
    
    func numberOfRows(in tableView: NSTableView) -> Int {
        
        return 0x10000 / 4;
    }
    
    func tableView(_ tableView: NSTableView, objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        
        var addr = UInt16(4 * row)
        
        if !shouldDisplay(addr) {
            return ""
        }
        switch(tableColumn?.identifier.rawValue) {
            
        case "addr":
            return addr
            
        case "ascii":
            var chars = [Character](repeating:".", count:4)
            let src = source(addr)
            for i in 0...3 {
                let byte = c!.c64.mem.spy(addr, source: src)
                let converted = byte // petscii2printable(byte, 45 /* '.' */)
                chars[i] = Character(UnicodeScalar(converted))
            }
            return String(chars)
            
        case "hex3":
            addr += 1
            fallthrough
            
        case "hex2":
            addr += 1
            fallthrough
            
        case "hex1":
            addr += 1
            fallthrough
            
        case "hex0":
            let src = source(addr)
            return c?.c64.mem.spy(addr, source: src) ?? ""
            
        default:
            break
        }
        
        return "";
    }
}

extension MEMTableView : NSTableViewDelegate {
    
    func tableView(_ tableView: NSTableView, willDisplayCell cell: Any, for tableColumn: NSTableColumn?, row: Int) {
        
        let cell = cell as! NSTextFieldCell
        cell.textColor = NSColor.blue
    }
}

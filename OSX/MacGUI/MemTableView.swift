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
    static let rom = 1
    static let romBasic = 2
    static let romChar = 3
    static let romKernal = 4
    static let crt = 5
    static let io = 6
    static let ioVic = 7
    static let ioSid = 8
    static let ioCia = 9
}

class MemTableView : NSTableView {
    
    var c : MyController? = nil
    var cbmfont = NSFont.init(name: "C64ProMono", size: 9)
    private var memView = MemoryView.cpuView
    private var highlighting = MemoryHighlighting.none
    
    override func awakeFromNib() {
        
        delegate = self
        dataSource = self
        target = self
    }
    
    func refresh() {
        
        reloadData()
    }
    
    func setMemView(_ value : Int) {
        memView = value
        refresh()
    }
    
    func setHighlighting(_ value : Int) {
        highlighting = value
        refresh()
    }
    
    // Returns the memory source for the specified address
    func source(_ addr: UInt16) -> MemoryType {
        
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
        case MemoryHighlighting.rom:
            return src == M_ROM || src == M_CRTLO || src == M_CRTHI
        case MemoryHighlighting.romBasic:
            return src == M_ROM && addr >= 0xA000 && addr <= 0xBFFF
        case MemoryHighlighting.romChar:
            return src == M_ROM && addr >= 0xD000 && addr <= 0xDFFF
        case MemoryHighlighting.romKernal:
            return src == M_ROM && addr >= 0xE000 && addr <= 0xFFFF
        case MemoryHighlighting.crt:
            return src == M_CRTLO || src == M_CRTHI
        case MemoryHighlighting.io:
            return src == M_IO
        case MemoryHighlighting.ioVic:
            return src == M_IO && addr >= 0xD000 && addr <= 0xD3FF
        case MemoryHighlighting.ioSid:
            return src == M_IO && addr >= 0xD400 && addr <= 0xD7FF
        case MemoryHighlighting.ioCia:
            return src == M_IO && addr >= 0xDC00 && addr <= 0xDDFF
        default:
            return false
        }
    }
}

extension MemTableView : NSTableViewDataSource {
    
    func numberOfRows(in tableView: NSTableView) -> Int {
        
        return 0x10000 / 4;
    }
    
    func tableView(_ tableView: NSTableView, objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        
        var addr = UInt16(4 * row)
        let src = source(addr)
        
        switch(tableColumn?.identifier.rawValue) {
            
        case "src":
            return (src == M_RAM || src == M_PP) ? "RAM" :
                (src == M_ROM) ? "ROM" :
                (src == M_IO) ? "IO" :
                (src == M_CRTLO || src == M_CRTHI) ? "CRT" : ""
            
        case "addr":
            return addr
            
        case "ascii":
            if !shouldDisplay(addr) {
                break
            }
            var str = ""
            let src = source(addr)
            for i in 0...3 {
                var byte = Int(c!.c64.mem.spypeek(addr + UInt16(i), source: src))
                if (byte < 32 || byte > 90) { byte = 46 }
                let scalar = UnicodeScalar(byte + 0xE000)
                str.unicodeScalars.append(scalar!)
            }
            return str
            
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
            if !shouldDisplay(addr) {
                break
            }
            let src = source(addr)
            return c?.c64.mem.spypeek(addr, source: src) ?? ""
            
        default:
            break
        }
        
        return "";
    }
}

extension MemTableView : NSTableViewDelegate {
    
    func tableView(_ tableView: NSTableView, willDisplayCell cell: Any, for tableColumn: NSTableColumn?, row: Int) {
        
        let cell = cell as! NSTextFieldCell

        if (tableColumn?.identifier.rawValue == "src") {
            cell.font = NSFont.systemFont(ofSize: 9)
            cell.textColor = .gray
        } else {
            cell.textColor = .black
        }
        
        if (tableColumn?.identifier.rawValue == "ascii") {
            cell.font = cbmfont
        }
        
        if shouldHighlight(UInt16(4 * row)) {
            cell.textColor = .systemRed
        } 
    }
    
    func tableView(_ tableView: NSTableView, setObjectValue object: Any?, for tableColumn: NSTableColumn?, row: Int) {
        
        var addr = UInt16(4 * row)
        switch(tableColumn?.identifier.rawValue) {
        case "hex0": break
        case "hex1": addr += 1; break
        case "hex2": addr += 2; break
        case "hex3": addr += 3; break
        default: return
        }
        
        let target = source(addr)
        if (target == M_CRTLO || target == M_CRTHI) {
            NSSound.beep()
            return
        }
        if let value = object as? UInt8 {
            track("Poking \(value) to \(addr) (target = \(target))")
            c?.c64.mem.poke(addr, value: value, target: target)
        }
    }
}

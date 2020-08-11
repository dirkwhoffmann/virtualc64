// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

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

class MemTableView: NSTableView {
    
    @IBOutlet weak var inspector: Inspector!

    var c64: C64Proxy { return inspector.parent.c64 }
    var mem: MemoryProxy { return c64.mem }

    // var c: MyController?
    var cbmfont = NSFont.init(name: "C64ProMono", size: 9)
    private var memView = MemoryView.cpuView
    private var highlighting = MemoryHighlighting.none
    
    override func awakeFromNib() {
        
        delegate = self
        dataSource = self
        target = self
    }
    
    func refresh(count: Int = 0, full: Bool = false) {
        
        if full {
            
            assignFormatter(inspector.fmt16, column: "addr")
            assignFormatter(inspector.fmt8, column: "hex0")
            assignFormatter(inspector.fmt8, column: "hex1")
            assignFormatter(inspector.fmt8, column: "hex2")
            assignFormatter(inspector.fmt8, column: "hex3")
            assignFormatter(inspector.fmt8, column: "hex4")
        }
        
        reloadData()
    }
    
    func setMemView(_ value: Int) {
        memView = value
        refresh()
    }
    
    func setHighlighting(_ value: Int) {
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
            return mem.peekSource(addr)
        }
    }

    // Return true if the specified memory address should be displayed
    func shouldDisplay(_ addr: UInt16) -> Bool {
        
        let src = source(addr)
        switch src {
        case M_IO:
            return addr >= 0xD000 && addr <= 0xDFFF
        case M_ROM:
            return (addr >= 0xA000 && addr <= 0xBFFF) || (addr >= 0xD000)
        default:
            return true
        }
    }
}

extension MemTableView: NSTableViewDataSource {
    
    func numberOfRows(in tableView: NSTableView) -> Int {
        
        return 0x10000 / 16
    }
    
    func tableView(_ tableView: NSTableView, objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        
        var addr = UInt16(4 * row)
        let src = source(addr)

        func value(_ addr: UInt16) -> Any? {
            
            if !shouldDisplay(addr) { return "" }
            let src = source(addr)
            return mem.spypeek(addr, source: src)
        }
                
        switch tableColumn?.identifier.rawValue {
            
        case "bank":
            
            switch src {
            case M_RAM:   return "RAM"
            case M_PP:    return "RAM+PP"
            case M_ROM:   return "ROM"
            case M_IO:    return "IO"
            case M_CRTLO: return "CRTLO"
            case M_CRTHI: return "CRTHI"
            default:      return ""
            }

        case "addr":
            return addr
            
        case "ascii":
            /*
            if !shouldDisplay(addr) {
                break
            }
            var str = ""
            let src = source(addr)
            for i in 0...3 {
                var byte = Int(mem.spypeek(addr + UInt16(i), source: src))
                if byte < 32 || byte > 90 { byte = 46 }
                let scalar = UnicodeScalar(byte + 0xE000)
                str.unicodeScalars.append(scalar!)
            }
            return str
            */
            return "0123456789ABCDEF"
        
        case "hex0": return value(addr)
        case "hex1": return value(addr + 0x1)
        case "hex2": return value(addr + 0x2)
        case "hex3": return value(addr + 0x3)
        case "hex4": return value(addr + 0x4)
        case "hex5": return value(addr + 0x5)
        case "hex6": return value(addr + 0x6)
        case "hex7": return value(addr + 0x7)
        case "hex8": return value(addr + 0x8)
        case "hex9": return value(addr + 0x9)
        case "hexA": return value(addr + 0xA)
        case "hexB": return value(addr + 0xB)
        case "hexC": return value(addr + 0xC)
        case "hexD": return value(addr + 0xD)
        case "hexE": return value(addr + 0xE)
        case "hexF": return value(addr + 0xF)
            
        default:
            return "???"
        }
    }
}

extension MemTableView: NSTableViewDelegate {
    
    func tableView(_ tableView: NSTableView, willDisplayCell cell: Any, for tableColumn: NSTableColumn?, row: Int) {
        
        let cell = cell as? NSTextFieldCell

        if tableColumn?.identifier.rawValue == "src" {
            cell?.font = NSFont.systemFont(ofSize: 9)
            cell?.textColor = .gray
        } else {
            cell?.textColor = NSColor.textColor
        }
        
        if tableColumn?.identifier.rawValue == "ascii" {
            cell?.font = cbmfont
        }
    }
    
    func tableView(_ tableView: NSTableView, setObjectValue object: Any?, for tableColumn: NSTableColumn?, row: Int) {
        
        var addr = UInt16(4 * row)
        switch tableColumn?.identifier.rawValue {
        case "hex0": break
        case "hex1": addr += 1
        case "hex2": addr += 2
        case "hex3": addr += 3
        default: return
        }
        
        let target = source(addr)
        if target == M_CRTLO || target == M_CRTHI {
            NSSound.beep()
            return
        }
        if let value = object as? UInt8 {
            track("Poking \(value) to \(addr) (target = \(target))")
            mem.poke(addr, value: value, target: target)
        }
    }
}

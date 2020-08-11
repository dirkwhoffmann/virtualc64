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
        
        let addr = UInt16(16 * row)
        let bank = inspector.memBank[(16 * row) >> 12]!
        
        func value(_ addr: UInt16) -> Any? {
            
            if !shouldDisplay(addr) { return "" }
            let src = source(addr)
            return mem.spypeek(addr, source: src)
        }
                
        switch tableColumn?.identifier.rawValue {
            
        case "bank":
            
            switch bank {
            case M_RAM:   return "RAM"
            case M_PP:    return row == 0 ? "RAM+PP" : "RAM"
            case M_ROM:   return "ROM"
            case M_IO:    return "IO"
            case M_CRTLO: return "CRTLO"
            case M_CRTHI: return "CRTHI"
            default:      return ""
            }

        case "addr":
            return addr
            
        case "ascii":
            return mem.txtdump(Int(addr), num: 16, src: bank)
                    
        case "hex0":
            return mem.memdump(Int(addr), num: 16, hex: inspector.hex, src: bank)
            
        default:
            return "???"
        }
        
        return ""
    }
}

extension MemTableView: NSTableViewDelegate {
    
    func tableView(_ tableView: NSTableView, willDisplayCell cell: Any, for tableColumn: NSTableColumn?, row: Int) {
        
        let cell = cell as? NSTextFieldCell

        if tableColumn?.identifier.rawValue == "bank" {
            cell?.font = NSFont.systemFont(ofSize: 10)
            cell?.textColor = .gray
        } else {
            cell?.textColor = NSColor.textColor
        }
        
        if tableColumn?.identifier.rawValue == "ascii" {
            // cell?.font = cbmfont
        }
    }
}

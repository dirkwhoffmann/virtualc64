// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class MemTableView: NSTableView {
    
    @IBOutlet weak var inspector: Inspector!

    var emu: EmulatorProxy? { return inspector.parent.emu }
    var mem: MemoryProxy? { return emu?.mem }

    override init(frame frameRect: NSRect) { super.init(frame: frameRect); commonInit() }
    required init?(coder: NSCoder) { super.init(coder: coder); commonInit() }

    func commonInit() {

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
    
    var bankType: vc64.MemType {

        let type = inspector.bankType[inspector.displayedBank]!
        return type.rawValue == 0 ? .NONE : type
    }
}

extension MemTableView: NSTableViewDataSource {
    
    func numberOfRows(in tableView: NSTableView) -> Int {
        
        return 4096 / 16 // Bank size divided by bytes per row
    }
    
    func tableView(_ tableView: NSTableView, objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        
        let base = inspector.displayedBank * 4096
        let addr = UInt16(base + 16 * row)
        
        switch tableColumn?.identifier.rawValue {
            
        case "addr":
            return addr
            
        case "ascii":
            return mem?.txtdump(Int(addr), num: 16, src: bankType) ?? ""

        case "hex0":
            return mem?.memdump(Int(addr), num: 16, hex: inspector.hex, src: bankType) ?? ""

        default:
            return "???"
        }        
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
    }
}

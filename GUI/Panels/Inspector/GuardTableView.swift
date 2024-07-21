// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class GuardTableView: NSTableView {

    @IBOutlet weak var inspector: Inspector!
    var emu: EmulatorProxy? { return inspector.parent.emu }
    var cpu: CPUProxy? { return emu?.cpu }

    // Symbols
    var symEnabled = "⛔"  // "\u{26D4}"
    var symDisabled = "⚪" // "\u{26AA}"
    var symTrash = "🗑" // "\u{1F5D1}"

    // Data caches
    var disabledCache: [Int: Bool] = [:]
    var addrCache: [Int: Int] = [:]
    var numRows = 0

    override func awakeFromNib() {

        delegate = self
        dataSource = self
        target = self

        action = #selector(clickAction(_:))
    }

    func cache() { }
    func click(row: Int, col: Int) { }
    func edit(row: Int, addr: Int) { }

    func refresh(count: Int = 0, full: Bool = false) {
        
        if full {
            assignFormatter(inspector.fmt16, column: "addr")
            cache()
            reloadData()
        }
    }

    @IBAction func clickAction(_ sender: NSTableView!) {

        click(row: sender.clickedRow, col: sender.clickedColumn)
    }
}

extension GuardTableView: NSTableViewDataSource {

    func numberOfRows(in tableView: NSTableView) -> Int {

        return numRows + 1
    }

    func tableView(_ tableView: NSTableView, objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {

        let last = row == numRows

        switch tableColumn?.identifier.rawValue {

        case "break" where disabledCache[row] == true:
            return last ? "" : symDisabled
        case "break":
            return last ? "" : symEnabled
        case "addr":
            return last ? "Add..." : addrCache[row]!
        case "delete":
            return last ? "" : symTrash

        default: return ""
        }
    }
}

extension GuardTableView: NSTableViewDelegate {

    func tableView(_ tableView: NSTableView, willDisplayCell cell: Any, for tableColumn: NSTableColumn?, row: Int) {

        if tableColumn?.identifier.rawValue == "addr" {
            if let cell = cell as? NSTextFieldCell {

                let last = row == numRows
                let disabled = !last && disabledCache[row] == true
                let selected = tableView.selectedRow == row
                let edited = tableView.editedRow == row

                cell.textColor =
                    disabled ? NSColor.disabledControlTextColor :
                    edited ? NSColor.textColor :
                    selected ? NSColor.white :
                    last ? NSColor.disabledControlTextColor :
                    NSColor.textColor
            }
        }
    }

    func tableView(_ tableView: NSTableView, shouldEdit tableColumn: NSTableColumn?, row: Int) -> Bool {

        if tableColumn?.identifier.rawValue == "addr" {
            return true
        }

        return false
    }

    func tableView(_ tableView: NSTableView, setObjectValue object: Any?, for tableColumn: NSTableColumn?, row: Int) {

        if tableColumn?.identifier.rawValue != "addr" { NSSound.beep(); return }
        guard let addr = object as? Int else { NSSound.beep(); return }

        edit(row: row, addr: addr)
        inspector.fullRefresh()
     }
}

class BreakTableView: GuardTableView {
    
    override func cache() {

        if let cpu = cpu {

            numRows = 0
            while cpu.hasBreakpoint(withNr: numRows) {

                let info = cpu.breakpoint(withNr: numRows)
                disabledCache[numRows] = !info.enabled
                addrCache[numRows] = Int(info.addr)
                numRows += 1
            }
        }
    }

    override func click(row: Int, col: Int) {

        if let cpu = cpu {

            if cpu.hasBreakpoint(withNr: row) {

                let bp = cpu.breakpoint(withNr: row)

                if col == 0 {

                    // Toggle enable flag
                    emu?.put(bp.enabled ? .BP_DISABLE_NR : .BP_ENABLE_NR, value: row)
                    inspector.fullRefresh()
                }

                if col == 0 || col == 1 {

                    // Jump to breakpoint address
                    inspector.fullRefresh()
                    inspector.cpuInstrView.jumpTo(addr: Int(bp.addr))
                }

                if col == 2 {

                    // Delete
                    emu?.put(.BP_REMOVE_NR, value: row)
                    inspector.fullRefresh()
                }
            }
        }
    }

    override func edit(row: Int, addr: Int) {

        if let cpu = cpu {

            // Abort if a breakpoint is already set
            if cpu.hasBreakpoint(atAddr: addr) { NSSound.beep(); return }

            emu?.suspend()

            if row < numRows {
                emu?.put(.BP_MOVE_TO, value: row, value2: addr)
            } else {
                emu?.put(.BP_SET_AT, value: addr)
            }

            inspector.cpuInstrView.jumpTo(addr: addr)

            emu?.resume()
        }
    }
}

class WatchTableView: GuardTableView {

    override func cache() {

        if let cpu = cpu {

            numRows = 0
            while cpu.hasWatchpoint(withNr: numRows) {

                let info = cpu.watchpoint(withNr: numRows)
                disabledCache[numRows] = !info.enabled
                addrCache[numRows] = Int(info.addr)
                numRows += 1
            }

            symEnabled = "⚠️"
        }
    }

    override func click(row: Int, col: Int) {
        
        if let cpu = cpu {

            if cpu.hasWatchpoint(withNr: row) {

                let wp = cpu.watchpoint(withNr: row)

                if col == 0 {

                    emu?.put(wp.enabled ? .WP_DISABLE_NR : .WP_ENABLE_NR, value: row)
                    inspector.fullRefresh()
                }

                if col == 2 {

                    // Delete
                    emu?.put(.WP_REMOVE_NR, value: row)
                    inspector.fullRefresh()
                }
            }
        }
    }
    
    override func edit(row: Int, addr: Int) {
        
        if let cpu = cpu {

            // Abort if a watchpoint is already set
            if cpu.hasBreakpoint(atAddr: addr) { NSSound.beep(); return }

            emu?.suspend()

            if row < numRows {
                emu?.put(.WP_MOVE_TO, value: row)
            } else {
                emu?.put(.WP_SET_AT, value: addr)
            }

            emu?.resume()
        }
    }
}

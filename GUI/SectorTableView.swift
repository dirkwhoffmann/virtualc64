// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class SectorTableView: NSTableView, NSTableViewDelegate {
    
    @IBOutlet weak var inspector: Inspector!
   
    var c64: C64Proxy { return inspector.parent.c64 }
    var drive: DriveProxy { return inspector.drive }
    var halftrack: Int { return inspector.selectedHalftrack }
    var sector: Int { return inspector.selectedSector }
    
    // Data caches

    // Mapping from table rows to sector numbers
    var sectorForRow: [Int: Int] = [:]

    // var bankCache: [Int: MemoryType] = [:]

    override func awakeFromNib() {

        delegate = self
        dataSource = self
        target = self
        action = #selector(clickAction(_:))
    }
    
    func cache() {
                
        // Map row numbers to sector numbers
        sectorForRow = [:]
        
        if halftrack >= 0 {
            var row = 0
            for i in 0 ... Int(maxNumberOfSectors - 1) {
                let info = drive.disk.sectorInfo(Sector(i))
                if info.headerBegin != info.headerEnd {
                    sectorForRow[row] = i
                    row += 1
                }
            }
        }
    }
    
    func refresh(count: Int = 0, full: Bool = false) {
        
        if full {
            cache()
            if sector < 0 {
                selectRowIndexes([], byExtendingSelection: false)
            }
        }
        reloadData()
    }

    @IBAction func clickAction(_ sender: NSTableView!) {

        track()
        
        inspector.selectedSector = sender.clickedRow
        inspector.fullRefresh()
    }
}

extension SectorTableView: NSTableViewDataSource {
    
    func numberOfRows(in tableView: NSTableView) -> Int {
        
        return sectorForRow.count
    }
        
    func tableView(_ tableView: NSTableView, objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        
        let sector = sectorForRow[row]!
        return "Sector \(sector)"
    }
}

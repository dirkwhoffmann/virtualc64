// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class SectorTableView: NSTableView, NSTableViewDelegate {
    
    @IBOutlet weak var inspector: DiskInspector!

    var sector: Int? { return inspector.sector }
    var halftrack: Int? { return inspector.halftrack }
    var analyzer: DiskAnalyzerProxy? { return inspector.analyzer }
    
    //
    // Data cache
    //
    
    // Mapping from table rows to sector numbers
    var sectorForRow: [Int: Int] = [:]
    
    // Indicates if the cached data needs an update
    var isDirty = false
    
    override func awakeFromNib() {

        delegate = self
        dataSource = self
        target = self
    }
    
    func cache() {
                
        // Map row numbers to sector numbers
        sectorForRow = [:]
        
        if halftrack != nil && analyzer != nil {
            
            var row = 0
            for i in 0 ... Int(vc64.highestSector) {
                let info = analyzer!.sectorInfo(halftrack!, sector: vc64.Sector(i))
                if info.headerBegin != info.headerEnd {
                    sectorForRow[row] = i
                    row += 1
                }
            }
        }
        
        isDirty = false
    }
    
    func refresh() {

        if isDirty { cache() }
        
        reloadData()
        scrollToRow()
    }

    func scrollToRow() {

        if let s = sector {

            scrollRowToVisible(s)
            selectRowIndexes([s], byExtendingSelection: false)
        }
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
    
    func tableView(_ tableView: NSTableView, shouldSelectRow row: Int) -> Bool {
        
        if let sector = sectorForRow[row] {
            
            inspector.setSelectedSector(sector)
        }
        return true
    }
}

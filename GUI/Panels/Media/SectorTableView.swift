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

    var sector: Int { return inspector.selectedSector }
    var halftrack: Halftrack? { return inspector.halftrack }
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
            for i in 0 ... Int(highestSector) {
                let info = analyzer!.sectorInfo(halftrack!, sector: Sector(i))
                if info.headerBegin != info.headerEnd {
                    sectorForRow[row] = i
                    row += 1
                }
            }
        }
        
        isDirty = false
    }
    
    func refresh() {

        // Update cached data if neccessary
        if isDirty { cache() }
        
        // Unselect all items if no sector is selected
        selectRowIndexes([], byExtendingSelection: false)

        reloadData()
    }

    func scrollToRow() {

        if sector >= 0 {

            selectRowIndexes([sector], byExtendingSelection: false)
            scrollRowToVisible(sector)
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

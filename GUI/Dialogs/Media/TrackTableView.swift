// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class TrackTableView: NSTableView, NSTableViewDelegate {
    
    @IBOutlet weak var inspector: DiskInspector!
   
    var halftrack: Int? { return inspector.halftrack }
    var analyzer: DiskAnalyzerProxy? { return inspector.analyzer }

    // Number of items in the table view (0 if no disk is present)
    var numItems = 0
    
    // Indicates if halftracks should be shown, too
    var showHalftracks: Bool { return inspector.showHalftracks }
    
    override func awakeFromNib() {

        delegate = self
        dataSource = self
        target = self
        
        scrollRowToVisible(0)
        selectRowIndexes([0], byExtendingSelection: false)

    }
    
    func cache() {
        
        if analyzer != nil {
            numItems = Int(showHalftracks ? vc64.highestHalftrack : vc64.highestTrack)
        } else {
            numItems = 0
        }
    }
    
    func refresh() {

        // Update cached data
        cache()

        // Select the correct row
        let rows: IndexSet = halftrack == nil ? [] : [Int(halftrack!) - 1]
        selectRowIndexes(rows, byExtendingSelection: false)

        reloadData()
    }
    
    func scrollToRow() {
        
        if let ht = halftrack {

            let row = showHalftracks ? ht - 1 : ht / 2
            selectRowIndexes([row], byExtendingSelection: false)
            scrollRowToVisible(row)
        }
    }
}

extension TrackTableView: NSTableViewDataSource {
    
    func numberOfRows(in tableView: NSTableView) -> Int {
        
        return numItems
    }
        
    func tableView(_ tableView: NSTableView, objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {

        if showHalftracks {

            let halftrack = row + 1
            let fulltrack = (halftrack + 1) / 2
            return row % 2 == 0 ? "Track \(fulltrack)" : "Track \(fulltrack).5"

        } else {

            let fulltrack = row + 1
            return "Track \(fulltrack)"
        }
    }
    
    func tableView(_ tableView: NSTableView, shouldSelectRow row: Int) -> Bool {

        // inspector.setSelectedSector(-1)

        if showHalftracks {
            inspector.setSelectedHalftrack(row + 1)
        } else {
            inspector.setSelectedTrack(row + 1)
        }

        return true
    }
}

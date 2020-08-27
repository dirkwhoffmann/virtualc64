// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class TrackTableView: NSTableView, NSTableViewDelegate {
    
    @IBOutlet weak var inspector: Inspector!
   
    var c64: C64Proxy { return inspector.parent.c64 }
    var drive: DriveProxy { return inspector.drive }
    var halftrack: Int { return inspector.selectedHalftrack }

    // Number of items in the table view (0 if no disk is present)
    var numItems = 0
    
    // Indicates if halftracks should be shown, too
    var showHalftracks = false
    
    override func awakeFromNib() {

        delegate = self
        dataSource = self
        target = self
        
        scrollRowToVisible(0)
        selectRowIndexes([0], byExtendingSelection: false)

    }
    
    func cache() {
        
        numItems = Int(showHalftracks ? highestHalftrack : highestTrack)
        if !drive.hasDisk() { numItems = 0}
    }
    
    func refresh(count: Int = 0, full: Bool = false) {
        
        if full {

            // Update cached data
            cache()
            
            // Select the correct row
            let rows: IndexSet = halftrack <= 0 ? [] : [Int(halftrack) - 1]
            selectRowIndexes(rows, byExtendingSelection: false)
        }
        reloadData()
    }
    
    func scrollToRow() {
        
        if halftrack > 0 {
            let row = Int(halftrack) - 1
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
        
        // Display mode 1: Full tracks, only
        if !showHalftracks { return "Track \(row + 1)" }
        
        // Display mode 2: Full tracks + half tracks
        let fulltrack = (row + 1) / 2
        return row % 2 == 0 ? "Track \(fulltrack)" : "Halftrack \(fulltrack).5"
    }
    
    func tableView(_ tableView: NSTableView, shouldSelectRow row: Int) -> Bool {
                
        let halftrack = showHalftracks ? row + 1 : 2 * row + 1
        inspector.setSelectedSector(-1)
        inspector.setSelectedHalftrack(halftrack)
        return true
    }
}

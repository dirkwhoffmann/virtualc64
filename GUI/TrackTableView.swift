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

    // Number of tracks of the displayed disk (0 if no disk is present)
    var numTracks = 0
    
    // Indicates if halftracks should be shown, too
    var showHalftracks = false
    
    // Data caches
    // var bankCache: [Int: MemoryType] = [:]

    override func awakeFromNib() {

        delegate = self
        dataSource = self
        target = self
        action = #selector(clickAction(_:))
        
        scrollRowToVisible(0)
        selectRowIndexes([0], byExtendingSelection: false)

    }
    
    func cache() {

        if drive.hasDisk() {
            track("cache: hasDisk")
            numTracks = Int(showHalftracks ? maxNumberOfHalftracks : maxNumberOfTracks)
        } else {
            track("cache: no disk")
            numTracks = 0
        }
    }
    
    func refresh(count: Int = 0, full: Bool = false) {
        
        if full {
            cache()
        }
        reloadData()
    }

    @IBAction func clickAction(_ sender: NSTableView!) {
        
        track()
        
        let row = sender.clickedRow
        if row < 0 { return }
        
        let halftrack = Halftrack(showHalftracks ? row + 1 : 2 * row + 1)
        inspector.selectedSector = -1
        inspector.selectedHalftrack = Int(halftrack)
        inspector.fullRefresh()
    }
}

extension TrackTableView: NSTableViewDataSource {
    
    func numberOfRows(in tableView: NSTableView) -> Int {
        
        return numTracks
    }
        
    func tableView(_ tableView: NSTableView, objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        
        // Display mode 1: Full tracks, only
        if !showHalftracks { return "Track \(row + 1)" }
        
        // Display mode 2: Full tracks + half tracks
        let fulltrack = (row + 1) / 2
        return row % 2 == 0 ? "Track \(fulltrack)" : "Halftrack \(fulltrack).5"
    }
}

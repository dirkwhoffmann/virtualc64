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

    // This table can display tracks or sectors
    var displayTracks = true

    // Number of tracks and sectors to select from
    var numHalftracks = 42
    var numSectors = 42
    
    // Data caches
    // var bankCache: [Int: MemoryType] = [:]

    override func awakeFromNib() {

        delegate = self
        dataSource = self
        target = self
        action = #selector(clickAction(_:))
    }
    
    func cache() {

    }
    
    func refresh(count: Int = 0, full: Bool = false) {
        
        cache()
        reloadData()
    }

    @IBAction func clickAction(_ sender: NSTableView!) {

        // inspector.jumpTo(bank: sender.clickedRow)
    }
}

extension TrackTableView: NSTableViewDataSource {
    
    func numberOfRows(in tableView: NSTableView) -> Int {
        
        if displayTracks {
            return numHalftracks
        } else {
            return numSectors
        }
    }
        
    func tableView(_ tableView: NSTableView, objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        
        let halftrack = row + 1
        if halftrack % 2 == 1 {
            return "Track \(halftrack)"
        } else {
            return "Track \(Double(halftrack) / 2 + 0.5)"
        }
    }
}

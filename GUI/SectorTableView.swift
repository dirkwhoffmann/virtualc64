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

    }
}

extension SectorTableView: NSTableViewDataSource {
    
    func numberOfRows(in tableView: NSTableView) -> Int {
        
        return 42
    }
        
    func tableView(_ tableView: NSTableView, objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        
        return "Sector \(row)"
    }
}

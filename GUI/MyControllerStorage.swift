// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import Foundation

extension MyController {
        
    //
    // Snapshots
    //
    
    func load(snapshot: SnapshotProxy?) {
        
        if snapshot == nil { return }
        
        c64.suspend()
        c64.load(fromSnapshot: snapshot)
        c64.resume()
    }
        
    func restoreSnapshot(item: Int) -> Bool {
        
        if let snapshot = mydocument.snapshots.element(at: item) {
            load(snapshot: snapshot)
            return true
        }
        
        return false
    }
    
    func restoreLatestSnapshot() -> Bool {
        
        let count = mydocument!.snapshots.count
        return count > 0 ? restoreSnapshot(item: count - 1) : false
    }
}

// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

// import Foundation

extension MyController {

    var snapshotCount: Int { return mydocument!.snapshots.count }

    func restoreSnapshot(item: Int) throws {
        
        if let snapshot = mydocument.snapshots.element(at: item) {
            try emu?.flash(snapshot)
        }
    }
    
    func restoreLatestSnapshot() throws {
        
        let count = mydocument!.snapshots.count
        if count > 0 { try restoreSnapshot(item: count - 1) }
    }
}

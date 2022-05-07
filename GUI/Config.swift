// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

//
// Debug settings
//

public extension Int {

    static let defaults     = 0     // User defaults
    static let events       = 0     // Event processing
    static let exec         = 0     // Execution of external tools
    static let hid          = 0     // Human interface devices
    static let media        = 0     // Media files, screenshots, snapshots
    static let lifetime     = 1     // Object management, delegation calls
    static let shutdown     = 0     // Termination procedure
}

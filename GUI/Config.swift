// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

//
// Build settings
//

struct BuildSettings {

    // Determines how the GUI interacts with the emulator's message system.
    // If true, the GUI registers a callback function in the emulator core
    // to process messages as they arrive. If false, the GUI polls messages
    // from the queue during each update cycle.
    
    static let msgCallback = true
}

//
// Debug settings
//

public extension Int {

    static let audio        = 0     // Audio unit
    static let config       = 0     // Configuration
    static let defaults     = 0     // User defaults
    static let dragndrop    = 0     // Drag and drop
    static let events       = 0     // Event processing
    static let exec         = 0     // Execution of external tools
    static let hid          = 0     // Human interface devices
    static let lifetime     = 0     // Object management, delegation calls
    static let media        = 0     // Media files, screenshots, snapshots
    static let metal        = 0     // GPU drawing, shaders
    static let vsync        = 0     // VSYNC mode
    static let shutdown     = 0     // Termination procedure
}

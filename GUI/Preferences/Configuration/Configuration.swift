// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class Configuration {
    
    var parent: MyController!
    var c64: C64Proxy { return parent.c64 }
    var renderer: Renderer { return parent.renderer }
    var gamePadManager: GamePadManager { return parent.gamePadManager }
    
    //
    // Rom settings
    //
    
    var basicRomURL: URL = URL(fileURLWithPath: "/")
    var charRomURL: URL = URL(fileURLWithPath: "/")
    var kernalRomURL: URL = URL(fileURLWithPath: "/")
    var vc1541RomURL: URL = URL(fileURLWithPath: "/")

    //
    // Hardware settings
    //
    
    var vicModel: Int {
        get { return c64.vic.model() }
        set { c64.vic.setModel(newValue) }
    }
    
    
    
    
    
    init(with controller: MyController) { parent = controller }
}

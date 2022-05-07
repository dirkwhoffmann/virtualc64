// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class MediaManager {

    var controller: MyController!
    var document: MyDocument!
    var c64: C64Proxy!

    init(with controller: MyController) {

        debug(.lifetime, "Creating media manager")
        
        self.controller = controller
        self.document = controller.mydocument
        self.c64 = controller.c64
    }
}

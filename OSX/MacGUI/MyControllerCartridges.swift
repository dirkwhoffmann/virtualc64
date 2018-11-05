//
// This file is part of VirtualC64 - A user-friendly Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

extension MyController {

    @IBAction func cartridgeEjectAction(_ sender: Any!) {
        
        c64.detachCartridgeAndReset()
    }
}

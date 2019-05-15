//
// This file is part of VirtualC64 - A cycle accurate Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

extension MyController {
    
    @IBAction func setMemSource(_ sender: NSPopUpButton!) {
        
        memTableView.setMemView(sender.selectedTag())
    }
    
    @IBAction func setHighlighting(_ sender: NSPopUpButton!) {
        
        memTableView.setHighlighting(sender.selectedTag())
    }
}

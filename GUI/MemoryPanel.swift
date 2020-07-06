// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension MyController {
    
    @IBAction func setMemSource(_ sender: NSPopUpButton!) {
        
        memTableView.setMemView(sender.selectedTag())
    }
    
    @IBAction func setHighlighting(_ sender: NSPopUpButton!) {
        
        memTableView.setHighlighting(sender.selectedTag())
    }
}

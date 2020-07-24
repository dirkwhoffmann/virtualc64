// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension Inspector {
    
    func refreshMemory(count: Int = 0, full: Bool = false) {
        
        // if full { }
        
        // memTableView.refresh(count: count, full: full)
        // memBankTableView.refresh(count: count, full: full)
    }
    
    @IBAction func memSourceAction(_ sender: NSPopUpButton!) {
        
        track()
    }
    
    @IBAction func memSearchAction(_ sender: NSTextField!) {
        
        track()
        /*
        let input = sender.stringValue
        if let addr = Int(input, radix: 16), input != "" {
            sender.stringValue = String(format: "%06X", addr)
            setSelected(addr)
        } else {
            sender.stringValue = ""
            selected = -1
        }
        fullRefresh()
        */
    }
}

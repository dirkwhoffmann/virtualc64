// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension Inspector {
    
    func refreshDisk(count: Int = 0, full: Bool = false) {
        
        if full {
            
        }
    }
    
    @IBAction func drvDriveAction(_ sender: NSSegmentedControl!) {
        
        track(sender.selectedSegment == 0 ? "Drive 8" : "Drive 9")
    }

    @IBAction func drvGcrBytesAction(_ sender: NSSegmentedControl!) {
        
        track(sender.selectedSegment == 0 ? "GCR" : "Bytes")
    }

    @IBAction func drvHeaderDataAction(_ sender: NSSegmentedControl!) {
        
        track(sender.selectedSegment == 0 ? "Header" : "Data")
    }

    @IBAction func drvSeekAction(_ sender: NSButton!) {
        
        track("Seek")
    }

    @IBAction func drvHalftracksAction(_ sender: NSButton!) {
        
        track("Halftracks \(sender.state)")
    }

    @IBAction func drvWarningAction(_ sender: NSButton!) {
        
        track("Warning")
    }    
}

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
                        
            if !drive.hasDisk() {
                selectedHalftrack = -1
                selectedSector = -1
            }

            drvTrackTableView.refresh(count: count, full: full)
            drvSectorTableView.refresh(count: count, full: full)
            drvDiskDataView.refresh(count: count, full: full)
        }
    }
    
    func didSetHalftrack() {
        
        if selectedHalftrack >= 0 && drive.hasDisk() {
            
            // Read track data
            drive.disk.analyzeHalftrack(Halftrack(selectedHalftrack))
            
            // Check for errors
            let trackIsValid = drive.disk.numErrors() == 0
            drvWarningText.isHidden = trackIsValid
            drvWarningButton.isHidden = trackIsValid
            
            // Let the data view know about the change
            drvDiskDataView.updateTrackData()
        }
    }
    
    func didSetSector() {
            
        // Let the data view know about the change
        drvDiskDataView.updateSectorData()
    }
        
    @IBAction func drvDriveAction(_ sender: NSSegmentedControl!) {
        
        track()
        selectedDrive = sender.selectedSegment == 0 ? DRIVE8 : DRIVE9
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
        drvTrackTableView.showHalftracks = sender.state == .on
        fullRefresh()
    }
    
    @IBAction func drvWarningAction(_ sender: NSButton!) {
        
        track("Warning")
        
        let nibName = NSNib.Name("DiskErrors")
        let controller = DiskErrorController.init(windowNibName: nibName)
        controller.parent = self
        
        window!.beginSheet(controller.window!, completionHandler: { result in
            if result == NSApplication.ModalResponse.OK {
                controller.cleanup()
            }
        })
    }    
}

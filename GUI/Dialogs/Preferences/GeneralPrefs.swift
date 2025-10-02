// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension PreferencesController {
    
    func refreshGeneralTab() {

        if let emu = emu {
            
            // Initialize combo boxes
            if genFFmpegPath.tag == 0 {

                genFFmpegPath.tag = 1

                for i in 0...5 {
                    if let path = emu.recorder.findFFmpeg(i) {
                        genFFmpegPath.addItem(withObjectValue: path)
                    } else {
                        break
                    }
                }
            }

            // Snapshots
            genAutoSnapshots.state = pref.autoSnapshots ? .on : .off
            genSnapshotInterval.integerValue = pref.snapshotInterval
            genSnapshotInterval.isEnabled = pref.autoSnapshots

            // Screenshots
            genScreenshotSourcePopup.selectItem(withTag: pref.screenshotSource)
            genScreenshotTargetPopup.selectItem(withTag: pref.screenshotTargetIntValue)

            // Screen captures
            let hasFFmpeg = emu.recorder.hasFFmpeg
            genFFmpegPath.stringValue = emu.recorder.path
            genFFmpegPath.textColor = hasFFmpeg ? .textColor : .warningColor
            genSource.selectItem(withTag: pref.captureSource)
            genBitRate.stringValue = "\(pref.bitRate)"
            genAspectX.integerValue = pref.aspectX
            genAspectY.integerValue = pref.aspectY
            genSource.isEnabled = hasFFmpeg
            genBitRate.isEnabled = hasFFmpeg
            genAspectX.isEnabled = hasFFmpeg
            genAspectY.isEnabled = hasFFmpeg

            // Fullscreen
            genAspectRatioButton.state = pref.keepAspectRatio ? .on : .off
            genExitOnEscButton.state = pref.exitOnEsc ? .on : .off

            // Miscellaneous
            genEjectUnasked.state = pref.ejectWithoutAsking ? .on : .off
            genCloseWithoutAskingButton.state = pref.closeWithoutAsking ? .on : .off
            genPauseInBackground.state = pref.pauseInBackground ? .on : .off
        }
    }
    
    func selectGeneralTab() {

        refreshGeneralTab()
    }
        
    //
    // Action methods (Snapshots)
    //
    
    @IBAction func genAutoSnapshotAction(_ sender: NSButton!) {
        
        pref.autoSnapshots = sender.state == .on
        refresh()
    }
    
    @IBAction func genSnapshotIntervalAction(_ sender: NSTextField!) {
        
        if sender.integerValue > 0 {
            pref.snapshotInterval = sender.integerValue
        }
        refresh()
    }
    
    //
    // Action methods (Screenshots)
    //

    @IBAction func genScreenshotSourceAction(_ sender: NSPopUpButton!) {
        
        pref.screenshotSource = sender.selectedTag()
        refresh()
    }
    
    @IBAction func genScreenshotTargetAction(_ sender: NSPopUpButton!) {
        
        pref.screenshotTargetIntValue = sender.selectedTag()
        refresh()
    }
    
    //
    // Action methods (Screen captures)
    //

    @IBAction func genPathAction(_ sender: NSComboBox!) {

        let path = sender.stringValue
        pref.ffmpegPath = path
        refresh()

        // Display a warning if the recorder is inaccessible
        let fm = FileManager.default
        if fm.fileExists(atPath: path), !fm.isExecutableFile(atPath: path) {

            parent.showAlert(.recorderSandboxed(exec: path), window: window)
        }
    }
    
    @IBAction func genCaptureSourceAction(_ sender: NSPopUpButton!) {
        
        pref.captureSource = sender.selectedTag()
        refresh()
    }

    @IBAction func genBitRateAction(_ sender: NSComboBox!) {
        
        var input = sender.objectValueOfSelectedItem as? Int
        if input == nil { input = sender.integerValue }
        
        if let bitrate = input {
            pref.bitRate = bitrate
        }
        refresh()
    }

    @IBAction func genAspectXAction(_ sender: NSTextField!) {
        
        pref.aspectX = sender.integerValue
        refresh()
    }

    @IBAction func genAspectYAction(_ sender: NSTextField!) {
        
        pref.aspectY = sender.integerValue
        refresh()
    }
    
    //
    // Action methods (Fullscreen)
    //
    
    @IBAction func genAspectRatioAction(_ sender: NSButton!) {
        
        pref.keepAspectRatio = (sender.state == .on)
        refresh()
    }
    
    @IBAction func genExitOnEscAction(_ sender: NSButton!) {
        
        pref.exitOnEsc = (sender.state == .on)
        refresh()
    }
    
    //
    // Action methods (Miscellaneous)
    //
    
    @IBAction func genEjectWithoutAskingAction(_ sender: NSButton!) {
        
        pref.ejectWithoutAsking = (sender.state == .on)
        refresh()
    }

    @IBAction func genPauseInBackgroundAction(_ sender: NSButton!) {
        
        pref.pauseInBackground = (sender.state == .on)
        refresh()
    }
    
    @IBAction func genCloseWithoutAskingAction(_ sender: NSButton!) {
        
        pref.closeWithoutAsking = (sender.state == .on)
        for c in myAppDelegate.controllers {
            c.needsSaving = c.emu?.running ?? false
        }
        refresh()
    }
        
    //
    // Action methods (Misc)
    //
    
    @IBAction func genPresetAction(_ sender: NSPopUpButton!) {
        
        assert(sender.selectedTag() == 0)

        // Revert to standard settings
        EmulatorProxy.defaults.removeGeneralUserDefaults()

        // Apply the new settings
        pref.applyGeneralUserDefaults()

        refresh()
    }
}

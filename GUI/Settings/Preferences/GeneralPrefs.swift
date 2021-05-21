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
        
        track()
                
        // Fullscreen
        genAspectRatioButton.state = pref.keepAspectRatio ? .on : .off
        genExitOnEscButton.state = pref.exitOnEsc ? .on : .off
        
        // Snapshots and Screenshots
        genAutoSnapshots.state = pref.autoSnapshots ? .on : .off
        genSnapshotInterval.integerValue = pref.snapshotInterval
        genSnapshotInterval.isEnabled = pref.autoSnapshots
        genScreenshotSourcePopup.selectItem(withTag: pref.screenshotSource)
        genScreenshotTargetPopup.selectItem(withTag: pref.screenshotTargetIntValue)
        
        // Screen captures
        let hasFFmpeg = c64.recorder.hasFFmpeg
        genCaptureSource.selectItem(withTag: pref.captureSource)
        genBitRate.stringValue = "\(pref.bitRate)"
        genAspectX.integerValue = pref.aspectX
        genAspectY.integerValue = pref.aspectY
        genCaptureSource.isEnabled = hasFFmpeg
        genBitRate.isEnabled = hasFFmpeg
        genAspectX.isEnabled = hasFFmpeg
        genAspectY.isEnabled = hasFFmpeg
        if hasFFmpeg {
            genFFmpegPath.textColor = .textColor
            genFFmpegPath.stringValue = "/usr/local/bin/ffmpeg"
        } else {
            genFFmpegPath.textColor = .warningColor
            genFFmpegPath.stringValue = "Requires /usr/local/bin/ffmpeg"
        }

        // Drive
        genWarpMode.selectItem(withTag: pref.warpModeIntValue)
        
        // Miscellaneous
        genEjectUnasked.state = pref.driveEjectUnasked ? .on : .off
        genPauseInBackground.state = pref.pauseInBackground ? .on : .off
        genCloseWithoutAskingButton.state = pref.closeWithoutAsking ? .on : .off
    }
    
    func selectGeneralTab() {

        track()
        refreshGeneralTab()
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
    // Action methods (Snapshots and screenshots)
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
    
    @IBAction func genCaptureSourceAction(_ sender: NSPopUpButton!) {
        
        track("tag = \(sender.selectedTag())")
        pref.captureSource = sender.selectedTag()
        refresh()
    }

    @IBAction func genBitRateAction(_ sender: NSComboBox!) {
        
        var input = sender.objectValueOfSelectedItem as? Int
        if input == nil { input = sender.integerValue }
        
        if let bitrate = input {
            track("bitrate = \(bitrate)")
            pref.bitRate = bitrate
        }
        refresh()
    }

    @IBAction func genAspectXAction(_ sender: NSTextField!) {
        
        track("value = \(sender.integerValue)")
        pref.aspectX = sender.integerValue
        refresh()
    }

    @IBAction func genAspectYAction(_ sender: NSTextField!) {
        
        track("value = \(sender.integerValue)")
        pref.aspectY = sender.integerValue
        refresh()
    }
    
    //
    // Action methods (Warp mode)
    //
    
    @IBAction func genWarpModeAction(_ sender: NSPopUpButton!) {
        
        pref.warpMode = WarpMode(rawValue: sender.selectedTag())!
        refresh()
    }
    
    //
    // Action methods (Miscellaneous)
    //
    
    @IBAction func genEjectWithoutAskingAction(_ sender: NSButton!) {
        
        pref.driveEjectUnasked = (sender.state == .on)
        refresh()
    }

    @IBAction func genPauseInBackgroundAction(_ sender: NSButton!) {
        
        pref.pauseInBackground = (sender.state == .on)
        refresh()
    }
    
    @IBAction func genCloseWithoutAskingAction(_ sender: NSButton!) {
        
        pref.closeWithoutAsking = (sender.state == .on)
        for c in myAppDelegate.controllers {
            c.needsSaving = c.c64.running
        }
        refresh()
    }
        
    //
    // Action methods (Misc)
    //
    
    @IBAction func genPresetAction(_ sender: NSPopUpButton!) {
        
        track()
        assert(sender.selectedTag() == 0)
        
        UserDefaults.resetGeneralUserDefaults()
        pref.loadGeneralUserDefaults()
        refresh()
    }
}

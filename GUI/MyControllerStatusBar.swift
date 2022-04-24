// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension MyController {
    
    var hourglass: NSImage? {
        
        switch pref.warpMode {
        case .auto where c64.warpMode == true:
            return NSImage(named: "hourglass3Template")
        case .auto:
            return NSImage(named: "hourglass1Template")
        case .off:
            return NSImage(named: "warpOffTemplate")
        case .on:
            return NSImage(named: "warpOnTemplate")
        }
    }
    
    var cartridgeSwitch: NSImage? {
        
		if !c64.expansionport.hasSwitch { return nil }
        
        let pos = c64.expansionport.switchPosition()
        
        if pos < 0 { return NSImage(named: "crtSwitchLeftTemplate") }
        if pos > 0 { return NSImage(named: "crtSwitchRightTemplate") }
        return NSImage(named: "crtSwitchNeutralTemplate")
    }
    
    public func refreshStatusBar() {
                        
        let connected8 = c64.drive8.isConnected()
        let connected9 = c64.drive9.isConnected()
        let on8 = c64.drive8.isSwitchedOn()
        let on9 = c64.drive9.isSwitchedOn()

        let running = c64.running
        let debug = c64.debugMode
        let jammed = c64.cpu.isJammed()
        let warp = c64.warpMode
        
        let hasCrt = c64.expansionport.cartridgeAttached()
            
        // Floppy drives
        refreshStatusBarDriveItems(drive: .DRIVE8)
        refreshStatusBarDriveItems(drive: .DRIVE9)
            
        // Datasette
        refreshStatusBarDatasette()
        
        // Warp mode
        refreshStatusBarWarpIcon()
        
        // Visibility
        let items: [NSView: Bool] = [
            
            redLED8: connected8,
            redLED9: connected9,
            greenLED8: connected8,
            greenLED9: connected9,
            trackNumber8: connected8 && on8,
            trackNumber9: connected9 && on9,
            
            haltIcon: jammed,
            debugIcon: debug,
            muteIcon: warp || muted,
            
            tapeIcon: c64.datasette.hasTape,
            tapeCounter: c64.datasette.hasTape,
            tapeProgress: c64.datasette.motor,
            
            crtIcon: hasCrt,
            
            warpIcon: running,
            activityType: running,
            activityInfo: running,
            activityBar: running
        ]
        
        for (item, visible) in items {
            item.isHidden = !visible || !statusBar
        }
    }
    
    private func refreshStatusBarDriveItems(drive: DriveID) {
        
        refreshStatusBarLEDs(drive: drive)
        refreshStatusBarTracks(drive: drive)
        refreshStatusBarDiskIcons(drive: drive)
        refreshStatusBarDriveActivity(drive: drive)
    }
    
    func refreshStatusBarLEDs(drive: DriveID) {
        
        assert(drive == .DRIVE8 || drive == .DRIVE9)
        
        if drive == .DRIVE8 {
            
            greenLED8.image = c64.drive8.greenLedImage
            redLED8.image = c64.drive8.redLedImage
            
        } else {
            
            greenLED9.image = c64.drive9.greenLedImage
            redLED9.image = c64.drive9.redLedImage
        }
    }
    
    func refreshStatusBarTracks(drive: DriveID) {
        
        assert(drive == .DRIVE8 || drive == .DRIVE9)

        if drive == .DRIVE8 {
            
            trackNumber8.integerValue = Int((c64.drive8.halftrack() + 1) / 2)
            trackNumber8.textColor = c64.drive8.writeMode() ? .red : .secondaryLabelColor
            
        } else {
            
            trackNumber9.integerValue = Int((c64.drive9.halftrack() + 1) / 2)
            trackNumber9.textColor = c64.drive9.writeMode() ? .red : .secondaryLabelColor
        }
    }
    
    func refreshStatusBarDiskIcons(drive: DriveID) {
    
        assert(drive == .DRIVE8 || drive == .DRIVE9)

        if drive == .DRIVE8 {
            diskIcon8.image = c64.drive8.icon
            diskIcon8.isHidden = !c64.drive8.isConnected() || !c64.drive8.hasDisk() || !statusBar
        } else {
            diskIcon9.image = c64.drive9.icon
            diskIcon9.isHidden = !c64.drive9.isConnected() || !c64.drive9.hasDisk() || !statusBar
        }
    }
    
    func refreshStatusBarDriveActivity() {
        
        refreshStatusBarDriveActivity(drive: .DRIVE8)
        refreshStatusBarDriveActivity(drive: .DRIVE9)
    }

    func refreshStatusBarDriveActivity(drive: DriveID) {
        
        assert(drive == .DRIVE8 || drive == .DRIVE9)

        if drive == .DRIVE8 {
            
            // if c64.iec.transferring && c64.drive8.isRotating() {
            if c64.drive8.isRotating() {
                spinning8.startAnimation(self)
                spinning8.isHidden = !statusBar
            } else {
                spinning8.stopAnimation(self)
                spinning8.isHidden = true
            }
            
        } else {
            
            // if c64.iec.transferring && c64.drive9.isRotating() {
            if c64.drive9.isRotating() {
                spinning9.startAnimation(self)
                spinning9.isHidden = !statusBar
            } else {
                spinning9.stopAnimation(self)
                spinning9.isHidden = true
            }
        }
    }

    func refreshStatusBarDatasette() {
        
        if c64.datasette.motor && c64.datasette.playKey {
            tapeProgress.startAnimation(self)
        } else {
            tapeProgress.stopAnimation(self)
        }
        
        let counter = c64.datasette.counter
        let min = counter / 60
        let sec = counter % 60
        tapeCounter.stringValue = String(format: "%02d:%02d", min, sec)
    }
    
    func refreshStatusBarWarpIcon() {
        
        warpIcon.image = hourglass
    }
    
    func showStatusBar(_ value: Bool) {
        
        if statusBar != value {
            
            if value {
                
                metal.shrink()
                window?.setContentBorderThickness(26, for: .minY)
                adjustWindowSize()
                
            } else {
                
                metal.expand()
                window?.setContentBorderThickness(0, for: .minY)
                adjustWindowSize()
            }
            
            statusBar = value
            refreshStatusBar()
        }
    }
    
    func updateSpeedometer() {
        
        speedometer.updateWith(cycle: c64.cpu.cycles, frame: renderer.frames)
        
        switch activityType.selectedTag() {

        case 0:
            let mhz = speedometer.mhz
            activityBar.doubleValue = 10 * mhz
            activityInfo.stringValue = String(format: "%.2f MHz", mhz)
            
        case 1:
            let cpu = c64.cpuLoad
            activityBar.integerValue = cpu
            activityInfo.stringValue = String(format: "%d%% CPU", cpu)
            
        case 2:
            let fps = speedometer.fps
            activityBar.doubleValue = fps
            activityInfo.stringValue = String(format: "%d FPS", Int(fps))

        default:
            activityBar.integerValue = 0
            activityInfo.stringValue = "???"
        }
    }
    
    //
    // Action methods
    //
    
    @IBAction func drivePowerButtonAction(_ sender: NSButton!) {
        
        track()
        
        switch sender.tag {
        case 8: drivePowerAction(drive: .DRIVE8)
        case 9: drivePowerAction(drive: .DRIVE9)
        default: fatalError()
        }        
    }
    
    @IBAction func warpAction(_ sender: Any!) {
        
        track()
        
        switch pref.warpMode {
        case .auto: pref.warpMode = .off
        case .off: pref.warpMode = .on
        case .on: pref.warpMode = .auto
        }
        
        refreshStatusBar()
    }
    
    @IBAction func activityTypeAction(_ sender: NSPopUpButton!) {

        track()
        
        var min, max, warn, crit: Double
        
        switch sender.selectedTag() {
        
        case 0: min = 0; max = 20; warn = 13; crit = 16
        case 1: min = 0; max = 100; warn = 50; crit = 75
        case 2: min = 0; max = 120; warn = 75; crit = 100

        default:
            fatalError()
        }
        
        activityBar.minValue = min
        activityBar.maxValue = max
        activityBar.warningValue = warn
        activityBar.criticalValue = crit

        refreshStatusBar()
    }
}

// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension MyController {
    
    var hourglass: NSImage? {
        
        switch pref.warpMode {
        case .auto where c64.warp == true:
            return NSImage.init(named: "hourglass3Template")
        case .auto:
            return NSImage.init(named: "hourglass1Template")
        case .off:
            return NSImage.init(named: "warpOffTemplate")
        case .on:
            return NSImage.init(named: "warpOnTemplate")
        }
    }
    
    var cartridgeSwitch: NSImage? {
        
        if !c64.expansionport.hasSwitch() { return nil }
        
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
        let warp = c64.warp
        
        let hasCrt = c64.expansionport.cartridgeAttached()
            
        // Floppy drives
        refreshStatusBarDriveItems(drive: .DRIVE8)
        refreshStatusBarDriveItems(drive: .DRIVE9)
                
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
            tapeProgress: c64.datasette.motor,
            
            crtIcon: hasCrt,
            
            warpIcon: running,
            clockSpeed: running,
            clockSpeedBar: running
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
            
            if c64.iec.busy && c64.drive8.isRotating() {
                spinning8.startAnimation(self)
                spinning8.isHidden = !statusBar
            } else {
                spinning8.stopAnimation(self)
                spinning8.isHidden = true
            }
            
        } else {
            
            if c64.iec.busy && c64.drive9.isRotating() {
                spinning9.startAnimation(self)
                spinning9.isHidden = !statusBar
            } else {
                spinning9.stopAnimation(self)
                spinning9.isHidden = true
            }
        }
    }

    func refreshStatusBarWarpIcon() {
        
        warpIcon.image = hourglass
    }
    
    func showStatusBar(_ value: Bool) {
        
        if statusBar != value {
            
            if value {
                
                metal.shrink()
                // window?.setContentBorderThickness(26, for: .minY)
                adjustWindowSize()
                
            } else {
                
                metal.expand()
                // window?.setContentBorderThickness(0, for: .minY)
                adjustWindowSize()
            }
            
            statusBar = value
            refreshStatusBar()
        }
    }
    
    @IBAction func drivePowerButtonAction(_ sender: NSButton!) {
        
        track()
        
        switch sender.tag {
        case 8: drivePowerAction(drive: .DRIVE8)
        case 9: drivePowerAction(drive: .DRIVE9)
        default: fatalError()
        }
        
        // refreshStatusBar()
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
}

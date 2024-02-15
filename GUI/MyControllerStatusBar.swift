// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension MyController {

    var hourglassIcon: NSImage? {

        if WarpMode(rawValue: config.warpMode) == .AUTO {

            return NSImage(named: c64.info.warping ? "hourglass3Template" : "hourglass1Template")

        } else {

            return NSImage(named: c64.info.warping ? "warpOnTemplate" : "warpOffTemplate")
        }
    }

    /*
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
    */

    var cartridgeSwitch: NSImage? {
        
        if !c64.expansionport.hasSwitch { return nil }

        let pos = c64.expansionport.switchPosition()
        
        if pos < 0 { return NSImage(named: "crtSwitchLeftTemplate") }
        if pos > 0 { return NSImage(named: "crtSwitchRightTemplate") }
        return NSImage(named: "crtSwitchNeutralTemplate")
    }
    
    public func refreshStatusBar() {

        let dsstate = c64.datasette.info

        let c64state = c64.info
        let running = c64state.running
        let tracking = c64state.tracking
        let warping = c64state.warping

        let connected8 = c64.drive8.isConnected()
        let connected9 = c64.drive9.isConnected()
        let on8 = c64.drive8.isSwitchedOn()
        let on9 = c64.drive9.isSwitchedOn()

        let hasCrt = c64.expansionport.cartridgeAttached()

        // Floppy drives
        refreshStatusBarDriveItems(drive: DRIVE8)
        refreshStatusBarDriveItems(drive: DRIVE9)

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
            trackIcon: tracking,
            muteIcon: warping || muted,
            
            tapeIcon: dsstate.hasTape,
            tapeCounter: dsstate.hasTape,
            tapeProgress: dsstate.motor,

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
    
    private func refreshStatusBarDriveItems(drive: Int) {
        
        refreshStatusBarLEDs(drive: drive)
        refreshStatusBarTracks(drive: drive)
        refreshStatusBarDiskIcons(drive: drive)
        refreshStatusBarDriveActivity(drive: drive)
    }
    
    func refreshStatusBarLEDs(drive: Int) {
        
        switch drive {

        case DRIVE8:

            greenLED8.image = c64.drive8.greenLedImage
            redLED8.image = c64.drive8.redLedImage

        case DRIVE9:

            greenLED9.image = c64.drive9.greenLedImage
            redLED9.image = c64.drive9.redLedImage

        default:
            fatalError()
        }
    }
    
    func refreshStatusBarTracks(drive: Int) {
        
        switch drive {

        case DRIVE8:
            
            trackNumber8.integerValue = Int((c64.drive8.halftrack() + 1) / 2)
            trackNumber8.textColor = c64.drive8.writeMode() ? .red : .secondaryLabelColor

        case DRIVE9:

            trackNumber9.integerValue = Int((c64.drive9.halftrack() + 1) / 2)
            trackNumber9.textColor = c64.drive9.writeMode() ? .red : .secondaryLabelColor

        default:
            fatalError()
        }
    }

    func refreshStatusBarDiskIcons(drive: Int) {

        switch drive {

        case DRIVE8:

            diskIcon8.image = c64.drive8.icon
            diskIcon8.isHidden = !c64.drive8.isConnected() || !c64.drive8.hasDisk || !statusBar

        case DRIVE9:

            diskIcon9.image = c64.drive9.icon
            diskIcon9.isHidden = !c64.drive9.isConnected() || !c64.drive9.hasDisk || !statusBar

        default:
            fatalError()
        }
    }

    func refreshStatusBarDriveActivity() {

        refreshStatusBarDriveActivity(drive: DRIVE8)
        refreshStatusBarDriveActivity(drive: DRIVE9)
    }

    func refreshStatusBarDriveActivity(drive: Int) {

        switch drive {

        case DRIVE8:

            if c64.drive8.isRotating() {
                spinning8.startAnimation(self)
                spinning8.isHidden = !statusBar
            } else {
                spinning8.stopAnimation(self)
                spinning8.isHidden = true
            }

        case DRIVE9:

            if c64.drive9.isRotating() {
                spinning9.startAnimation(self)
                spinning9.isHidden = !statusBar
            } else {
                spinning9.stopAnimation(self)
                spinning9.isHidden = true
            }

        default:
            fatalError()
        }
    }

    func refreshStatusBarDatasette() {

        let dsstate = c64.datasette.info

        if dsstate.motor && dsstate.playKey {
            tapeProgress.startAnimation(self)
        } else {
            tapeProgress.stopAnimation(self)
        }

        let counter = dsstate.counter
        let min = counter / 60
        let sec = counter % 60
        tapeCounter.stringValue = String(format: "%02d:%02d", min, sec)
    }

    func refreshStatusBarWarpIcon() {

        warpIcon.image = hourglassIcon
    }

    func showStatusBar(_ value: Bool) {

        if statusBar != value {

            if value {

                metal.adjustHeight(-26.0)
                window?.setContentBorderThickness(26, for: .minY)
                adjustWindowSize(dy: 26.0)

            } else {

                metal.adjustHeight(26.0)
                window?.setContentBorderThickness(0.0, for: .minY)
                adjustWindowSize(dy: -26.0)
            }

            statusBar = value
            refreshStatusBar()
        }
    }

    func updateSpeedometer() {

        func setColor(color: [NSColor]) {

            let min = activityBar.minValue
            let max = activityBar.maxValue
            let cur = (activityBar.doubleValue - min) / (max - min)

            let index =
            cur < 0.15 ? 0 :
            cur < 0.40 ? 1 :
            cur < 0.60 ? 2 :
            cur < 0.85 ? 3 : 4

            activityBar.fillColor = color[index]
        }

        speedometer.updateWith(cycle: c64.cpu.clock, gpuFrame: renderer.frames)

        switch activityType.selectedTag() {

        case 0:
            let mhz = speedometer.mhz
            activityBar.maxValue = 20
            activityBar.doubleValue = 10 * mhz
            activityInfo.stringValue = String(format: "%.2f MHz", mhz)
            setColor(color: [.systemRed, .systemYellow, .systemGreen, .systemYellow, .systemRed])

        case 1:
            let fps = c64.stats.fps
            activityBar.maxValue = 120
            activityBar.doubleValue = fps
            activityInfo.stringValue = String(format: "%d Hz", Int(fps))
            setColor(color: [.systemRed, .systemYellow, .systemGreen, .systemYellow, .systemRed])

        case 2:
            let cpu = Int(c64.stats.cpuLoad * 100)
            activityBar.maxValue = 100
            activityBar.integerValue = cpu
            activityInfo.stringValue = String(format: "%d%% CPU", cpu)
            setColor(color: [.systemGreen, .systemGreen, .systemGreen, .systemYellow, .systemRed])

        case 3:
            let fps = speedometer.gpsFps
            activityBar.maxValue = 120
            activityBar.doubleValue = fps
            activityInfo.stringValue = String(format: "%d FPS", Int(fps))
            setColor(color: [.systemRed, .systemYellow, .systemGreen, .systemYellow, .systemRed])

        case 4:
            let fill = c64.sid.stats.fillLevel * 100.0
            activityBar.maxValue = 100
            activityBar.doubleValue = fill
            activityInfo.stringValue = String(format: "Fill level %d%%", Int(fill))
            setColor(color: [.systemRed, .systemYellow, .systemGreen, .systemYellow, .systemRed])

        default:
            activityBar.integerValue = 0
            activityInfo.stringValue = "???"
        }
    }

    //
    // Action methods
    //

    @IBAction func drivePowerButtonAction(_ sender: NSButton!) {

        drivePowerAction(drive: sender.tag == 0 ? DRIVE8 : DRIVE9)
    }

    @IBAction func warpAction(_ sender: Any!) {

        switch WarpMode(rawValue: config.warpMode) {

        case .AUTO: config.warpMode = WarpMode.NEVER.rawValue
        case .NEVER: config.warpMode = WarpMode.ALWAYS.rawValue
        case .ALWAYS: config.warpMode = WarpMode.AUTO.rawValue

        default:
            fatalError()
        }

        refreshStatusBar()
        myAppDelegate.prefController?.refresh()
    }

    /*
    @IBAction func timeLapseAction(_ sender: NSSlider!) {

        let value = sender.integerValue
        let incr = sender.altIncrementValue
        print("value = \(value) incr = \(incr)")
        config.tim
        refreshStatusBar()
    }
*/
    @IBAction func activityTypeAction(_ sender: NSPopUpButton!) {

        /*
        var min, max: Double

        switch sender.selectedTag() {

        case 0: min = 0; max = 20;
        case 1: min = 0; max = 120;
        case 2: min = 0; max = 100;
        case 3: min = 0; max = 120;
        case 4: min = 0; max = 100;

        default:
            fatalError()
        }

        activityBar.minValue = min
        activityBar.maxValue = max
        // activityBar.warningValue = warn
        // activityBar.criticalValue = crit
        */

        refreshStatusBar()
    }
}

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

        if vc64.Warp(rawValue: config.warpMode) == .AUTO {
            return NSImage(named: emu?.info.warping == true ? "hourglass3Template" : "hourglass1Template")
        } else {
            return NSImage(named: emu?.info.warping == true ? "warpOnTemplate" : "warpOffTemplate")
        }
    }

    public func refreshStatusBar() {

        guard let emu = emu else { return }

        let dsstate = emu.datasette.info

        let info = emu.info
        let running = info.running
        let tracking = info.tracking
        let warping = info.warping
        let boost = emu.get(.C64_SPEED_BOOST)

        let config8 = emu.drive8.config
        let config9 = emu.drive9.config
        let connected8 = config8.connected
        let connected9 = config9.connected
        let on8 = config8.switchedOn
        let on9 = config9.switchedOn

        let hasCrt = emu.expansionport.cartridgeAttached()

        // Floppy drives
        refreshStatusBarDriveItems(drive: DRIVE8)
        refreshStatusBarDriveItems(drive: DRIVE9)

        // Datasette
        refreshStatusBarDatasette()

        // Remote server icon
        refreshStatusBarServerIcon()

        // Warp mode
        refreshStatusBarWarpIcon()

        // Speed adjust
        speedStepper.integerValue = boost
        speedStepper.toolTip = "\(boost) %"

        // Visibility
        let items: [NSView: Bool] = [

            redLED8: connected8,
            redLED9: connected9,
            greenLED8: connected8,
            greenLED9: connected9,
            trackNumber8: connected8 && on8,
            trackNumber9: connected9 && on9,

            haltIcon: jammed,
            serverIcon: true,
            trackIcon: tracking,
            muteIcon: warping || muted,

            tapeIcon: dsstate.hasTape,
            tapeCounter: dsstate.hasTape,
            tapeProgress: dsstate.motor,

            crtIcon: hasCrt,

            warpIcon: running,
            activityType: running,
            activityInfo: running,
            activityBar: running,
            speedStepper: running
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
        
        if let emu = emu {

            switch drive {

            case DRIVE8:

                greenLED8.image = emu.drive8.greenLedImage
                redLED8.image = emu.drive8.redLedImage

            case DRIVE9:

                greenLED9.image = emu.drive9.greenLedImage
                redLED9.image = emu.drive9.redLedImage

            default:
                fatalError()
            }
        }
    }
    
    func refreshStatusBarTracks(drive: Int) {
        
        if let emu = emu {

            switch drive {

            case DRIVE8:

                let info = emu.drive8.info
                trackNumber8.integerValue = Int((info.halftrack + 1) / 2)
                trackNumber8.textColor = info.writing ? .red : .secondaryLabelColor

            case DRIVE9:

                let info = emu.drive9.info
                trackNumber9.integerValue = Int((info.halftrack + 1) / 2)
                trackNumber9.textColor = info.writing ? .red : .secondaryLabelColor

            default:
                fatalError()
            }
        }
    }

    func refreshStatusBarDiskIcons(drive: Int) {

        if let emu = emu {

            switch drive {

            case DRIVE8:

                let info = emu.drive8.info
                let config = emu.drive8.config
                diskIcon8.image = emu.drive8.icon
                diskIcon8.isHidden = !config.connected || !info.hasDisk || !statusBar

            case DRIVE9:

                let info = emu.drive9.info
                let config = emu.drive9.config
                diskIcon9.image = emu.drive9.icon
                diskIcon9.isHidden = !config.connected || !info.hasDisk || !statusBar

            default:
                fatalError()
            }
        }
    }

    func refreshStatusBarDriveActivity() {

        refreshStatusBarDriveActivity(drive: DRIVE8)
        refreshStatusBarDriveActivity(drive: DRIVE9)
    }

    func refreshStatusBarDriveActivity(drive: Int) {

        if let emu = emu {

            switch drive {

            case DRIVE8:

                if emu.drive8.info.spinning {
                    spinning8.startAnimation(self)
                    spinning8.isHidden = !statusBar
                } else {
                    spinning8.stopAnimation(self)
                    spinning8.isHidden = true
                }

            case DRIVE9:

                if emu.drive9.info.spinning {
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
    }

    func refreshStatusBarDatasette() {

        if let emu = emu {

            let dsstate = emu.datasette.info

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
    }

    func refreshStatusBarServerIcon() {

        if let emu = emu {

            serverIcon.image = emu.remoteManager.icon
        }
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

        if let emu = emu {

            let state = emu.cpu.info

            speedometer.updateWith(cycle: state.cycle,
                                   emuFrame: Int64(emu.c64.info.frame),
                                   gpuFrame: renderer.frames)

            switch activityType.selectedTag() {

            case 0:
                let mhz = speedometer.mhz
                activityBar.maxValue = 20
                activityBar.doubleValue = 10 * mhz
                activityInfo.stringValue = String(format: "%.2f MHz", mhz)
                setColor(color: [.systemRed, .systemYellow, .systemGreen, .systemYellow, .systemRed])

            case 1:
                let fps = emu.stats.fps
                activityBar.maxValue = 120
                activityBar.doubleValue = fps
                activityInfo.stringValue = String(format: "%d Hz", Int(fps))
                setColor(color: [.systemRed, .systemYellow, .systemGreen, .systemYellow, .systemRed])

            case 2:
                let cpu = Int(emu.stats.cpuLoad * 100)
                activityBar.maxValue = 100
                activityBar.integerValue = cpu
                activityInfo.stringValue = String(format: "%d%% CPU", cpu)
                setColor(color: [.systemGreen, .systemGreen, .systemGreen, .systemYellow, .systemRed])

            case 3:
                let fps = speedometer.gpuFps
                activityBar.maxValue = 120
                activityBar.doubleValue = fps
                activityInfo.stringValue = String(format: "%d FPS", Int(fps))
                setColor(color: [.systemRed, .systemYellow, .systemGreen, .systemYellow, .systemRed])

            case 4:
                let fill = emu.audioPort.stats.fillLevel * 100.0
                activityBar.maxValue = 100
                activityBar.doubleValue = fill
                activityInfo.stringValue = String(format: "Fill level %d%%", Int(fill))
                setColor(color: [.systemRed, .systemYellow, .systemGreen, .systemYellow, .systemRed])

            default:
                activityBar.integerValue = 0
                activityInfo.stringValue = "???"
            }
        }
    }

    //
    // Action methods
    //

    @IBAction
    func drivePowerButtonAction(_ sender: NSButton!) {

        drivePowerAction(drive: sender.tag == 0 ? DRIVE8 : DRIVE9)
    }

    @IBAction
    func warpAction(_ sender: Any!) {

        switch vc64.Warp(rawValue: config.warpMode) {

        case .AUTO: config.warpMode = vc64.Warp.NEVER.rawValue
        case .NEVER: config.warpMode = vc64.Warp.ALWAYS.rawValue
        case .ALWAYS: config.warpMode = vc64.Warp.AUTO.rawValue

        default:
            fatalError()
        }

        refreshStatusBar()
        myAppDelegate.prefController?.refresh()
    }

    @IBAction func activityTypeAction(_ sender: NSPopUpButton!) {

        var min, max, warn, crit: Double

        switch sender.selectedTag() {

        case 0: min = 0; max = 140; warn = 77; crit = 105
        case 1: min = 0; max = 120; warn = 75; crit = 100
        case 2: min = 0; max = 100; warn = 50; crit = 75
        case 3: min = 0; max = 120; warn = 75; crit = 100
        case 4: min = 0; max = 100; warn = 85; crit = 95

        default:
            fatalError()
        }

        activityBar.minValue = min
        activityBar.maxValue = max
        activityBar.warningValue = warn
        activityBar.criticalValue = crit

        refreshStatusBar()
    }

    @IBAction
    func speedAction(_ sender: NSStepper!) {

        // Round the value to the next number dividable by 5
        var value = Int(round(sender.doubleValue / 5.0)) * 5

        // Make sure the value is in the valid range
        if value < 50 { value = 50 }
        if value > 200 { value = 200 }

        emu?.set(.C64_SPEED_BOOST, value: value)
    }

    @IBAction func speedResetAction(_ sender: Any!) {

        emu?.set(.C64_SPEED_BOOST, value: 100)
    }

    @IBAction
    func infoAction(_ sender: Any!) {

        print("infoAction")
        
        if let emu = emu, let info = infoText {

            // Get some auxiliary debug information from the emulator
            let attributes: [NSAttributedString.Key: Any] = [
                .font: NSFont.monospaced(ofSize: 11, weight: .semibold),
                .foregroundColor: NSColor.labelColor
            ]
            let text = NSAttributedString(string: emu.c64.stateString!, attributes: attributes)
            let size = CGRect(x: 0, y: 0, width: text.size().width + 16, height: text.size().height)

            // Put the information into an accessory view
            let accessory = NSTextView(frame: size)
            accessory.textStorage?.setAttributedString(text)
            accessory.drawsBackground = false
            accessory.isEditable = false

            // Create an alert
            let alert = NSAlert()
            alert.messageText = info
            alert.informativeText = infoText2 ?? ""
            alert.alertStyle = .informational
            alert.icon = NSImage(systemSymbolName: "waveform.badge.magnifyingglass",
                                 accessibilityDescription: nil)
            alert.addButton(withTitle: "OK")
            alert.accessoryView = accessory

            alert.runModal()
        }
    }
}

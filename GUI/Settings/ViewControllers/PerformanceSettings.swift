// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class PerformanceSettingsViewController: SettingsViewController {

    // Warp
    @IBOutlet weak var warpMode: NSPopUpButton!
    @IBOutlet weak var warpBoot: NSTextField!

    // Threading
    @IBOutlet weak var vSync: NSButton!
    @IBOutlet weak var speedBoost: NSSlider!
    @IBOutlet weak var speedBoostInfo: NSTextField!
    @IBOutlet weak var runAheadLabel: NSTextField!
    @IBOutlet weak var runAhead: NSSlider!
    @IBOutlet weak var runAheadInfo: NSTextField!

    // Boosters
    @IBOutlet weak var drivePowerSave: NSButton!
    @IBOutlet weak var viciiPowerSave: NSButton!
    @IBOutlet weak var sidPowerSave: NSButton!

    // Compression
    @IBOutlet weak var snapCompressor: NSPopUpButton!

    // Collision detection
    @IBOutlet weak var ssCollisions: NSButton!
    @IBOutlet weak var sbCollisions: NSButton!

    override var showLock: Bool { true }

    override func viewDidLoad() {

        log(.lifetime)
    }

    override func refresh() {

        super.refresh()
        
        guard let config = config else { return }

        // Warp
        warpMode.selectItem(withTag: config.warpMode)
        warpBoot.integerValue = config.warpBoot

        // Threading
        let boost = config.speedBoost
        let ahead = config.runAhead
        vSync.state = config.vsync ? .on : .off
        speedBoost.integerValue = boost
        speedBoostInfo.stringValue = "\(boost) %"
        speedBoost.isEnabled = !config.vsync
        speedBoostInfo.textColor = config.vsync ? .tertiaryLabelColor : .labelColor
        runAheadLabel.stringValue = ahead >= 0 ? "Run ahead:" : "Run behind:"
        runAhead.integerValue = ahead
        runAheadInfo.stringValue = "\(abs(ahead)) frame" + (abs(ahead) == 1 ? "" : "s")

        // Boosters
        drivePowerSave.state = config.drive8PowerSave ? .on : .off
        viciiPowerSave.state = config.viciiPowerSave ? .on : .off
        sidPowerSave.state = config.sidPowerSave ? .on : .off

        // Collision detection
        ssCollisions.state = config.ssCollisions ? .on : .off
        sbCollisions.state = config.sbCollisions ? .on : .off

        // Compression
        snapCompressor.selectItem(withTag: config.snapCompressor)
    }

    //
    // Action methods (Boosters)
    //

    @IBAction func drivePowerSaveAction(_ sender: NSButton!) {

        config?.drive8PowerSave = sender.state == .on
        config?.drive9PowerSave = sender.state == .on
    }

    @IBAction func viciiPowerSaveAction(_ sender: NSButton!) {

        config?.viciiPowerSave = sender.state == .on
    }

    @IBAction func sidPowerSaveAction(_ sender: NSButton!) {

        config?.sidPowerSave = sender.state == .on
    }

    //
    // Action methods (Collisions)
    //

    @IBAction func ssCollisionsAction(_ sender: NSButton!) {

        config?.ssCollisions = sender.state == .on
    }

    @IBAction func sbCollisionsAction(_ sender: NSButton!) {

        config?.sbCollisions = sender.state == .on
    }

    //
    // Action methods (Warp)
    //

    @IBAction func warpModeAction(_ sender: NSPopUpButton!) {

        config?.warpMode = sender.selectedTag()
    }

    @IBAction func warpBootAction(_ sender: NSTextField!) {

        config?.warpBoot = sender.integerValue
    }

    //
    // Action methods (Threading)
    //

    @IBAction func vSyncAction(_ sender: NSButton!) {

        config?.vsync = sender.state == .on
    }

    @IBAction func speedBoostAction(_ sender: NSSlider!) {

        config?.speedBoost = sender.integerValue
    }

    @IBAction func runAheadAction(_ sender: NSSlider!) {

        config?.runAhead = sender.integerValue
    }

    //
    // Action methods (Compressor)
    //

    @IBAction func snapCompressorAction(_ sender: NSPopUpButton!) {

        config?.snapCompressor = sender.selectedTag()
    }

    //
    // Presets and Saving
    //

    override func preset(tag: Int) {

        guard let emu = emu else { return }

        emu.suspend()

        // Revert to standard settings
        EmulatorProxy.defaults.removePerformanceUserDefaults()

        // Update the configuration
        config?.applyPerformanceUserDefaults()

        // Override some options
        switch tag {

        case 1: // Accurate

            config?.drive8PowerSave = false
            config?.drive9PowerSave = false
            config?.viciiPowerSave = false
            config?.sidPowerSave = false
            config?.ssCollisions = true
            config?.sbCollisions = true

        case 2: // Accelerated

            config?.drive8PowerSave = true
            config?.drive9PowerSave = true
            config?.viciiPowerSave = true
            config?.sidPowerSave = true
            config?.ssCollisions = false
            config?.sbCollisions = false

        default:
            break
        }

        emu.resume()
    }

    override func save() {

        config?.savePerformanceUserDefaults()
    }
}

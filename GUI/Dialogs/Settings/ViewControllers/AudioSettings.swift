// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class AudioSettingsViewController: SettingsViewController {

    // Engine
    @IBOutlet weak var engine: NSPopUpButton!
    @IBOutlet weak var filter: NSButton!
    @IBOutlet weak var sampling: NSPopUpButton!

    // In
    @IBOutlet weak var vol0: NSSlider!
    @IBOutlet weak var vol1: NSSlider!
    @IBOutlet weak var vol2: NSSlider!
    @IBOutlet weak var vol3: NSSlider!
    @IBOutlet weak var pan0: NSSlider!
    @IBOutlet weak var pan1: NSSlider!
    @IBOutlet weak var pan2: NSSlider!
    @IBOutlet weak var pan3: NSSlider!

    // Out
    @IBOutlet weak var volL: NSSlider!
    @IBOutlet weak var volR: NSSlider!

    // Drives
    @IBOutlet weak var stepVolume: NSSlider!
    @IBOutlet weak var ejectVolume: NSSlider!
    @IBOutlet weak var insertVolume: NSSlider!
    @IBOutlet weak var drive8Pan: NSSlider!
    @IBOutlet weak var drive9Pan: NSSlider!

    override var showLock: Bool { true }

    override func viewDidLoad() {

        log(.lifetime)
    }

    override func refresh() {

        guard let config = config else { return }

        // Engine
        engine.selectItem(withTag: config.sidEngine)
        filter.state = config.sidFilter ? .on : .off
        sampling.isEnabled = config.sidEngine == vc64.SIDEngine.RESID.rawValue
        sampling.selectItem(withTag: config.sidSampling)

        // In
        vol0.integerValue = config.vol0
        vol1.integerValue = config.vol1
        vol2.integerValue = config.vol2
        vol3.integerValue = config.vol3
        pan0.integerValue = config.pan0
        pan1.integerValue = config.pan1
        pan2.integerValue = config.pan2
        pan3.integerValue = config.pan3

        // Out
        volL.integerValue = config.volL
        volR.integerValue = config.volR

        // Drives
        stepVolume.integerValue = config.stepVolume
        insertVolume.integerValue = config.insertVolume
        ejectVolume.integerValue = config.ejectVolume
        drive8Pan.integerValue = config.drive8Pan
        drive9Pan.integerValue = config.drive9Pan
    }

    @IBAction func vol0Action(_ sender: NSSlider!) {

        config?.vol0 = sender.integerValue
    }

    @IBAction func vol1Action(_ sender: NSSlider!) {

        config?.vol1 = sender.integerValue
    }

    @IBAction func vol2Action(_ sender: NSSlider!) {

        config?.vol2 = sender.integerValue
    }

    @IBAction func vol3Action(_ sender: NSSlider!) {

        config?.vol3 = sender.integerValue
    }

    @IBAction func pan0Action(_ sender: NSSlider!) {

        config?.pan0 = sender.integerValue
    }

    @IBAction func pan1Action(_ sender: NSSlider!) {

        config?.pan1 = sender.integerValue
    }

    @IBAction func pan2Action(_ sender: NSSlider!) {

        config?.pan2 = sender.integerValue
    }

    @IBAction func pan3Action(_ sender: NSSlider!) {

        config?.pan3 = sender.integerValue
    }

    @IBAction func volLAction(_ sender: NSSlider!) {

        config?.volL = sender.integerValue
    }

    @IBAction func volRAction(_ sender: NSSlider!) {

        config?.volR = sender.integerValue
    }

    @IBAction func stepVolumeAction(_ sender: NSSlider!) {

        config?.stepVolume = sender.integerValue
    }

    @IBAction func insertVolumeAction(_ sender: NSSlider!) {

        config?.insertVolume = sender.integerValue
    }

    @IBAction func ejectVolumeAction(_ sender: NSSlider!) {

        config?.ejectVolume = sender.integerValue
    }

    @IBAction func drive8PanAction(_ sender: NSSlider!) {

        config?.drive8Pan = sender.integerValue
    }

    @IBAction func drive9PanAction(_ sender: NSSlider!) {

        config?.drive9Pan = sender.integerValue
    }

    @IBAction func engineAction(_ sender: NSPopUpButton!) {

        config?.sidEngine = sender.selectedTag()
    }

    @IBAction func samplingAction(_ sender: NSPopUpButton!) {

        config?.sidSampling = sender.selectedTag()
    }

    @IBAction func filterAction(_ sender: NSButton!) {

        config?.sidFilter = sender.state == .on
    }

    //
    // Presets and Saving
    //

    override func preset(tag: Int) {

        guard let emu = emu, let config = config else { return }

        emu.suspend()

        // Revert to standard settings
        EmulatorProxy.defaults.removeAudioUserDefaults()

        // Update the configuration
        config.applyAudioUserDefaults()

        // Override some options
        switch tag {

        case 0: // Mono
            config.pan0 = 0
            config.pan1 = 0
            config.pan2 = 0
            config.pan3 = 0
            config.drive8Pan = 0
            config.drive9Pan = 0

        case 1: // Stereo
            config.pan0 = 100
            config.pan1 = 300
            config.pan2 = 300
            config.pan3 = 100
            config.drive8Pan = 100
            config.drive9Pan = 300

        default:
            fatalError()
        }

        emu.resume()
    }

    override func save() {

        config?.saveAudioUserDefaults()
    }
}

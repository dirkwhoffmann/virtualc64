// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class GeneralSettingsViewController: SettingsViewController {

    // Fullscreen
    @IBOutlet weak var aspectRatioButton: NSButton!
    @IBOutlet weak var exitOnEscButton: NSButton!

    // Mouse
    @IBOutlet weak var retainMouseKeyComb: NSPopUpButton!
    @IBOutlet weak var retainMouseByClick: NSButton!
    @IBOutlet weak var retainMouseByEntering: NSButton!
    @IBOutlet weak var releaseMouseKeyComb: NSPopUpButton!
    @IBOutlet weak var releaseMouseByShaking: NSButton!

    // Misc
    @IBOutlet weak var ejectWithoutAskingButton: NSButton!
    @IBOutlet weak var closeWithoutAskingButton: NSButton!
    @IBOutlet weak var pauseInBackground: NSButton!

    override func viewDidLoad() {

        log(.lifetime)
    }

    //
    // Methods from SettingsViewController
    //

    override func refresh() {

        super.refresh()

        // Fullscreen
        aspectRatioButton.state = pref.keepAspectRatio ? .on : .off
        exitOnEscButton.state = pref.exitOnEsc ? .on : .off

        // Miscellaneous
        ejectWithoutAskingButton.state = pref.ejectWithoutAsking ? .on : .off
        closeWithoutAskingButton.state = pref.closeWithoutAsking ? .on : .off
        pauseInBackground.state = pref.pauseInBackground ? .on : .off
    }

    //
    // Action methods (Fullscreen)
    //

    @IBAction func aspectRatioAction(_ sender: NSButton!) {

        pref.keepAspectRatio = (sender.state == .on)
        refresh()
    }

    @IBAction func exitOnEscAction(_ sender: NSButton!) {

        pref.exitOnEsc = (sender.state == .on)
        refresh()
    }

    //
    // Action methods (Mouse)
    //

    @IBAction func retainMouseKeyCombAction(_ sender: NSPopUpButton!) {

        // pref.retainMouseKeyComb = sender.selectedTag()
        refresh()
    }

    @IBAction func retainMouseAction(_ sender: NSButton!) {

        switch sender.tag {

        case 1: pref.retainMouseByClick    = (sender.state == .on)
        case 2: pref.retainMouseByEntering = (sender.state == .on)
        default: fatalError()
        }

        refresh()
    }

    @IBAction func releaseMouseKeyCombAction(_ sender: NSPopUpButton!) {

        // pref.releaseMouseKeyComb = sender.selectedTag()
        refresh()
    }

    @IBAction func releaseMouseAction(_ sender: NSButton!) {

        switch sender.tag {

        case 1: pref.releaseMouseByShaking = (sender.state == .on)
        default: fatalError()
        }

        refresh()
    }

    //
    // Action methods (Misc)
    //

    @IBAction func ejectWithoutAskingAction(_ sender: NSButton!) {

        pref.ejectWithoutAsking = (sender.state == .on)
        refresh()
    }

    @IBAction func closeWithoutAskingAction(_ sender: NSButton!) {

        pref.closeWithoutAsking = (sender.state == .on)
        refresh()
    }

    @IBAction func pauseInBackgroundAction(_ sender: NSButton!) {

        pref.pauseInBackground = (sender.state == .on)
        refresh()
    }

    @IBAction func screenshotSourceAction(_ sender: NSPopUpButton!) {

        pref.screenshotSourceIntValue = sender.selectedTag()
        refresh()
    }

    @IBAction func screenshotFormatAction(_ sender: NSPopUpButton!) {

        pref.screenshotFormatIntValue = sender.selectedTag()
        refresh()
    }

    @IBAction func screenshotCutoutAction(_ sender: NSPopUpButton!) {

        pref.screenshotCutoutIntValue = sender.selectedTag()
        refresh()
    }

    @IBAction func screenshotWidthAction(_ sender: NSTextField!) {

        pref.screenshotWidth = sender.integerValue
        refresh()
    }

    @IBAction func screenshotHeightAction(_ sender: NSTextField!) {

        pref.screenshotHeight = sender.integerValue
        refresh()
    }

    //
    // Presets and Saving
    //
    
    override func preset(tag: Int) {

        print("preset(tag: \(tag))")

        // Revert to standard settings
        EmulatorProxy.defaults.removeGeneralUserDefaults()

        // Apply the new settings
        pref.applyGeneralUserDefaults()
    }

    override func save() {

        pref.saveGeneralUserDefaults()
    }
}

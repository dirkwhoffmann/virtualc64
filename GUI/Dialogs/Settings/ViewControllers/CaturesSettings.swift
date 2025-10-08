// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class CapturesSettingsViewController: SettingsViewController {

    // Snapshots
    @IBOutlet weak var snapshotsAutoDelete: NSButton!
    @IBOutlet weak var snapshotHelp: NSTextField!
    /*
    @IBOutlet weak var snapshotCapacity: NSSlider!
    @IBOutlet weak var snapshotAutoDelete: NSButton!
    @IBOutlet weak var autoSnapshots: NSButton!
    @IBOutlet weak var snapshotInterval: NSTextField!
    */

    // Screenshots
    @IBOutlet weak var screenshotFormatPopup: NSPopUpButton!
    @IBOutlet weak var screenshotSourcePopup: NSPopUpButton!
    @IBOutlet weak var screenshotCutoutPopup: NSPopUpButton!
    @IBOutlet weak var screenshotCutoutText: NSTextField!
    @IBOutlet weak var screenshotWidth: NSTextField!
    @IBOutlet weak var screenshotWidthText: NSTextField!
    @IBOutlet weak var screenshotHeight: NSTextField!
    @IBOutlet weak var screenshotHeightText: NSTextField!

    override func viewDidLoad() {

        log(.lifetime)
    }

    //
    // Methods from SettingsViewController
    //

    override func refresh() {

        super.refresh()

        // Snapshots
        if pref.snapshotAutoDelete {

            snapshotsAutoDelete.state = .on
            snapshotHelp.stringValue =
            "vAmiga stores up to \(MyDocument.maxSnapshots) snapshots. " +
            "The oldest is deleted automatically once the limit is reached."

        } else {

            snapshotsAutoDelete.state = .off
            snapshotHelp.stringValue =
            "vAmiga stores up to \(MyDocument.maxSnapshots) snapshots. " +
            "Manual deletion is required to add another."
        }
            /*
        snapshotCapacity.integerValue = pref.snapshotStorage
        snapshotAutoDelete.state = pref.snapshotAutoDelete ? .on : .off
        autoSnapshots.state = pref.autoSnapshots ? .on : .off
        snapshotInterval.integerValue = pref.snapshotInterval
        snapshotInterval.isEnabled = pref.autoSnapshots
        */

        // Screenshots
        let framebuffer = pref.screenshotSource == .framebuffer
        let custom = pref.screenshotCutout == .custom
        screenshotFormatPopup.selectItem(withTag: pref.screenshotFormatIntValue)
        screenshotSourcePopup.selectItem(withTag: pref.screenshotSourceIntValue)
        screenshotCutoutPopup.selectItem(withTag: pref.screenshotCutoutIntValue)
        screenshotCutoutPopup.isHidden = framebuffer
        screenshotCutoutText.isHidden = framebuffer
        screenshotWidth.integerValue = pref.screenshotWidth
        screenshotWidth.isHidden = !custom || framebuffer
        screenshotWidthText.isHidden = !custom || framebuffer
        screenshotHeight.integerValue = pref.screenshotHeight
        screenshotHeight.isHidden = !custom || framebuffer
        screenshotHeightText.isHidden = !custom || framebuffer
    }

    //
    // Action methods
    //

    @IBAction func autoDeleteSnapshotAction(_ sender: NSButton!) {

        pref.snapshotAutoDelete = sender.state == .on
        refresh()
    }

    /*
    @IBAction func snapshotStorageAction(_ sender: NSTextField!) {

        if sender.integerValue > 0 {
            pref.snapshotStorage = sender.integerValue
        }
        refresh()
    }

    @IBAction func autoSnapshotAction(_ sender: NSButton!) {

        pref.autoSnapshots = sender.state == .on
        refresh()
    }

    @IBAction func snapshotIntervalAction(_ sender: NSTextField!) {

        print("snapshotIntervalAction: \(sender.integerValue)")
        if sender.integerValue > 0 {
            pref.snapshotInterval = sender.integerValue
        }
        refresh()
    }
    */

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

    @IBAction func retroVisorAction(_ sender: Any!) {

        if let url = URL(string: "https://dirkwhoffmann.github.io/RetroVisor/") {
            NSWorkspace.shared.open(url)
        }
    }

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

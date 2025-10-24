// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class SettingsViewController: NSViewController {

    @IBOutlet weak var presetPopup: NSPopUpButton!
    @IBOutlet weak var lockImage: NSButton!
    @IBOutlet weak var lockInfo1: NSTextField!
    @IBOutlet weak var lockInfo2: NSTextField!

    var pref: Preferences { myAppDelegate.pref }
    var controller: MyController? { MyAppDelegate.currentController }
    var config: Configuration? { return controller?.config }
    var gamePadManager: GamePadManager? { controller?.gamePadManager }
    var renderer: Renderer? { controller?.renderer }
    var emu: EmulatorProxy? { controller?.emu }

    var showLock: Bool { false }

    func activate() {

        refresh()
    }

    func refresh() {

        if emu?.poweredOff == true {

            lockInfo1.isHidden = true
            lockInfo2.isHidden = true
            lockImage.isHidden = false
            lockImage.image = NSImage(systemSymbolName: "power",
                                      accessibilityDescription: "Power")

        } else {

            lockInfo1.isHidden = !showLock
            lockInfo2.isHidden = !showLock
            lockImage.isHidden = !showLock
            lockImage.image = NSImage(systemSymbolName: "lock.fill",
                                      accessibilityDescription: "Lock")
        }
    }

    override func keyDown(with event: NSEvent) { }
    override func flagsChanged(with event: NSEvent) { }
    func preset(tag: Int) { }
    func save() { }

    @IBAction func presetAction(_ sender: NSPopUpButton) {

        preset(tag: sender.selectedTag())
    }

    @IBAction func presetMenuAction(_ sender: NSMenuItem!) {

        preset(tag: sender.tag)
    }

    @IBAction func saveAction(_ sender: Any) {

        save()
    }

    @IBAction func unlockAction(_ sender: Any!) {
        
        if emu?.poweredOff == true {
            try? emu?.run()
        } else {
            emu?.powerOff()
        }
        refresh()
    }
}

class SettingsView: NSView {

    private var bgLayer: CALayer?

    override func viewDidMoveToSuperview() {

        super.viewDidMoveToSuperview()
        wantsLayer = true

        if let layer = self.layer, bgLayer == nil {

            let background = CALayer()
            background.contents = nil // NSImage(named: "settingsBg")
            background.contentsGravity = .resizeAspectFill
            background.autoresizingMask = [.layerWidthSizable, .layerHeightSizable]
            layer.insertSublayer(background, at: 0)
            bgLayer = background
        }
    }

    override func layout() {

        super.layout()
        bgLayer?.frame = CGRect(x: 0, y: 0,
                                width: bounds.width,
                                height: bounds.height - 32)
    }
}

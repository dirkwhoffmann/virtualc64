// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import Carbon.HIToolbox

class ControlsSettingsViewController: SettingsViewController {

    // Tag of the button that is currently being recorded
    var recordedKey: Int?

    // Emulation keys
    @IBOutlet weak var left1: NSTextField!
    @IBOutlet weak var left1button: NSButton!
    @IBOutlet weak var right1: NSTextField!
    @IBOutlet weak var right1button: NSButton!
    @IBOutlet weak var up1: NSTextField!
    @IBOutlet weak var up1button: NSButton!
    @IBOutlet weak var down1: NSTextField!
    @IBOutlet weak var down1button: NSButton!
    @IBOutlet weak var fire1: NSTextField!
    @IBOutlet weak var fire1button: NSButton!

    @IBOutlet weak var left2: NSTextField!
    @IBOutlet weak var left2button: NSButton!
    @IBOutlet weak var right2: NSTextField!
    @IBOutlet weak var right2button: NSButton!
    @IBOutlet weak var up2: NSTextField!
    @IBOutlet weak var up2button: NSButton!
    @IBOutlet weak var down2: NSTextField!
    @IBOutlet weak var down2button: NSButton!
    @IBOutlet weak var fire2: NSTextField!
    @IBOutlet weak var fire2button: NSButton!

    @IBOutlet weak var mouseLeft: NSTextField!
    @IBOutlet weak var mouseLeftButton: NSButton!
    @IBOutlet weak var mouseMiddle: NSTextField!
    @IBOutlet weak var mouseMiddleButton: NSButton!
    @IBOutlet weak var mouseRight: NSTextField!
    @IBOutlet weak var mouseRightButton: NSButton!

    @IBOutlet weak var disconnectKeys: NSButton!
    @IBOutlet weak var amigaKeysCombEnable: NSButton!
    @IBOutlet weak var amigaKeysCombButton: NSPopUpButton!

    //
    // Methods from SettingsViewController
    //

    override func refresh() {

        super.refresh()

        // Mouse button keyset
        refreshKey(map: 0, dir: .PRESS_LEFT, button: mouseLeftButton, txt: mouseLeft)
        refreshKey(map: 0, dir: .PRESS_RIGHT, button: mouseRightButton, txt: mouseRight)

        // First joystick keyset
        refreshKey(map: 1, dir: .PULL_UP, button: up1button, txt: up1)
        refreshKey(map: 1, dir: .PULL_DOWN, button: down1button, txt: down1)
        refreshKey(map: 1, dir: .PULL_LEFT, button: left1button, txt: left1)
        refreshKey(map: 1, dir: .PULL_RIGHT, button: right1button, txt: right1)
        refreshKey(map: 1, dir: .PRESS_FIRE, button: fire1button, txt: fire1)

        // Second joystick keyset
        refreshKey(map: 2, dir: .PULL_UP, button: up2button, txt: up2)
        refreshKey(map: 2, dir: .PULL_DOWN, button: down2button, txt: down2)
        refreshKey(map: 2, dir: .PULL_LEFT, button: left2button, txt: left2)
        refreshKey(map: 2, dir: .PULL_RIGHT, button: right2button, txt: right2)
        refreshKey(map: 2, dir: .PRESS_FIRE, button: fire2button, txt: fire2)

        disconnectKeys.state = pref.disconnectJoyKeys ? .on : .off
        // amigaKeysCombEnable.state = pref.amigaKeysCombEnable ? .on : .off
        // amigaKeysCombButton.selectItem(withTag: pref.amigaKeysComb)
    }

    func refreshKey(map: Int, dir: GamePadAction, button: NSButton, txt: NSTextField) {

        var keyDesc = ""
        var keyCode = ""

        // Which MacKey is assigned to this joystick action?
        for (key, direction) in pref.keyMaps[map] where direction == dir.rawValue {

            keyCode = NSString(format: "%02X", key.keyCode) as String
            keyDesc = key.stringValue
            break
        }

        // Update text and button image
        if button.tag == recordedKey {

            button.title = ""
            button.image = NSImage(named: "recordKeyRed")
            button.imageScaling = .scaleAxesIndependently

        } else {

            button.image = NSImage(named: "recordKey")
            button.imageScaling = .scaleAxesIndependently
        }
        button.title = keyCode
        txt.stringValue = keyDesc
    }

    // Translates a button tag back to the related slot and gamepad action
    func gamePadAction(for tag: Int) -> (Int, GamePadAction) {

        switch tag {
        case 7...9:   return (0, GamePadAction(rawValue: tag)!)      // Mouse
        case 0...4:   return (1, GamePadAction(rawValue: tag)!)      // Joy 1
        case 10...14: return (2, GamePadAction(rawValue: tag - 10)!) // Joy 2
        default:      fatalError()
        }
    }

    //
    // Keyboard events
    //

    override func keyDown(with event: NSEvent) {

        if keyDown(with: MacKey(event: event)) {
            return
        }

        // The controller wasn't interested. Process it as usual
        interpretKeyEvents([event])
    }

    override func flagsChanged(with event: NSEvent) {

        switch Int(event.keyCode) {

        case kVK_Shift where event.modifierFlags.contains(.shift):
            keyDown(with: MacKey.shift)
        case kVK_RightShift where event.modifierFlags.contains(.shift):
            keyDown(with: MacKey.rightShift)
        case kVK_Option where event.modifierFlags.contains(.option):
            keyDown(with: MacKey.option)
        case kVK_RightOption where event.modifierFlags.contains(.option):
            keyDown(with: MacKey.rightOption)

        default:
            break
        }
    }

    @discardableResult
    func keyDown(with macKey: MacKey) -> Bool {

        // Only proceed if a recording sessing is in progress
        guard let key = recordedKey else { return false }

        // Record the key if it is not the ESC key
        if macKey != MacKey.escape {

            let (slot, action) = gamePadAction(for: key)
            gamePadManager?.gamePads[slot]!.bind(key: macKey, action: action)
        }

        recordedKey = nil
        refresh()
        return true
    }

    //
    // Action methods
    //

    @IBAction func recordKeyAction(_ sender: NSButton!) {

        recordedKey = sender.tag
        refresh()
    }

    @IBAction func deleteKeysetAction(_ sender: NSButton!) {

        assert(sender.tag >= 0 && sender.tag <= 2)

        pref.keyMaps[sender.tag] = [:]
        refresh()
    }

    @IBAction func disconnectKeysAction(_ sender: NSButton!) {

        pref.disconnectJoyKeys = (sender.state == .on)
        refresh()
    }

    @IBAction func amigaKeysCombEnableAction(_ sender: NSButton!) {

        // pref.amigaKeysCombEnable = sender.state == .on
        refresh()
    }

    @IBAction func amigaKeysCombPopupAction(_ sender: NSButton!) {

        // pref.amigaKeysComb = sender.tag
        refresh()
    }

    override func preset(tag: Int) {

        // Revert to standard settings
        EmulatorProxy.defaults.removeControlsUserDefaults()

        // Apply the new settings
        pref.applyControlsUserDefaults()
    }

    override func save() {

        pref.saveControlsUserDefaults()
    }
}

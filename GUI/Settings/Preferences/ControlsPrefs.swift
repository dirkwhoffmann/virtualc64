// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension PreferencesController {
    
    func refreshControlsTab() {
        
        track()
        
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
            if button.tag == conRecordedKey {
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
        
        // First joystick keyset
        refreshKey(map: 1, dir: .PULL_UP, button: conUp1button, txt: conUp1)
        refreshKey(map: 1, dir: .PULL_DOWN, button: conDown1button, txt: conDown1)
        refreshKey(map: 1, dir: .PULL_LEFT, button: conLeft1button, txt: conLeft1)
        refreshKey(map: 1, dir: .PULL_RIGHT, button: conRight1button, txt: conRight1)
        refreshKey(map: 1, dir: .PRESS_FIRE, button: conFire1button, txt: conFire1)
        
        // Second joystick keyset
        refreshKey(map: 2, dir: .PULL_UP, button: conUp2button, txt: conUp2)
        refreshKey(map: 2, dir: .PULL_DOWN, button: conDown2button, txt: conDown2)
        refreshKey(map: 2, dir: .PULL_LEFT, button: conLeft2button, txt: conLeft2)
        refreshKey(map: 2, dir: .PULL_RIGHT, button: conRight2button, txt: conRight2)
        refreshKey(map: 2, dir: .PRESS_FIRE, button: conFire2button, txt: conFire2)
        
        conDisconnectKeys.state = pref.disconnectJoyKeys ? .on : .off
        
        // Joystick buttons
        conAutofire.state = pref.autofire ? .on : .off
        conAutofireCease.state = pref.autofireBullets > 0 ? .on : .off
        conAutofireBullets.integerValue = Int(pref.autofireBullets.magnitude)
        conAutofireFrequency.floatValue = pref.autofireFrequency
        conAutofireCease.isEnabled = conAutofire.state == .on
        conAutofireCeaseText.textColor = conAutofire.state == .on ? .controlTextColor : .disabledControlTextColor
        conAutofireBullets.isEnabled = conAutofire.state == .on
        conAutofireFrequency.isEnabled = conAutofire.state == .on
        
        // Mouse
        let model = c64.port1.mouse.model
        assert(model == c64.port2.mouse.model)
        conMouseModel.selectItem(withTag: model.rawValue)
        conMouseInfo.isHidden = model == .C1350
    }
    
    func selectControlsTab() {

        track()
        refreshControlsTab()
    }
    
    // Translates a button tag back to the related slot and gamepad action
    func gamePadAction(for tag: Int) -> (Int, GamePadAction) {
        
        switch tag {
        case 5...6:   return (0, GamePadAction(rawValue: tag)!)      // Mouse
        case 0...4:   return (1, GamePadAction(rawValue: tag)!)      // Joy 1
        case 10...14: return (2, GamePadAction(rawValue: tag - 10)!) // Joy 2
        default:      fatalError()
        }
    }
    
    //
    // Keyboard events
    //
    
    // Handles a key press event.
    // Returns true if the controller has responded to this key.
    func conKeyDown(with macKey: MacKey) -> Bool {
        
        track()
        
        // Only proceed if a recording sessing is in progress
        if conRecordedKey == nil { return false }
        
        track()
        
        // Record the key if it is not the ESC key
        if macKey != MacKey.escape {
            let (slot, action) = gamePadAction(for: conRecordedKey!)
            gamePadManager.gamePads[slot]?.bind(key: macKey, action: action)
        }
        
        conRecordedKey = nil
        refresh()
        return true
    }
    
    //
    // Action methods
    //
    
    @IBAction func conRecordKeyAction(_ sender: NSButton!) {
        
        conRecordedKey = sender.tag
        refresh()
    }
    
    @IBAction func conDisconnectKeysAction(_ sender: NSButton!) {
        
        pref.disconnectJoyKeys = (sender.state == .on)
        refresh()
    }
    
    @IBAction func conDeleteKeysetAction(_ sender: NSButton!) {

        assert(sender.tag >= 0 && sender.tag <= 2)
        
        pref.keyMaps[sender.tag] = [:]
        refresh()
    }

    @IBAction func conAutofireAction(_ sender: NSButton!) {
        
        pref.autofire = (sender.state == .on)
        refresh()
    }
    
    @IBAction func conAutofireCeaseAction(_ sender: NSButton!) {
        
        let sign = sender.state == .on ? 1 : -1
        let bullets = pref.autofireBullets.magnitude
        pref.autofireBullets = Int(bullets) * sign
        refresh()
    }
    
    @IBAction func conAutofireBulletsAction(_ sender: NSTextField!) {
        
        pref.autofireBullets = sender.integerValue
        refresh()
    }
    
    @IBAction func conAutofireFrequencyAction(_ sender: NSSlider!) {
        
        pref.autofireFrequency = sender.floatValue
        refresh()
    }
    
    @IBAction func conMouseModelAction(_ sender: NSPopUpButton!) {
        
        pref.mouseModelIntValue = sender.selectedTag()
        refresh()
    }
        
    //
    // Action methods (Misc)
    //
    
    @IBAction func conPresetAction(_ sender: NSPopUpButton!) {
        
        track()
        assert(sender.selectedTag() == 0)
        
        UserDefaults.resetControlsUserDefaults()
        pref.loadControlsUserDefaults()
        refresh()
    }
}

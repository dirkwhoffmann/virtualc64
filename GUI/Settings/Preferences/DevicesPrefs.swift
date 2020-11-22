// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension PreferencesController {
    
    func refreshDevicesTab() {
        
        let port1 = c64.port1!
        let port2 = c64.port2!
        
        func refreshKey(map: Int, dir: GamePadAction, button: NSButton, txt: NSTextField) {
            
            var macKeyCode: NSAttributedString = NSAttributedString.init()
            var macKeyDesc: String = ""
            
            // Which MacKey is assigned to this joystick action?
            for (key, direction) in pref.keyMaps[map] where direction == dir.rawValue {
                let attr = [NSAttributedString.Key.foregroundColor: NSColor.black]
                let myStr = NSString(format: "%02X", key.keyCode) as String
                macKeyCode = NSAttributedString(string: myStr, attributes: attr)
                macKeyDesc = key.stringValue
                break
            }
            
            // Update text and button image
            if button.tag == devRecordedKey {
                button.title = ""
                button.image = NSImage(named: "key_red")
                button.imageScaling = .scaleAxesIndependently
            } else {
                button.image = NSImage(named: "key")
                button.imageScaling = .scaleAxesIndependently
            }
            button.attributedTitle = macKeyCode
            txt.stringValue = macKeyDesc
        }
        
        // First joystick keyset
        refreshKey(map: 0, dir: .PULL_UP, button: devUp1button, txt: devUp1)
        refreshKey(map: 0, dir: .PULL_DOWN, button: devDown1button, txt: devDown1)
        refreshKey(map: 0, dir: .PULL_LEFT, button: devLeft1button, txt: devLeft1)
        refreshKey(map: 0, dir: .PULL_RIGHT, button: devRight1button, txt: devRight1)
        refreshKey(map: 0, dir: .PRESS_FIRE, button: devFire1button, txt: devFire1)
        
        // Second joystick keyset
        refreshKey(map: 1, dir: .PULL_UP, button: devUp2button, txt: devUp2)
        refreshKey(map: 1, dir: .PULL_DOWN, button: devDown2button, txt: devDown2)
        refreshKey(map: 1, dir: .PULL_LEFT, button: devLeft2button, txt: devLeft2)
        refreshKey(map: 1, dir: .PULL_RIGHT, button: devRight2button, txt: devRight2)
        refreshKey(map: 1, dir: .PRESS_FIRE, button: devFire2button, txt: devFire2)
        
        devDisconnectKeys.state = pref.disconnectJoyKeys ? .on : .off
        
        // Joystick buttons
         assert(pref.autofire == port2.autofire())
         assert(pref.autofireBullets == port2.autofireBullets())
         assert(pref.autofireFrequency == port2.autofireFrequency())
         assert(port1.autofire() == port2.autofire())
         assert(port1.autofireBullets() == port2.autofireBullets())
         assert(port1.autofireFrequency() == port2.autofireFrequency())
         devAutofire.state = pref.autofire ? .on : .off
         devAutofireCease.state = pref.autofireBullets > 0 ? .on : .off
         devAutofireBullets.integerValue = Int(pref.autofireBullets.magnitude)
         devAutofireFrequency.floatValue = pref.autofireFrequency
         devAutofireCease.isEnabled = devAutofire.state == .on
         devAutofireCeaseText.textColor = devAutofire.state == .on ? .controlTextColor : .disabledControlTextColor
         devAutofireBullets.isEnabled = devAutofire.state == .on
         devAutofireFrequency.isEnabled = devAutofire.state == .on
        
        // Mouse
        let model = c64.mouse.model()
        devMouseModel.selectItem(withTag: model.rawValue)
        devMouseInfo.isHidden = model == .MOUSE1350
    }
    
    // Translates a button tag back to the related slot and gamepad action
    func gamePadAction(for tag: Int) -> (Int, GamePadAction) {
        
        switch tag {
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
    func devKeyDown(with macKey: MacKey) -> Bool {
        
        // Only proceed if a recording sessing is in progress
        if devRecordedKey == nil { return false }
        
        track()
        
        // Record the key if it is not the ESC key
        if macKey != MacKey.escape {
            let (slot, action) = gamePadAction(for: devRecordedKey!)
            gamePadManager.gamePads[slot]?.bind(key: macKey, action: action)
        }
        
        devRecordedKey = nil
        refresh()
        return true
    }
    
    //
    // Action methods
    //
    
    @IBAction func devRecordKeyAction(_ sender: NSButton!) {
        
        devRecordedKey = sender.tag
        refresh()
    }
    
    @IBAction func devDisconnectKeysAction(_ sender: NSButton!) {
        
        pref.disconnectJoyKeys = (sender.state == .on)
        refresh()
    }
    
    @IBAction func devDeleteKeysetAction(_ sender: NSButton!) {

        assert(sender.tag >= 0 && sender.tag <= 2)
        
        pref.keyMaps[sender.tag] = [:]
        refresh()
    }

    @IBAction func devAutofireAction(_ sender: NSButton!) {
        
        pref.autofire = (sender.state == .on)
        refresh()
    }
    
    @IBAction func devAutofireCeaseAction(_ sender: NSButton!) {
        
        let sign = sender.state == .on ? 1 : -1
        let bullets = pref.autofireBullets.magnitude
        pref.autofireBullets = Int(bullets) * sign
        refresh()
    }
    
    @IBAction func devAutofireBulletsAction(_ sender: NSTextField!) {
        
        pref.autofireBullets = sender.integerValue
        refresh()
    }
    
    @IBAction func devAutofireFrequencyAction(_ sender: NSSlider!) {
        
        pref.autofireFrequency = sender.floatValue
        refresh()
    }
    
    @IBAction func devMouseModelAction(_ sender: NSPopUpButton!) {
        
        proxy?.mouse.setModel(MouseModel.init(rawValue: sender.selectedTag())!)
        refresh()
    }
        
    //
    // Action methods (Misc)
    //
    
    @IBAction func devPresetAction(_ sender: NSPopUpButton!) {
        
        track()
        assert(sender.selectedTag() == 0)
        
        UserDefaults.resetDevicesUserDefaults()
        pref.loadDevicesUserDefaults()
        refresh()
    }
}

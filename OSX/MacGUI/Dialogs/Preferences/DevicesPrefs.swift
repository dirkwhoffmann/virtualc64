//
// This file is part of VirtualC64 - A cycle accurate Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

extension PreferencesController {
    
    func refreshDevicesTab() {
        
        track()
    
        // Joystick emulation keys
        updateJoyKeyMap(0, dir: JOYSTICK_UP, button: devUp1button, txt: devUp1)
        updateJoyKeyMap(0, dir: JOYSTICK_DOWN, button: devDown1button, txt: devDown1)
        updateJoyKeyMap(0, dir: JOYSTICK_LEFT, button: devLeft1button, txt: devLeft1)
        updateJoyKeyMap(0, dir: JOYSTICK_RIGHT, button: devRight1button, txt: devRight1)
        updateJoyKeyMap(0, dir: JOYSTICK_FIRE, button: devFire1button, txt: devFire1)
        updateJoyKeyMap(1, dir: JOYSTICK_UP, button: devUp2button, txt: devUp2)
        updateJoyKeyMap(1, dir: JOYSTICK_DOWN, button: devDown2button, txt: devDown2)
        updateJoyKeyMap(1, dir: JOYSTICK_LEFT, button: devLeft2button, txt: devLeft2)
        updateJoyKeyMap(1, dir: JOYSTICK_RIGHT, button: devRight2button, txt: devRight2)
        updateJoyKeyMap(1, dir: JOYSTICK_FIRE, button: devFire2button, txt: devFire2)
        devDisconnectKeys.state = parent.keyboardcontroller.disconnectJoyKeys ? .on : .off
        
        assert(c64.port1.autofire() == c64.port2.autofire())
        assert(c64.port1.autofireBullets() == c64.port2.autofireBullets())
        assert(c64.port1.autofireFrequency() == c64.port2.autofireFrequency())
        
        // Joystick buttons
        devAutofire.state = c64.port1.autofire() ? .on : .off
        devAutofireCease.state = c64.port1.autofireBullets() > 0 ? .on : .off
        devAutofireBullets.integerValue = Int(c64.port1.autofireBullets().magnitude)
        devAutofireFrequency.floatValue = c64.port1.autofireFrequency()
        devAutofireCease.isEnabled = devAutofire.state == .on
        devAutofireCeaseText.textColor = devAutofire.state == .on ? .controlTextColor : .disabledControlTextColor
        devAutofireBullets.isEnabled = devAutofire.state == .on
        devAutofireFrequency.isEnabled = devAutofire.state == .on
        
        // Mouse
        let model = c64.mouse.model()
        devMouseModel.selectItem(withTag: model)
        devMouseInfo.isHidden = (model == Int(MOUSE1350.rawValue))
        
        devOkButton.title = parent.c64.isRunnable() ? "OK" : "Quit"
    }
    
    func updateJoyKeyMap(_ nr: Int, dir: JoystickDirection, button: NSButton, txt: NSTextField) {
        
        precondition(nr == 0 || nr == 1)
        
        let keyMap = parent.gamePadManager.gamePads[nr]?.keyMap
        
        // Which MacKey is assigned to this joystick action?
        var macKey: MacKey?
        var macKeyCode: NSAttributedString = NSAttributedString.init()
        var macKeyDesc: String = ""
        for (key, direction) in keyMap! {
            if direction == dir.rawValue {
                let attr = [NSAttributedString.Key.foregroundColor: NSColor.black]
                macKey = key
                let myStr = NSString(format: "%02X", macKey!.keyCode) as String
                macKeyCode = NSAttributedString(string: myStr, attributes: attr)
                macKeyDesc = macKey?.description?.uppercased() ?? ""
                break
            }
        }
        
        // Update text and button image
        let recordKey = (nr == 0) ? devRecordKey1 : devRecordKey2
        if (recordKey == dir) {
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
    
    //
    // Keyboard events
    //
    
    func devKeyDown(with macKey: MacKey) {
        
        track()
        
        // Check for ESC key
        if macKey == MacKey.escape {
            cancelAction(self)
            return
        }
        
        if devRecordKey1 != nil {
            
            parent.gamePadManager.gamePads[0]?.assign(key: macKey, direction: devRecordKey1!)
            devRecordKey1 = nil
        }
        if devRecordKey2 != nil {
            
            parent.gamePadManager.gamePads[1]?.assign(key: macKey, direction: devRecordKey2!)
            devRecordKey2 = nil
        }
        
        refresh()
    }
    
    //
    // Action methods
    //
    
    @IBAction func devRecordKeyAction(_ sender: NSButton!) {
        
        let tag = UInt32(sender.tag)
        
        if tag >= 0 && tag <= 5 {
            devRecordKey1 = JoystickDirection(rawValue: tag)
            devRecordKey2 = nil
        } else if tag >= 10 && tag <= 15 {
            devRecordKey1 = nil
            devRecordKey2 = JoystickDirection(rawValue: (tag - 10))
        } else {
            assert(false);
        }
        
        refresh()
    }
    
    @IBAction func devDisconnectKeysAction(_ sender: NSButton!) {
        
        parent.keyboardcontroller.disconnectJoyKeys = (sender.state == .on)
        refresh()
    }
    
    @IBAction func devAutofireAction(_ sender: NSButton!) {
        
        let value = sender.state
        track("value = \(value)")
        c64.port1.setAutofire(sender.state == .on)
        c64.port2.setAutofire(sender.state == .on)
        refresh()
    }
    
    @IBAction func devAutofireCeaseAction(_ sender: NSButton!) {
        
        let value = sender.state
        track("value = \(value)")
        
        assert(c64.port1.autofireBullets() == c64.port2.autofireBullets())
        let bullets = Int(c64.port1.autofireBullets().magnitude)
        let sign = sender.state == .on ? 1 : -1;
        c64.port1.setAutofireBullets(bullets * sign)
        c64.port2.setAutofireBullets(bullets * sign)
        refresh()
    }
    
    @IBAction func devAutofireBulletsAction(_ sender: NSTextField!) {
        
        let value = sender.integerValue
        track("value = \(value)")
        c64.port1.setAutofireBullets(value)
        c64.port2.setAutofireBullets(value)
        refresh()
    }
    
    @IBAction func devAutofireFrequencyAction(_ sender: NSSlider!) {
        
        let value = sender.floatValue
        track("value = \(value)")
        c64.port1.setAutofireFrequency(value)
        c64.port2.setAutofireFrequency(value)
        refresh()
    }
    
    @IBAction func devMouseModelAction(_ sender: NSPopUpButton!) {
        
        c64.mouse.setModel(sender.selectedTag())
        refresh()
    }
    
    @IBAction func devHelpAction(_ sender: Any!) {
        
        if let url = URL.init(string: "http://www.dirkwhoffmann.de/virtualc64/ROMs.html") {
            NSWorkspace.shared.open(url)
        }
    }
    
    @IBAction func devFactorySettingsAction(_ sender: Any!) {
        
        parent.resetDevicesUserDefaults()

        /*
        // Joystick emulation keys
        parent.gamePadManager.restoreFactorySettings()
        parent.keyboardcontroller.disconnectJoyKeys = Defaults.disconnectJoyKeys
        
        // Autofire
        c64.port1.setAutofire(Defaults.autofire)
        c64.port2.setAutofire(Defaults.autofire)
        c64.port1.setAutofireBullets(Defaults.autofireBullets)
        c64.port2.setAutofireBullets(Defaults.autofireBullets)
        c64.port1.setAutofireFrequency(Defaults.autofireFrequency)
        c64.port2.setAutofireFrequency(Defaults.autofireFrequency)
        
        // Mouse
        c64.mouse.setModel(Int(Defaults.mouseModel.rawValue))
        */
        
        refresh()
    }
}

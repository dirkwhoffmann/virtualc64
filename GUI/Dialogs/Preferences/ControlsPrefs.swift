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
                
        func refreshKey(map: Int, dir: vc64.GamePadAction, button: NSButton, txt: NSTextField) {

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
        
        // Emulation keys
        refreshKey(map: 0, dir: .PRESS_LEFT, button: conMouseLeftButton, txt: conMouseLeft)
        refreshKey(map: 0, dir: .PRESS_RIGHT, button: conMouseRightButton, txt: conMouseRight)

        refreshKey(map: 1, dir: .PULL_UP, button: conUp1button, txt: conUp1)
        refreshKey(map: 1, dir: .PULL_DOWN, button: conDown1button, txt: conDown1)
        refreshKey(map: 1, dir: .PULL_LEFT, button: conLeft1button, txt: conLeft1)
        refreshKey(map: 1, dir: .PULL_RIGHT, button: conRight1button, txt: conRight1)
        refreshKey(map: 1, dir: .PRESS_FIRE, button: conFire1button, txt: conFire1)
        
        refreshKey(map: 2, dir: .PULL_UP, button: conUp2button, txt: conUp2)
        refreshKey(map: 2, dir: .PULL_DOWN, button: conDown2button, txt: conDown2)
        refreshKey(map: 2, dir: .PULL_LEFT, button: conLeft2button, txt: conLeft2)
        refreshKey(map: 2, dir: .PULL_RIGHT, button: conRight2button, txt: conRight2)
        refreshKey(map: 2, dir: .PRESS_FIRE, button: conFire2button, txt: conFire2)
        
        conDisconnectKeys.state = pref.disconnectJoyKeys ? .on : .off
        
        // Mouse
        conRetainMouseKeyComb.selectItem(withTag: pref.retainMouseKeyComb)
        conRetainMouseKeyComb.isEnabled = pref.retainMouseWithKeys
        conRetainMouseWithKeys.state = pref.retainMouseWithKeys ? .on : .off
        conRetainMouseByClick.state = pref.retainMouseByClick ? .on : .off
        conRetainMouseByEntering.state = pref.retainMouseByEntering ? .on : .off
        conReleaseMouseKeyComb.selectItem(withTag: pref.releaseMouseKeyComb)
        conReleaseMouseKeyComb.isEnabled = pref.releaseMouseWithKeys
        conReleaseMouseWithKeys.state = pref.releaseMouseWithKeys ? .on : .off
        conReleaseMouseByShaking.state = pref.releaseMouseByShaking ? .on : .off
    }
    
    func selectControlsTab() {

        refreshControlsTab()
    }
    
    // Translates a button tag back to the related slot and gamepad action
    func gamePadAction(for tag: Int) -> (Int, vc64.GamePadAction) {
        
        switch tag {
        case 5...6:   return (0, vc64.GamePadAction(rawValue: tag)!)      // Mouse
        case 0...4:   return (1, vc64.GamePadAction(rawValue: tag)!)      // Joy 1
        case 10...14: return (2, vc64.GamePadAction(rawValue: tag - 10)!) // Joy 2
        default:      fatalError()
        }
    }
    
    //
    // Keyboard events
    //
    
    // Handles a key press event.
    // Returns true if the controller has responded to this key.
    func conKeyDown(with macKey: MacKey) -> Bool {

        // Only proceed if a recording sessing is in progress
        if conRecordedKey == nil { return false }

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
            
    @IBAction func conRetainMouseKeyCombAction(_ sender: NSPopUpButton!) {
        
        pref.retainMouseKeyComb = sender.selectedTag()
        refresh()
    }
    
    @IBAction func conRetainMouseAction(_ sender: NSButton!) {
        
        switch sender.tag {
            
        case 0: pref.retainMouseWithKeys   = (sender.state == .on)
        case 1: pref.retainMouseByClick    = (sender.state == .on)
        case 2: pref.retainMouseByEntering = (sender.state == .on)
        default: fatalError()
        }

        refresh()
    }
    
    @IBAction func conReleaseMouseKeyCombAction(_ sender: NSPopUpButton!) {
        
        pref.releaseMouseKeyComb = sender.selectedTag()
        refresh()
    }
    
    @IBAction func conReleaseMouseAction(_ sender: NSButton!) {
        
        switch sender.tag {
            
        case 0: pref.releaseMouseWithKeys  = (sender.state == .on)
        case 1: pref.releaseMouseByShaking = (sender.state == .on)
        default: fatalError()
        }
        
        refresh()
    }
    
    //
    // Action methods (Misc)
    //
    
    @IBAction func conPresetAction(_ sender: NSPopUpButton!) {
        
        assert(sender.selectedTag() == 0)

        // Revert to standard settings
        EmulatorProxy.defaults.removeControlsUserDefaults()

        // Apply the new settings
        pref.applyControlsUserDefaults()
        refresh()
    }
}

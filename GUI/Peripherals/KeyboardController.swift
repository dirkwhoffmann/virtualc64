// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import Carbon.HIToolbox

class KeyboardController: NSObject {
    
    var myAppDelegate: MyAppDelegate { return NSApp.delegate as! MyAppDelegate }
    var parent: MyController!
        
    var emu: EmulatorProxy? { return parent.emu }
    var keyboard: KeyboardProxy? { return emu?.keyboard }
    var renderer: Renderer { return parent.renderer }
    var pref: Preferences { return parent.pref }
    
    // Remembers the state of some keys (true = currently pressed)
    var leftShift   = false, rightShift   = false
    var leftControl = false, rightControl = false
    var leftOption  = false, rightOption  = false
    var leftCommand = false, rightCommand = false
    var capsLock    = false

    // Remembers the currently pressed keys and their assigned C64 key list.
    // This variable is only used when keys are mapped symbolically. It it
    // written in keyDown and picked up in keyUp.
    var pressedKeys: [Int: [C64Key]] = [:]
        
    init(parent: MyController) {
        
        super.init()
        self.parent = parent
    }
    
    func keyDown(with event: NSEvent) {
                
        // Intercept if the console is open
        if renderer.console.isVisible { renderer.console.keyDown(with: event); return }

        // Ignore repeating keys
        if event.isARepeat { return }
        
        // Exit fullscreen mode if escape key is pressed
        if event.keyCode == kVK_Escape && renderer.fullscreen && pref.exitOnEsc {
            parent.window!.toggleFullScreen(nil)
            return
        }
        
        // Ignore keys that are pressed in combination with the Command key
        if event.modifierFlags.contains(NSEvent.ModifierFlags.command) { return }
        
        keyDown(with: MacKey(event: event))
    }
    
    func keyUp(with event: NSEvent) {
                
        // Intercept if the console is open
        if renderer.console.isVisible { renderer.console.keyUp(with: event); return }
        
        keyUp(with: MacKey(event: event))
    }
    
    func flagsChanged(with event: NSEvent) {
                
        debug(.events,
              "ls: \(leftShift) rs: \(rightShift) lc: \(leftControl) " +
              "rc: \(rightControl) lo: \(leftOption) ro: \(rightOption)")
        
        // Determine the pressed or released key
        switch Int(event.keyCode) {
            
        case kVK_Shift:
            leftShift = event.modifierFlags.contains(.shift) ? !leftShift : false
            if leftShift { keyDown(with: MacKey.shift) } else { keyUp(with: MacKey.shift) }

        case kVK_RightShift:
            rightShift = event.modifierFlags.contains(.shift) ? !rightShift : false
            if rightShift { keyDown(with: MacKey.rightShift) } else { keyUp(with: MacKey.rightShift) }

        case kVK_Control:
            leftControl = event.modifierFlags.contains(.control) ? !leftControl : false
            if leftControl { keyDown(with: MacKey.control) } else { keyUp(with: MacKey.control) }

        case kVK_RightControl:
            rightControl = event.modifierFlags.contains(.control) ? !rightControl : false
            if rightControl { keyDown(with: MacKey.rightControl) } else { keyUp(with: MacKey.rightControl) }

        case kVK_Option:
            leftOption = event.modifierFlags.contains(.option) ? !leftOption : false
            if leftOption { keyDown(with: MacKey.option) } else { keyUp(with: MacKey.option) }

        case kVK_RightOption:
            rightOption = event.modifierFlags.contains(.option) ? !rightOption : false
            if rightOption { keyDown(with: MacKey.rightOption) } else { keyUp(with: MacKey.rightOption) }

        case kVK_Command where myAppDelegate.mapLeftCmdKey != nil:
            let key = myAppDelegate.mapLeftCmdKey!
            leftCommand = event.modifierFlags.contains(.command) ? !leftCommand : false
            myApp.disableCmdKey = leftCommand
            if leftCommand { keyDown(with: key) } else { keyUp(with: key) }

        case kVK_RightCommand where myAppDelegate.mapRightCmdKey != nil:
            let key = myAppDelegate.mapRightCmdKey!
            rightCommand = event.modifierFlags.contains(.command) ? !rightCommand : false
            myApp.disableCmdKey = rightCommand
            if rightCommand { keyDown(with: key) } else { keyUp(with: key) }

        case kVK_CapsLock where myAppDelegate.mapCapsLockWarp:
            capsLock = event.modifierFlags.contains(.capsLock)
            if capsLock { capsLockDown() } else { capsLockUp() }

        default:
            break
        }
    }
    
    func keyDown(with macKey: MacKey) {

        // Abort auto-typing (if active)
        keyboard?.abortAutoTyping()

        // Check if this key is used to emulate a game device
        if parent.gamePad1?.processKeyDownEvent(macKey: macKey) == true {
            if pref.disconnectJoyKeys { return }
        }
        if parent.gamePad2?.processKeyDownEvent(macKey: macKey) == true {
            if pref.disconnectJoyKeys { return }
        }

        // Check mapping mode
        if pref.mapKeysByPosition {

            // Positional key mapping
            keyDown(with: macKey, keyMap: pref.keyMap)

        } else {
            
            // Symbolic key mapping
            let c64Keys = translate(macKey: macKey)
            
            if c64Keys != [] {
                
                // Store key combination for later use in keyUp
                pressedKeys[macKey.keyCode] = c64Keys
                
                // Press all required keys
                for key in c64Keys {

                    keyboard?.pressKey(key.nr)
                }

                parent.virtualKeyboard?.refresh()
            }
        }
    }
    
    func keyDown(with macKey: MacKey, keyMap: [MacKey: C64Key]) {
        
        if let c64Key = keyMap[macKey] { keyDown(with: c64Key) }
    }

    func keyDown(with c64Key: C64Key) {

        keyboard?.pressKey(c64Key.nr)
        parent.virtualKeyboard?.refresh()
    }

    func keyUp(with macKey: MacKey) {
        
        // Check if this key is used to emulate a game device
        if parent.gamePad1?.processKeyUpEvent(macKey: macKey) == true {
            if pref.disconnectJoyKeys { return }
        }
        if parent.gamePad2?.processKeyUpEvent(macKey: macKey) == true {
            if pref.disconnectJoyKeys { return }
        }

        // Check mapping mode
        if pref.mapKeysByPosition {

            // Positional key mapping
            keyUp(with: macKey, keyMap: pref.keyMap)
            
        } else {
            
            // Symbolic key mapping
            if let c64Keys = pressedKeys[macKey.keyCode] {
                for key in c64Keys {
                    keyboard?.releaseKey(key.nr)
                }
            }

            parent.virtualKeyboard?.refresh()
        }
    }
    
    func keyUp(with macKey: MacKey, keyMap: [MacKey: C64Key]) {
        
        if let c64Key = keyMap[macKey] { keyUp(with: c64Key) }
    }

    func keyUp(with c64Key: C64Key) {

        keyboard?.releaseKey(c64Key.nr)
        parent.virtualKeyboard?.refresh()
    }

    // Standard physical key map
    static let standardKeyMap: [MacKey: C64Key] = [
        
        // First row of C64 keyboard
        MacKey.Ansi.grave: C64Key.leftArrow,
        // MacKey.Iso.hat: C64Key.leftArrow,
        MacKey.Ansi.digit0: C64Key.digit0,
        MacKey.Ansi.digit1: C64Key.digit1,
        MacKey.Ansi.digit2: C64Key.digit2,
        MacKey.Ansi.digit3: C64Key.digit3,
        MacKey.Ansi.digit4: C64Key.digit4,
        MacKey.Ansi.digit5: C64Key.digit5,
        MacKey.Ansi.digit6: C64Key.digit6,
        MacKey.Ansi.digit7: C64Key.digit7,
        MacKey.Ansi.digit8: C64Key.digit8,
        MacKey.Ansi.digit9: C64Key.digit9,
        MacKey.Ansi.minus: C64Key.minus,
        MacKey.Ansi.equal: C64Key.plus,
        MacKey.delete: C64Key.delete,
        MacKey.F1: C64Key.F1F2,
        
        // Second row of C64 keyboard
        MacKey.tab: C64Key.control,
        MacKey.Ansi.Q: C64Key.Q,
        MacKey.Ansi.W: C64Key.W,
        MacKey.Ansi.E: C64Key.E,
        MacKey.Ansi.R: C64Key.R,
        MacKey.Ansi.T: C64Key.T,
        MacKey.Ansi.Y: C64Key.Y,
        MacKey.Ansi.U: C64Key.U,
        MacKey.Ansi.I: C64Key.I,
        MacKey.Ansi.O: C64Key.O,
        MacKey.Ansi.P: C64Key.P,
        MacKey.Ansi.leftBracket: C64Key.at,
        MacKey.Ansi.rightBracket: C64Key.asterisk,
        MacKey.F3: C64Key.F3F4,
        
        // Third row of C64 keyboard
        MacKey.control: C64Key.runStop,
        MacKey.Ansi.A: C64Key.A,
        MacKey.Ansi.S: C64Key.S,
        MacKey.Ansi.D: C64Key.D,
        MacKey.Ansi.F: C64Key.F,
        MacKey.Ansi.G: C64Key.G,
        MacKey.Ansi.H: C64Key.H,
        MacKey.Ansi.J: C64Key.J,
        MacKey.Ansi.K: C64Key.K,
        MacKey.Ansi.L: C64Key.L,
        MacKey.Ansi.semicolon: C64Key.semicolon,
        MacKey.Ansi.quote: C64Key.colon,
        MacKey.Ansi.backSlash: C64Key.equal,
        MacKey.ret: C64Key.ret,
        MacKey.F5: C64Key.F5F6,
        
        // Fourth row of C64 keyboard
        MacKey.option: C64Key.commodore,
        MacKey.shift: C64Key.shift,
        MacKey.rightShift: C64Key.rightShift,
        MacKey.Ansi.Z: C64Key.Z,
        MacKey.Ansi.X: C64Key.X,
        MacKey.Ansi.C: C64Key.C,
        MacKey.Ansi.V: C64Key.V,
        MacKey.Ansi.B: C64Key.B,
        MacKey.Ansi.N: C64Key.N,
        MacKey.Ansi.M: C64Key.M,
        MacKey.Ansi.comma: C64Key.comma,
        MacKey.Ansi.period: C64Key.period,
        MacKey.Ansi.slash: C64Key.slash,
        MacKey.curRight: C64Key.curLeftRight,
        // MacKey.curLeft: C64Key.curLeftRight,
        MacKey.curDown: C64Key.curUpDown,
        // MacKey.curUp: C64Key.curUpDown,
        MacKey.F7: C64Key.F7F8,

        // Fifth row of C64 keyboard
        MacKey.space: C64Key.space
    ]
    
    // Logical key mapping
    // Keys are mapped based on their meaning or the characters they represent
    func translate(macKey: MacKey) -> [C64Key] {
        
        switch macKey {
        
        // First row of C64 keyboard
        case MacKey.pageDown: return [C64Key.leftArrow]
        case MacKey.home: return [C64Key.home]
        case MacKey.clear: return [C64Key.home, C64Key.shift]
        case MacKey.delete: return [C64Key.delete]
            
        // Second row of C64 keyboard
        case MacKey.tab: return [C64Key.control]
        case MacKey.pageUp: return [C64Key.upArrow]
            
        // Third row of C64 keyboard
        case MacKey.control: return [C64Key.runStop]
        case MacKey.ret: return [C64Key.ret]

        // Fourth row of C64 keyboard
        case MacKey.option: return [C64Key.commodore]
        case MacKey.curLeft: return [C64Key.curLeftRight, C64Key.shift]
        case MacKey.curRight: return [C64Key.curLeftRight]
        case MacKey.curUp: return [C64Key.curUpDown, C64Key.shift]
        case MacKey.curDown: return [C64Key.curUpDown]
           
        // Fifth row of C64 keyboard
        case MacKey.space: return [C64Key.space]
            
        // Function keys
        case MacKey.F1: return [C64Key.F1F2]
        case MacKey.F2: return [C64Key.F1F2, C64Key.shift]
        case MacKey.F3: return [C64Key.F3F4]
        case MacKey.F4: return [C64Key.F3F4, C64Key.shift]
        case MacKey.F5: return [C64Key.F5F6]
        case MacKey.F6: return [C64Key.F5F6, C64Key.shift]
        case MacKey.F7: return [C64Key.F7F8]
        case MacKey.F8: return [C64Key.F7F8, C64Key.shift]
            
        default:
            
            // Translate symbolically
            return C64Key.translate(char: macKey.stringValue)
        }
    }
    
    func pressKey(key: C64Key, duration: TimeInterval? = nil) {
    
        // Press key
        keyboard?.pressKey(key.nr)

        // Schedule the key release
        if let seconds = duration {
            keyboard?.releaseKey(key.nr, delay: seconds)
        }
    }

    func toggleKey(key: C64Key) {

        keyboard?.toggleKey(key.nr)
    }

    func pressKeyCombination(key1: C64Key, key2: C64Key, duration: TimeInterval? = nil) {

        // Press keys
        keyboard?.pressKey(key1.nr)
        keyboard?.pressKey(key2.nr)

        // Schedule the key releases
        if let seconds = duration {
            keyboard?.releaseKey(key1.nr, delay: seconds)
            keyboard?.releaseKey(key2.nr, delay: seconds)
        }
    }

    func capsLockDown() {

        parent.config.warpMode = vc64.Warp.ALWAYS.rawValue
    }

    func capsLockUp() {

        parent.config.warpMode = vc64.Warp.NEVER.rawValue
    }

    func type(_ string: String) {

        keyboard?.autoType(string)
    }
}

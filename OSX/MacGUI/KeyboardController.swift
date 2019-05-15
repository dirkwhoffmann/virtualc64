//
// This file is part of VirtualC64 - A cycle accurate Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation
import Carbon.HIToolbox

//!@ brief Keyboard event handler
class KeyboardController: NSObject {
    
    var controller: MyController!
    
    /// Determines whether the joystick emulation keys should be uncoupled from
    // the keyboard.
    var disconnectJoyKeys = Defaults.disconnectJoyKeys
    
    /**
     Key mapping mode
     
     The user can choose between a symbolic and a positional key assignment.
     Symbolic assignment tries to assign the keys according to their meaning
     while positional assignment establishes a one-to-one mapping between Mac
     keys and C64 keys.
    */
    var mapKeysByPosition = Defaults.mapKeysByPosition
    
    /// Used key map if keys are mapped by position
 
    var keyMap: [MacKey: C64Key] = Defaults.keyMap
    
    // Delete when Objective-C code is gone
    func getDisconnectEmulationKeys() -> Bool { return disconnectJoyKeys }
    func setDisconnectEmulationKeys(_ b: Bool) { disconnectJoyKeys = b }
    func getMapKeysByPosition() -> Bool { return mapKeysByPosition }
    func setMapKeysByPosition(_ b: Bool) { mapKeysByPosition = b }

    /// Remembers the currently pressed key modifiers
    var leftShift: Bool = false
    var rightShift: Bool = false
    var control: Bool = false
    var option: Bool = false
    var command: Bool = false
    
    /**
     Remembers the currently pressed keys and their assigned C64 key list

     This variable is only used when keys are mapped symbolically. It's written
     in keyDown and picked up in keyUp.
     */
    var pressedKeys: [MacKey: [C64Key]] = [:]
    
    /**
     Checks if the internal values are consistent with the provides flags.
     
     There should never be an insonsistency. But if there is, we release the
     suspicous key. Otherwise, we risk to block the C64's keyboard matrix
     for good.
     */
    func checkConsistency(withEvent event: NSEvent) {
        
        let flags = event.modifierFlags
        
        if (leftShift || rightShift) != flags.contains(NSEvent.ModifierFlags.shift) {
            keyUp(with: MacKey.leftShift)
            keyUp(with: MacKey.rightShift)
            Swift.print("*** SHIFT inconsistency detected *** \(leftShift) \(rightShift)")
        }
        if control != flags.contains(NSEvent.ModifierFlags.control) {
            keyUp(with: MacKey.control)
            Swift.print("*** SHIFT inconsistency *** \(control)")
        }
        if option != flags.contains(NSEvent.ModifierFlags.option) {
            keyUp(with: MacKey.option)
            Swift.print("*** SHIFT inconsistency *** \(option)")
        }
    }
    
    init(controller c: MyController) {
        
        super.init()
        self.controller = c
    }
    
    func keyDown(with event: NSEvent) {
        
        // track()
        
        // Ignore repeating keys
        if event.isARepeat {
            return
        }
        
        // Exit fullscreen mode if escape key is pressed
        if event.keyCode == MacKey.escape.keyCode && controller.metalScreen.fullscreen {
            controller.window!.toggleFullScreen(nil)
        }
        
        let keyCode = event.keyCode
        let flags = event.modifierFlags
        let characters = event.charactersIgnoringModifiers
        
        // Ignore keys that are pressed in combination with the command key
        if flags.contains(NSEvent.ModifierFlags.command) {
            // track("Ignoring the command key")
            return
        }
        
        // Create and press MacKey
        let macKey = MacKey.init(keyCode: keyCode, characters: characters)
        checkConsistency(withEvent: event)
        keyDown(with: macKey)
    }
    
    func keyUp(with event: NSEvent) {

        let keyCode = event.keyCode
        let characters = event.charactersIgnoringModifiers

        // Create and release macKey
        let macKey = MacKey.init(keyCode: keyCode, characters: characters)
        checkConsistency(withEvent: event)
        keyUp(with: macKey)
    }
    
    func flagsChanged(with event: NSEvent) {
        
        // track("\(event)")
        let mod = event.modifierFlags
        let keyCode = event.keyCode
        
        if keyCode == kVK_Shift {
            if !leftShift {
                leftShift = true
                keyDown(with: MacKey.leftShift)
            } else {
                leftShift = false
                keyUp(with: MacKey.leftShift)
            }
        }
        if keyCode == kVK_RightShift {
            if !rightShift {
                rightShift = true
                keyDown(with: MacKey.rightShift)
            } else {
                rightShift = false
                keyUp(with: MacKey.rightShift)
            }
        }
        if mod.contains(.control) && !control {
            control = true
            keyDown(with: MacKey.control)
        }
        if !mod.contains(.control) && control {
            control = false
            keyUp(with: MacKey.control)
        }
        if mod.contains(.option) && !option {
            option = true
            keyDown(with: MacKey.option)
        }
        if !mod.contains(.option) && option {
            option = false
            keyUp(with: MacKey.option)
        }
        
        command = mod.contains(.command)
    }
    
    func keyDown(with macKey: MacKey) {
        
        // track("\(macKey)")
        
        // Check if this key is used for joystick emulation
        if controller.gamePadManager.keyDown(with: macKey) && disconnectJoyKeys {
            return
        }
        
        if mapKeysByPosition {
            keyDown(with: macKey, keyMap: keyMap)
            return
        }

        // Translate MacKey to a list of C64Keys
        let c64Keys = translate(macKey: macKey)
        
        if c64Keys != [] {

            // Store key combination for later use in keyUp
            pressedKeys[macKey] = c64Keys
        
            // Press all required keys
            for key in c64Keys {
                controller.c64.keyboard.pressKey(atRow: key.row, col: key.col)
            }
        }
    }
    
    func keyDown(with macKey: MacKey, keyMap: [MacKey: C64Key]) {
        
        if let key = keyMap[macKey] {
            controller.c64.keyboard.pressKey(atRow: key.row, col: key.col)
        }
    }
        
    func keyUp(with macKey: MacKey) {
        
        // Check if this key is used for joystick emulation
        if controller.gamePadManager.keyUp(with: macKey) && disconnectJoyKeys {
            return
        }
        
        if mapKeysByPosition {
            keyUp(with: macKey, keyMap: keyMap)
            return
        }
        
        // Lookup keys to be released
        if let c64Keys = pressedKeys[macKey] {
            for key in c64Keys {
                controller.c64.keyboard.releaseKey(atRow: key.row, col: key.col)
            }
        }
    }
    
    func keyUp(with macKey: MacKey, keyMap: [MacKey: C64Key]) {
        
        if let key = keyMap[macKey] {
            // track("Releasing row: \(key.row) col: \(key.col)\n")
            controller.c64.keyboard.releaseKey(atRow: key.row, col: key.col)
        }
    }
    
    /// Standard physical key map
    /// Keys are matched based on their position on the keyboard
    static let standardKeyMap: [MacKey: C64Key] = [
        
        // First row of C64 keyboard
        MacKey.Ansi.grave: C64Key.leftArrow,
        MacKey.Iso.hat: C64Key.leftArrow,
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
        MacKey.leftShift: C64Key.shift,
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
        MacKey.curLeft: C64Key.curLeftRight,
        MacKey.curDown: C64Key.curUpDown,
        MacKey.curUp: C64Key.curUpDown,
        MacKey.F7: C64Key.F7F8,

        // Fifth row of C64 keyboard
        MacKey.space: C64Key.space
    ]
    
    /// Logical key mapping
    /// Keys are mapped based on their meaning or the characters they represent
    func translate(macKey: MacKey) -> [C64Key] {
        
        switch macKey {
        
        // First row of C64 keyboard
        case MacKey.delete: return [C64Key.delete]

        // Second row of C64 keyboard
        case MacKey.tab: return [C64Key.control]

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
            return C64Key.translate(char: macKey.description)
        }
    }
    
    func _type(keyList: [C64Key]) {
        
        for key in keyList {
            if key == .restore {
                controller.c64.keyboard.pressRestoreKey()
            } else {
                controller.c64.keyboard.pressKey(atRow: key.row, col: key.col)
            }
        }
        usleep(useconds_t(50000))
        
        for key in keyList {
            if key == .restore {
                controller.c64.keyboard.releaseRestoreKey()
            } else {
            controller.c64.keyboard.releaseKey(atRow: key.row, col: key.col)
            }
        }
    }
    
    func _type(key: C64Key) {
        type(keyList: [key])
    }

    func type(keyList: [C64Key]) {
        
        DispatchQueue.global().async {
            self._type(keyList: keyList)
        }
    }
 
    func type(key: C64Key) {
        
        DispatchQueue.global().async {
            self._type(key: key)
        }
    }
    
    func type(string: String?,
              initialDelay: useconds_t = 0,
              completion: (() -> Void)? = nil) {

        if var truncated = string {
            
            // Shorten string if it is too large
            if truncated.count > 255 {
                truncated = truncated.prefix(256) + "..."
            }
            
            // Type string ...
            DispatchQueue.global().async {
                
                usleep(initialDelay)
                for c in truncated.lowercased() {
                    let c64Keys = C64Key.translate(char: String(c))
                    self._type(keyList: c64Keys)
                    usleep(useconds_t(20000))
                }
                completion?()
            }
        }
    }
    
    func type(_ string: String?, initialDelay seconds: Double = 0.0) {
        let uSeconds = useconds_t(1000000 * seconds)
        type(string: string, initialDelay: uSeconds)
    }
    
    func typeOnKeyboardAndPressPlay(string: String?) {
        type(string: string, completion: controller.c64.datasette.pressPlay)
    }
}

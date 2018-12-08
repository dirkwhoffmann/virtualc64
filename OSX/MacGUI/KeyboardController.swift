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
    
    var controller : MyController!
    
    /// Determines whether the joystick emulation keys should be uncoupled from
    // the keyboard.
    var disconnectEmulationKeys: Bool = true
    
    /**
     Key mapping mode
     
     The user can choose between a symbolic and a positional key assignment.
     Symbolic assignment tries to assign the keys according to their meaning
     while positional assignment establishes a one-to-one mapping between Mac
     keys and C64 keys.
    */
    var mapKeysByPosition = Defaults.mapKeysByPosition
    
    /// Used key map if keys are mapped by position
 
    var keyMap: [MacKey:C64Key] = KeyboardController.standardKeyMap
    
    // Delete when Objective-C code is gone
    func getDisconnectEmulationKeys() -> Bool { return disconnectEmulationKeys }
    func setDisconnectEmulationKeys(_ b: Bool) { disconnectEmulationKeys = b }
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
    var pressedKeys: [MacKey:[C64Key]] = [:]
    
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
        if (event.isARepeat) {
            return
        }
        
        // Exit fullscreen mode if escape key is pressed
        if (event.keyCode == MacKey.escape.keyCode && controller.metalScreen.fullscreen) {
            controller.window!.toggleFullScreen(nil)
        }
        
        let keyCode = event.keyCode
        let flags = event.modifierFlags
        let characters = event.charactersIgnoringModifiers
        
        // Ignore keys that are pressed in combination with the command key
        if (flags.contains(NSEvent.ModifierFlags.command)) {
            // track("Ignoring the command key")
            return
        }
        
        // Create and press MacKey
        let macKey = MacKey.init(keyCode: keyCode, characters: characters)
        checkConsistency(withEvent: event)
        keyDown(with: macKey)
    }
    
    func keyUp(with event: NSEvent)
    {
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
        if controller.gamePadManager.keyDown(with: macKey) && disconnectEmulationKeys {
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
    
    func keyDown(with macKey: MacKey, keyMap: [MacKey:C64Key]) {
        
        if let key = keyMap[macKey] {
            controller.c64.keyboard.pressKey(atRow: key.row, col: key.col)
        }
    }
        
    func keyUp(with macKey: MacKey) {
        
        // Check if this key is used for joystick emulation
        if controller.gamePadManager.keyUp(with: macKey) && disconnectEmulationKeys {
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
    
    func keyUp(with macKey: MacKey, keyMap: [MacKey:C64Key]) {
        
        if let key = keyMap[macKey] {
            // track("Releasing row: \(key.row) col: \(key.col)\n")
            controller.c64.keyboard.releaseKey(atRow: key.row, col: key.col)
        }
    }

    
    /// Standard physical key map
    /// Keys are matched based on their position on the keyboard
    static let standardKeyMap: [MacKey:C64Key] = [
        
        // First row of C64 keyboard
        MacKey.ansi.grave: C64Key.leftArrow,
        MacKey.iso.hat: C64Key.leftArrow,
        MacKey.ansi.digit0: C64Key.digit0,
        MacKey.ansi.digit1: C64Key.digit1,
        MacKey.ansi.digit2: C64Key.digit2,
        MacKey.ansi.digit3: C64Key.digit3,
        MacKey.ansi.digit4: C64Key.digit4,
        MacKey.ansi.digit5: C64Key.digit5,
        MacKey.ansi.digit6: C64Key.digit6,
        MacKey.ansi.digit7: C64Key.digit7,
        MacKey.ansi.digit8: C64Key.digit8,
        MacKey.ansi.digit9: C64Key.digit9,
        MacKey.ansi.minus: C64Key.minus,
        MacKey.ansi.equal: C64Key.plus,
        MacKey.delete: C64Key.delete,
        MacKey.F1 : C64Key.F1F2,
        
        // Second row of C64 keyboard
        MacKey.tab: C64Key.control,
        MacKey.ansi.Q: C64Key.Q,
        MacKey.ansi.W: C64Key.W,
        MacKey.ansi.E: C64Key.E,
        MacKey.ansi.R: C64Key.R,
        MacKey.ansi.T: C64Key.T,
        MacKey.ansi.Y: C64Key.Y,
        MacKey.ansi.U: C64Key.U,
        MacKey.ansi.I: C64Key.I,
        MacKey.ansi.O: C64Key.O,
        MacKey.ansi.P: C64Key.P,
        MacKey.ansi.leftBracket: C64Key.at,
        MacKey.ansi.rightBracket: C64Key.asterisk,
        MacKey.F3 : C64Key.F3F4,
        
        // Third row of C64 keyboard
        MacKey.control: C64Key.runStop,
        MacKey.ansi.A: C64Key.A,
        MacKey.ansi.S: C64Key.S,
        MacKey.ansi.D: C64Key.D,
        MacKey.ansi.F: C64Key.F,
        MacKey.ansi.G: C64Key.G,
        MacKey.ansi.H: C64Key.H,
        MacKey.ansi.J: C64Key.J,
        MacKey.ansi.K: C64Key.K,
        MacKey.ansi.L: C64Key.L,
        MacKey.ansi.semicolon: C64Key.semicolon,
        MacKey.ansi.quote: C64Key.colon,
        MacKey.ansi.backSlash: C64Key.equal,
        MacKey.ret: C64Key.ret,
        MacKey.F5 : C64Key.F5F6,
        
        // Fourth row of C64 keyboard
        MacKey.option: C64Key.commodore,
        MacKey.leftShift: C64Key.shift,
        MacKey.rightShift: C64Key.rightShift,
        MacKey.ansi.Z: C64Key.Z,
        MacKey.ansi.X: C64Key.X,
        MacKey.ansi.C: C64Key.C,
        MacKey.ansi.V: C64Key.V,
        MacKey.ansi.B: C64Key.B,
        MacKey.ansi.N: C64Key.N,
        MacKey.ansi.M: C64Key.M,
        MacKey.ansi.comma: C64Key.comma,
        MacKey.ansi.period: C64Key.period,
        MacKey.ansi.slash: C64Key.slash,
        MacKey.curRight : C64Key.curLeftRight,
        MacKey.curLeft : C64Key.curLeftRight,
        MacKey.curDown : C64Key.curUpDown,
        MacKey.curUp : C64Key.curUpDown,
        MacKey.F7 : C64Key.F7F8,

        // Fifth row of C64 keyboard
        MacKey.space : C64Key.space
    ]
    
    /// Logical key mapping
    /// Keys are mapped based on their meaning or the characters they represent
    func translate(macKey: MacKey) -> [C64Key] {
        
        switch (macKey) {
        
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
    
    func _typeOnKeyboard(keyList: [C64Key]) {
        
        for key in keyList {
            if (key == .restore) {
                controller.c64.keyboard.pressRestoreKey()
            } else {
                controller.c64.keyboard.pressKey(atRow: key.row, col: key.col)
            }
        }
        usleep(useconds_t(50000))
        
        for key in keyList {
            if (key == .restore) {
                controller.c64.keyboard.releaseRestoreKey()
            } else {
            controller.c64.keyboard.releaseKey(atRow: key.row, col: key.col)
            }
        }
    }
    
    func _typeOnKeyboard(key: C64Key) {
        typeOnKeyboard(keyList: [key])
    }

    func typeOnKeyboard(keyList: [C64Key]) {
        
        DispatchQueue.global().async {
            self._typeOnKeyboard(keyList: keyList)
        }
    }
 
    func typeOnKeyboard(key: C64Key) {
        
        DispatchQueue.global().async {
            self._typeOnKeyboard(key: key)
        }
    }
    
    func typeOnKeyboard(string: String,
                              initialDelay: useconds_t = 0,
                              completion: (() -> Void)?) {

        let truncated = (string.count < 256) ? string : string.prefix(256) + "..."

        DispatchQueue.global().async {
        
            usleep(initialDelay);
            for c in truncated.lowercased() {
                let c64Keys = C64Key.translate(char: String(c))
                self._typeOnKeyboard(keyList: c64Keys)
                usleep(useconds_t(20000))
            }
            completion?()
        }
    }
    
    func type(_ string: String) {
        typeOnKeyboard(string: string, completion: nil)
    }
        
    func typeOnKeyboardAndPressPlay(string: String) {
        typeOnKeyboard(string: string, completion: controller.c64.datasette.pressPlay)
    }
}

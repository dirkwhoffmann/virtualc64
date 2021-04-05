// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

// swiftlint:disable colon

import Carbon.HIToolbox

/* Mapping from Mac key codes to textual representations. This mapping covers
 * keys with an empty standard representation.
 */
let mac2string: [Int: String] = [
    
    kVK_ANSI_Keypad0:     "\u{2327}", // ⌧
    kVK_ANSI_KeypadEnter: "\u{2305}", // ⌅
    kVK_Return:           "\u{21A9}", // ↩
    kVK_Tab:              "\u{21E5}", // ⇥
    kVK_Space:            "\u{2423}", // ␣
    kVK_Delete:           "\u{232B}", // ⌫
    kVK_Escape:           "\u{238B}", // ⎋
    kVK_Command:          "\u{2318}", // ⌘
    kVK_RightCommand:     "\u{2318}", // ⌘
    kVK_Shift:            "\u{21E7}", // ⇧
    kVK_RightShift:       "\u{21e7}", // ⇧
    kVK_CapsLock:         "\u{21EA}", // ⇪
    kVK_Option:           "\u{2325}", // ⌥
    kVK_RightOption:      "\u{2325}", // ⌥
    kVK_Control:          "\u{2732}", // ✲
    kVK_RightControl:     "\u{2732}", // ✲
    
    kVK_F1:               "F1",
    kVK_F2:               "F2",
    kVK_F3:               "F3",
    kVK_F4:               "F4",
    kVK_F5:               "F5",
    kVK_F6:               "F6",
    kVK_F7:               "F7",
    kVK_F8:               "F8",
    kVK_F9:               "F9",
    kVK_F10:              "F10",
    kVK_F11:              "F11",
    kVK_F12:              "F12",
    kVK_F13:              "F13",
    kVK_F14:              "F14",
    kVK_F15:              "F15",
    kVK_F16:              "F16",
    kVK_F17:              "F17",
    kVK_F18:              "F18",
    kVK_F19:              "F19",
    kVK_F20:              "F20",
    kVK_Help:             "?\u{20DD}", // ?⃝
    kVK_Home:             "\u{2196}",  // ↖
    kVK_PageUp:           "\u{21DE}",  // ⇞
    kVK_PageDown:         "\u{21DF}",  // ⇟
    kVK_ForwardDelete:    "\u{2326}",  // ⌦
    kVK_End:              "\u{2198}",  // ↘
    kVK_LeftArrow:        "\u{2190}",  // ←
    kVK_RightArrow:       "\u{2192}",  // →
    kVK_UpArrow:          "\u{2191}",  // ↑
    kVK_DownArrow:        "\u{2193}"   // ↓
]

/* This structure represents a physical key of the Mac keyboard.
*/
struct MacKey: Codable {
    
    // The unique identifier of this Mac key
    var keyCode: Int = 0
    
    // Modifier flags at the time the key was pressed
    var carbonFlags: Int = 0
    
    init(keyCode: Int, flags: NSEvent.ModifierFlags = []) {
        
        self.keyCode = keyCode
        
        if flags.contains(.shift) { carbonFlags |= shiftKey }
        if flags.contains(.control) { carbonFlags |= controlKey }
        if flags.contains(.option) { carbonFlags |= optionKey }
        if flags.contains(.command) { carbonFlags |= cmdKey }
    }
    
    init(keyCode: UInt16, flags: NSEvent.ModifierFlags = []) {
        
        self.init(keyCode: Int(keyCode), flags: flags)
    }
    
    init(event: NSEvent) {
        
        self.init(keyCode: event.keyCode, flags: event.modifierFlags)
    }
    
    func hash(into hasher: inout Hasher) {
        
        hasher.combine(keyCode)
    }
    
    // Returns the modifier flags of this key
    var modifierFlags: NSEvent.ModifierFlags {
        
        var cocoaFlags: NSEvent.ModifierFlags = []
        
        if (carbonFlags & shiftKey)   != 0 { cocoaFlags.insert(.shift) }
        if (carbonFlags & controlKey) != 0 { cocoaFlags.insert(.control) }
        if (carbonFlags & optionKey)  != 0 { cocoaFlags.insert(.option) }
        if (carbonFlags & cmdKey)     != 0 { cocoaFlags.insert(.command) }
        
        return cocoaFlags
    }
    
    // Returns a string representation for this key
    var stringValue: String {
        
        // Check if this key has a custom representation
        if let s = mac2string[keyCode] {
            return s
        }
        
        // Return standard representation (keyboard dependent)
        if let s = String.init(keyCode: UInt16(keyCode), carbonFlags: carbonFlags) {
            return s
        }
        
        return ""
    }
}

extension MacKey: Equatable, Hashable {
    
    static func == (lhs: MacKey, rhs: MacKey) -> Bool {
        return lhs.keyCode == rhs.keyCode
    }
}

extension MacKey {
    
    // Layout independend keys. Keycodes are the same on all Mac keyboards
    static let escape       = MacKey.init(keyCode: kVK_Escape)
    static let shift        = MacKey.init(keyCode: kVK_Shift)
    static let rightShift   = MacKey.init(keyCode: kVK_RightShift)
    static let option       = MacKey.init(keyCode: kVK_Option)
    static let rightOption  = MacKey.init(keyCode: kVK_RightOption)
    static let control      = MacKey.init(keyCode: kVK_Control)
    static let rightControl = MacKey.init(keyCode: kVK_RightControl)
    static let command      = MacKey.init(keyCode: kVK_Command)
    static let rightCommand = MacKey.init(keyCode: kVK_RightCommand)

    static let ret          = MacKey.init(keyCode: kVK_Return)
    static let tab          = MacKey.init(keyCode: kVK_Tab)
    static let space        = MacKey.init(keyCode: kVK_Space)
    static let delete       = MacKey.init(keyCode: kVK_Delete)
    static let home         = MacKey.init(keyCode: kVK_Home)
    static let clear        = MacKey.init(keyCode: kVK_ANSI_KeypadClear)
    
    static let F1           = MacKey.init(keyCode: kVK_F1)
    static let F2           = MacKey.init(keyCode: kVK_F2)
    static let F3           = MacKey.init(keyCode: kVK_F3)
    static let F4           = MacKey.init(keyCode: kVK_F4)
    static let F5           = MacKey.init(keyCode: kVK_F5)
    static let F6           = MacKey.init(keyCode: kVK_F6)
    static let F7           = MacKey.init(keyCode: kVK_F7)
    static let F8           = MacKey.init(keyCode: kVK_F8)
    
    static let curLeft      = MacKey.init(keyCode: kVK_LeftArrow)
    static let curRight     = MacKey.init(keyCode: kVK_RightArrow)
    static let curDown      = MacKey.init(keyCode: kVK_DownArrow)
    static let curUp        = MacKey.init(keyCode: kVK_UpArrow)
    static let pageUp       = MacKey.init(keyCode: kVK_PageUp)
    static let pageDown     = MacKey.init(keyCode: kVK_PageDown)

    // Layout dependend keys. Keycodes refer to the keys on a standard ANSI US keyboard
    struct Ansi {
        static let grave    = MacKey.init(keyCode: kVK_ANSI_Grave)
        static let digit0   = MacKey.init(keyCode: kVK_ANSI_0)
        static let digit1   = MacKey.init(keyCode: kVK_ANSI_1)
        static let digit2   = MacKey.init(keyCode: kVK_ANSI_2)
        static let digit3   = MacKey.init(keyCode: kVK_ANSI_3)
        static let digit4   = MacKey.init(keyCode: kVK_ANSI_4)
        static let digit5   = MacKey.init(keyCode: kVK_ANSI_5)
        static let digit6   = MacKey.init(keyCode: kVK_ANSI_6)
        static let digit7   = MacKey.init(keyCode: kVK_ANSI_7)
        static let digit8   = MacKey.init(keyCode: kVK_ANSI_8)
        static let digit9   = MacKey.init(keyCode: kVK_ANSI_9)
        static let minus    = MacKey.init(keyCode: kVK_ANSI_Minus)
        static let equal    = MacKey.init(keyCode: kVK_ANSI_Equal)
        
        static let A        = MacKey.init(keyCode: kVK_ANSI_A)
        static let B        = MacKey.init(keyCode: kVK_ANSI_B)
        static let C        = MacKey.init(keyCode: kVK_ANSI_C)
        static let D        = MacKey.init(keyCode: kVK_ANSI_D)
        static let E        = MacKey.init(keyCode: kVK_ANSI_E)
        static let F        = MacKey.init(keyCode: kVK_ANSI_F)
        static let G        = MacKey.init(keyCode: kVK_ANSI_G)
        static let H        = MacKey.init(keyCode: kVK_ANSI_H)
        static let I        = MacKey.init(keyCode: kVK_ANSI_I)
        static let J        = MacKey.init(keyCode: kVK_ANSI_J)
        static let K        = MacKey.init(keyCode: kVK_ANSI_K)
        static let L        = MacKey.init(keyCode: kVK_ANSI_L)
        static let M        = MacKey.init(keyCode: kVK_ANSI_M)
        static let N        = MacKey.init(keyCode: kVK_ANSI_N)
        static let O        = MacKey.init(keyCode: kVK_ANSI_O)
        static let P        = MacKey.init(keyCode: kVK_ANSI_P)
        static let Q        = MacKey.init(keyCode: kVK_ANSI_Q)
        static let R        = MacKey.init(keyCode: kVK_ANSI_R)
        static let S        = MacKey.init(keyCode: kVK_ANSI_S)
        static let T        = MacKey.init(keyCode: kVK_ANSI_T)
        static let U        = MacKey.init(keyCode: kVK_ANSI_U)
        static let V        = MacKey.init(keyCode: kVK_ANSI_V)
        static let W        = MacKey.init(keyCode: kVK_ANSI_W)
        static let X        = MacKey.init(keyCode: kVK_ANSI_X)
        static let Y        = MacKey.init(keyCode: kVK_ANSI_Y)
        static let Z        = MacKey.init(keyCode: kVK_ANSI_Z)

        static let leftBracket  = MacKey.init(keyCode: kVK_ANSI_LeftBracket)
        static let rightBracket = MacKey.init(keyCode: kVK_ANSI_RightBracket)
        
        static let comma      = MacKey.init(keyCode: kVK_ANSI_Comma)
        static let period     = MacKey.init(keyCode: kVK_ANSI_Period)
        static let slash      = MacKey.init(keyCode: kVK_ANSI_Slash)
        static let backSlash  = MacKey.init(keyCode: kVK_ANSI_Backslash)
        static let semicolon  = MacKey.init(keyCode: kVK_ANSI_Semicolon)
        static let quote      = MacKey.init(keyCode: kVK_ANSI_Quote)
    }

    struct Iso {
        static let hat        = MacKey.init(keyCode: 0x0A)
    }

}

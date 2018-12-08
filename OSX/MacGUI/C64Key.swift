//
// This file is part of VirtualC64 - A cycle accurate Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

/// The C64Key structure represents a physical keys on the C64 keyboard.
/// Each of the 66 keys is specified uniquely by it's number ranging from
/// 0 to 64. When a key is pressed, a row bit and a column bit is set in the
/// keyboard matrix that can be read by the CIA chip. Note that the CapsLock
/// and the Restore key behave differently. Caps lock is a switch that holds
/// down the left shift key until it is released and restore has no key matrix
/// representation at all. Instead, it is directly connected to the NMI line.

struct C64Key : Codable {
    
    // A number that identifies this key uniquely
    var nr = -1
    
    // Row index
    var row = -1
    
    // Column index
    var col = -1
    
    // Flags for key modifiers
    let DEFAULT = 0
    let SHIFT = 1
    let COMMODORE = 2
    let LOWER = 4
    
    init(_ nr: Int) {
        
        precondition(nr >= 0 && nr <= 65)
        
        let rowcol = [
            // First physical key row
            (7,1), (7,0), (7,3), (1,0), (1,3), (2,0), (2,3), (3,0),
            (3,3), (4,0), (4,3), (5,0), (5,3), (6,0), (6,3), (0,0), (0,4) /* f1 */,
            
            // Second physical key row
            (7,2), (7,6), (1,1), (1,6), (2,1), (2,6), (3,1), (3,6),
            (4,1), (4,6), (5,1), (5,6), (6,1), (6,6), (9,9), (0,5) /* f3 */,

            // Third physical key row
            (7,7), (9,9), (1,2), (1,5), (2,2), (2,5), (3,2), (3,5),
            (4,2), (4,5), (5,2), (5,5), (6,2), (6,5), (0,1), (0,6) /* f5 */,
            
            // Fourth physical key row
            (7,5), (1,7), (1,4), (2,7), (2,4), (3,7), (3,4), (4,7),
            (4,4), (5,7), (5,4), (6,7), (6,4), (0,7), (0,2), (0,3) /* f7 */,
            
            // Fifth physical key row
            (7,4) /* space */
        ]
        
        precondition(rowcol.count == 66)
        
        self.nr = nr
        if (nr != 31 /* RESTORE */ && nr != 34 /* SHIFT LOCK */) {
            self.row = rowcol[nr].0
            self.col = rowcol[nr].1
        } else {
            precondition(rowcol[nr].0 == 9 && rowcol[nr].1 == 9)
        }
    }
    
    init(_ rowcol : (Int, Int) ) {
        
        precondition(rowcol.0 >= 0 && rowcol.0 < 8)
        precondition(rowcol.1 >= 0 && rowcol.1 < 8)
        
        let nr = [ 15, 47, 63, 64, 16, 32, 48, 62,
                    3, 19, 35,  4, 51, 36, 20, 50,
                    5, 21, 37,  6, 53, 38, 22, 52,
                    7, 23, 39,  8, 55, 40, 24, 54,
                    9, 25, 41, 10, 57, 42, 26, 56,
                   11, 27, 43, 12, 59, 44, 28, 58,
                   13, 29, 45, 14, 61, 46, 30, 60,
                    1,  0, 17,  2, 65, 49, 18, 33
        ]
        
        precondition(nr.count == 64)
        
        self.row = rowcol.0
        self.col = rowcol.1
        self.nr = nr[8 * row + col]
    }
}

extension C64Key: Equatable {
    static func ==(lhs: C64Key, rhs: C64Key) -> Bool {
        return lhs.nr == rhs.nr
    }
}

extension C64Key: Hashable {
    var hashValue: Int {
        return nr
    }
}

extension C64Key {
    
    // First row
    static let delete       = C64Key.init(15)
    static let ret          = C64Key.init(47)
    static let curLeftRight = C64Key.init(63)
    static let F7F8         = C64Key.init(64)
    static let F1F2         = C64Key.init(16)
    static let F3F4         = C64Key.init(32)
    static let F5F6         = C64Key.init(48)
    static let curUpDown    = C64Key.init(62)
    
    // Second row
    static let digit3       = C64Key.init(3)
    static let W            = C64Key.init(19)
    static let A            = C64Key.init(35)
    static let digit4       = C64Key.init(4)
    static let Z            = C64Key.init(51)
    static let S            = C64Key.init(36)
    static let E            = C64Key.init(20)
    static let shift        = C64Key.init(50)
    
    // Third row
    static let digit5       = C64Key.init(5)
    static let R            = C64Key.init(21)
    static let D            = C64Key.init(37)
    static let digit6       = C64Key.init(6)
    static let C            = C64Key.init(53)
    static let F            = C64Key.init(38)
    static let T            = C64Key.init(22)
    static let X            = C64Key.init(52)
    
    // Fourth row
    static let digit7       = C64Key.init(7)
    static let Y            = C64Key.init(23)
    static let G            = C64Key.init(39)
    static let digit8       = C64Key.init(8)
    static let B            = C64Key.init(55)
    static let H            = C64Key.init(40)
    static let U            = C64Key.init(24)
    static let V            = C64Key.init(54)
    
    // Fifth row
    static let digit9       = C64Key.init(9)
    static let I            = C64Key.init(25)
    static let J            = C64Key.init(41)
    static let digit0       = C64Key.init(10)
    static let M            = C64Key.init(57)
    static let K            = C64Key.init(42)
    static let O            = C64Key.init(26)
    static let N            = C64Key.init(56)
    
    // Sixth row
    static let plus         = C64Key.init(11)
    static let P            = C64Key.init(27)
    static let L            = C64Key.init(43)
    static let minus        = C64Key.init(12)
    static let period       = C64Key.init(59)
    static let colon        = C64Key.init(44)
    static let at           = C64Key.init(28)
    static let comma        = C64Key.init(58)
    
    // Seventh row
    static let pound        = C64Key.init(14)
    static let asterisk     = C64Key.init(29)
    static let semicolon    = C64Key.init(45)
    static let home         = C64Key.init(15)
    static let rightShift   = C64Key.init(61)
    static let equal        = C64Key.init(46)
    static let upArrow      = C64Key.init(30)
    static let slash        = C64Key.init(60)
    
    
    // Eights row
    static let digit1       = C64Key.init(1)
    static let leftArrow    = C64Key.init(0)
    static let control      = C64Key.init(17)
    static let digit2       = C64Key.init(2)
    static let space        = C64Key.init(65)
    static let commodore    = C64Key.init(49)
    static let Q            = C64Key.init(18)
    static let runStop      = C64Key.init(33)
    
    // Restore key
    static let restore      = C64Key.init(31)

    
    // Translates a character to a list of corresponding C64 keys
    // This function is used for symbolically mapping Mac keys to C64 keys
    static func translate(char: String?) -> [C64Key] {
        
        if char == nil { return [] }
        
        switch (char!) {
            
        // First row of C64 keyboard
        case "ü": return [C64Key.leftArrow]
        case "1": return [C64Key.digit1]
        case "!": return [C64Key.digit1, C64Key.shift]
        case "2": return [C64Key.digit2]
        case "\"": return [C64Key.digit2, C64Key.shift]
        case "3": return [C64Key.digit3]
        case "#": return [C64Key.digit3, C64Key.shift]
        case "4": return [C64Key.digit4]
        case "$": return [C64Key.digit4, C64Key.shift]
        case "5": return [C64Key.digit5]
        case "%": return [C64Key.digit5, C64Key.shift]
        case "6": return [C64Key.digit6]
        case "&": return [C64Key.digit6, C64Key.shift]
        case "7": return [C64Key.digit7]
        case "'": return [C64Key.digit7, C64Key.shift]
        case "8": return [C64Key.digit8]
        case "(": return [C64Key.digit8, C64Key.shift]
        case "9": return [C64Key.digit9]
        case ")": return [C64Key.digit9, C64Key.shift]
        case "0": return [C64Key.digit0]
        case "+": return [C64Key.plus]
        case "-": return [C64Key.minus]
        case "§": return [C64Key.pound]
            
        // Second row of C64 keyboard
        case "q": return [C64Key.Q]
        case "Q": return [C64Key.Q, C64Key.shift]
        case "w": return [C64Key.W]
        case "W": return [C64Key.W, C64Key.shift]
        case "e": return [C64Key.E]
        case "E": return [C64Key.E, C64Key.shift]
        case "r": return [C64Key.R]
        case "R": return [C64Key.R, C64Key.shift]
        case "t": return [C64Key.T]
        case "T": return [C64Key.T, C64Key.shift]
        case "y": return [C64Key.Y]
        case "Y": return [C64Key.Y, C64Key.shift]
        case "u": return [C64Key.U]
        case "U": return [C64Key.U, C64Key.shift]
        case "i": return [C64Key.I]
        case "I": return [C64Key.I, C64Key.shift]
        case "o": return [C64Key.O]
        case "O": return [C64Key.O, C64Key.shift]
        case "p": return [C64Key.P]
        case "P": return [C64Key.P, C64Key.shift]
        case "@": return [C64Key.at]
        case "ö": return [C64Key.at]
        case "*": return [C64Key.asterisk]
        case "ä": return [C64Key.upArrow]
            
        // Third row of C64 keyboard
        case "a": return [C64Key.A]
        case "A": return [C64Key.A, C64Key.shift]
        case "s": return [C64Key.S]
        case "S": return [C64Key.S, C64Key.shift]
        case "d": return [C64Key.D]
        case "D": return [C64Key.D, C64Key.shift]
        case "f": return [C64Key.F]
        case "F": return [C64Key.F, C64Key.shift]
        case "g": return [C64Key.G]
        case "G": return [C64Key.G, C64Key.shift]
        case "h": return [C64Key.H]
        case "H": return [C64Key.H, C64Key.shift]
        case "j": return [C64Key.J]
        case "J": return [C64Key.J, C64Key.shift]
        case "k": return [C64Key.K]
        case "K": return [C64Key.K, C64Key.shift]
        case "l": return [C64Key.L]
        case "L": return [C64Key.L, C64Key.shift]
        case ":": return [C64Key.colon]
        case "[": return [C64Key.colon, C64Key.shift]
        case ";": return [C64Key.semicolon]
        case "]": return [C64Key.semicolon, C64Key.shift]
        case "=": return [C64Key.equal]
        case "\n": return [C64Key.ret]

        // Fourth row of C64 keyboard
        case "z": return [C64Key.Z]
        case "Z": return [C64Key.Z, C64Key.shift]
        case "x": return [C64Key.X]
        case "X": return [C64Key.X, C64Key.shift]
        case "c": return [C64Key.C]
        case "C": return [C64Key.C, C64Key.shift]
        case "v": return [C64Key.V]
        case "V": return [C64Key.V, C64Key.shift]
        case "b": return [C64Key.B]
        case "B": return [C64Key.B, C64Key.shift]
        case "n": return [C64Key.N]
        case "N": return [C64Key.N, C64Key.shift]
        case "m": return [C64Key.M]
        case "M": return [C64Key.M, C64Key.shift]
        case ",": return [C64Key.comma]
        case "<": return [C64Key.comma, C64Key.shift]
        case ".": return [C64Key.period]
        case ">": return [C64Key.period, C64Key.shift]
        case "/": return [C64Key.slash]
        case "?": return [C64Key.slash, C64Key.shift]
           
        // Fifth row of C64 keyboard
        case " ": return [C64Key.space]
            
        default: return []
        }
    }
}

//
// Image processing
//

extension NSImage.Name {
    static let key_clr = NSImage.Name(rawValue: "key_clr")
    static let key_home = NSImage.Name(rawValue: "key_home")
    static let key_inst = NSImage.Name(rawValue: "key_inst")
    static let key_del = NSImage.Name(rawValue: "key_del")
    static let key_crsr_up = NSImage.Name(rawValue: "key_crsr_up")
    static let key_crsr_down = NSImage.Name(rawValue: "key_crsr_down")
    static let key_crsr_left = NSImage.Name(rawValue: "key_crsr_left")
    static let key_crsr_right = NSImage.Name(rawValue: "key_crsr_right")
    static let key_ctrl = NSImage.Name(rawValue: "key_ctrl")
    static let key_ctrl_pressed = NSImage.Name(rawValue: "key_ctrl_pressed")
    static let key_restore = NSImage.Name(rawValue: "key_restore")
    static let key_runstop = NSImage.Name(rawValue: "key_runstop")
    static let key_shiftlock = NSImage.Name(rawValue: "key_shiftlock")
    static let key_shiftlock_pressed = NSImage.Name(rawValue: "key_shiftlock_pressed")
    static let key_return = NSImage.Name(rawValue: "key_return")
    static let key_commodore = NSImage.Name(rawValue: "key_commodore")
    static let key_commodore_pressed = NSImage.Name(rawValue: "key_commodore_pressed")
    static let key_shift = NSImage.Name(rawValue: "key_shift")
    static let key_shift_pressed = NSImage.Name(rawValue: "key_shift_pressed")
}

extension C64Key {

    /// Return the physical layout parameters (width and height) for this key
    var layout: (Int,Int) {
        get {
            switch nr {
            case 47:
                return (64,32) // Return key
            case 17, 31, 50, 61:
                return (48,32) // Ctrl, Restore, Left Shift, Right Shift
            case 16, 32, 48, 64:
                return (48,32) // F1, F3, F5, F7
            default:
                return (32,32) // All other keys
            }
        }
    }
    
    /// Returns an empty background key image
    var background: NSImage {
        get {
            let imageName = (nr == 16 || nr == 32 || nr == 48 || nr == 64) ? "key_dark" : "key"
            let background = NSImage(named: NSImage.Name(rawValue: imageName))!
            return background.resizeImage(width: CGFloat(layout.0), height: CGFloat(layout.1))
        }
    }
    
    func plainKeyImage(width: Int, height: Int, dark: Bool = false) -> NSImage {

        let name = NSImage.Name(rawValue: dark ? "key_dark" : "key")
        let background = NSImage(named: name)!
        return background.resizeImage(width: CGFloat(width), height: CGFloat(height))
    }
    
    /// Returns an image representation for this key that is used in the
    /// virtual keyboard.
    func image(pressed: Bool = false,
               shift: Bool = false,
               control: Bool = false,
               commodore: Bool = false,
               lowercase: Bool = false) -> NSImage {
        
        // Create mapping table if neccessary
        if C64Key.map.isEmpty { initMap() }
        
        // Check for keys with a predrawn image
        switch (nr) {
        case 17: return NSImage.init(named: pressed ? .key_ctrl_pressed : .key_ctrl)!
        case 31: return NSImage.init(named: .key_restore)!
        case 33: return NSImage.init(named: .key_runstop)!
        case 34: return NSImage.init(named: pressed ? .key_shiftlock_pressed : .key_shiftlock)!
        case 47: return NSImage.init(named: .key_return)!
        case 49: return NSImage.init(named: pressed ? .key_commodore_pressed : .key_commodore)!
        case 50: return NSImage.init(named: pressed ? .key_shift_pressed : .key_shift)!
        case 61: return NSImage.init(named: pressed ? .key_shift_pressed : .key_shift)!
        case 14: return NSImage.init(named: shift ? .key_clr : .key_home)!
        case 15: return NSImage.init(named: shift ? .key_inst : .key_del)!
        case 62: return NSImage.init(named: shift ? .key_crsr_up : .key_crsr_down)!
        case 63: return NSImage.init(named: shift ? .key_crsr_left : .key_crsr_right)!
        default: break
        }
    
        // Check for keys with a color label
        var color: NSColor?
        if (nr >= 1 && nr <= 8) && (control || commodore) {
            let rgb = [(0x00, 0x00, 0x00), (0xff, 0xff, 0xff),
                       (0x91, 0x4a, 0x40), (0x86, 0xc5, 0xcc),
                       (0x93, 0x4e, 0xb6), (0x73, 0xb2, 0x4b),
                       (0x4a, 0x35, 0xaa), (0xd4, 0xe0, 0x7c),
                       (0x98, 0x6a, 0x2d), (0x66, 0x53, 0x00),
                       (0xc0, 0x81, 0x78), (0x60, 0x60, 0x60),
                       (0x8a, 0x8a, 0x8a), (0xb4, 0xed, 0x91),
                       (0x87, 0x77, 0xde), (0xb3, 0xb3, 0xb3)]
            let index = (nr - 1) + (commodore ? 8 : 0)
            let r = CGFloat(rgb[index].0) / 255.0
            let g = CGFloat(rgb[index].1) / 255.0
            let b = CGFloat(rgb[index].2) / 255.0
            color = NSColor.init(red: r, green: g, blue: b, alpha: 1.0)
            
            // Draw colored circle ...
            let image = background
            image.lockFocus()
            color?.setFill()
            let border = CGFloat(3.0)
            let width = image.size.width - (2.0 * border)
            let height = image.size.height - (2.0 * border)
            let rect = NSMakeRect(border, border, width, height)
            let cPath = NSBezierPath.init(roundedRect: rect, xRadius: 3.0, yRadius: 3.0)
            cPath.fill()
            image.unlockFocus()
            return image
        }
 
        // All other keys (textual label)
        var font: NSFont
        var yoffset: Int
        
        var index = 8 * nr
        index += (shift ? SHIFT : 0)
        index += (commodore ? COMMODORE : 0)
        index += (lowercase ? LOWER : 0)
        let (label, fontname) = C64Key.map[index]!
        if fontname == "" {
            font = NSFont.systemFont(ofSize: 13)
            yoffset = -6
        } else {
            font = NSFont.init(name: "C64ProMono", size: 9)!
            yoffset = -9
        }
        
        // Render key
        let textRect = CGRect(x: 0, y: yoffset, width: layout.0, height: layout.1)
        let paragraphStyle = NSMutableParagraphStyle()
        paragraphStyle.alignment = .center
        let textFontAttributes1 = [
            NSAttributedStringKey.font: font,
            NSAttributedStringKey.foregroundColor: NSColor.black,
            NSAttributedStringKey.paragraphStyle: paragraphStyle,
        ]
        let image = background
        image.lockFocus()
        label.draw(in: textRect, withAttributes: textFontAttributes1)
        image.unlockFocus()
        return image
    }
    
    /// Returns an image representation for this key that is used in the
    /// user dialog for configuring the key mapping.
    func image(keyCode: NSString) -> NSImage {
        
        precondition(nr != 31 /* RESTORE */);
        precondition(nr != 34 /* SHIFT LOCK */);
        precondition(row >= 0 && row < 8)
        precondition(col >= 0 && col < 8)
        
        // Get textual description for this key
        let curUD = "CU \u{21c5}"
        let curLR = "CU \u{21c6}"
        let shftL = "\u{21e7}"
        let shftR = "      \u{21e7}"
        let pound = "\u{00a3}"
        let lfArr = "\u{2190}"
        let upArr = "\u{2191}"
        let space = "\u{23b5}"
        let retrn = "\u{21b5}"
        var name = [
            ["DEL", retrn, curLR,  "F7",   "F1",  "F3", "F5",  curUD],
            ["3",   "W",   "A",    "4",    "Z",   "S",  "E",   shftL],
            ["5",   "R",   "D",    "6",    "C",   "F",  "T",   "X"],
            ["7",   "Y",   "G",    "8",    "B",   "H",  "U",   "V"],
            ["9",   "I",   "J",    "0",    "M",   "K",  "O",   "N"],
            ["+",   "P",   "L",    "-",    ".",   ":",  "@",   ","],
            [pound, "*",   ";",    "HOME", shftR, "=",  upArr, "/"],
            ["1",   lfArr, "CTRL", "2",    space, "C=", "Q",   "STOP"]
        ]
        
        let description = name[row][col]
        
        // Compute image
        let width = 48.0
        let height = 48.0

        let image = plainKeyImage(width: Int(width), height: Int(height))
        let imageRect = CGRect(x: 0, y: 0, width: width, height: height)
        let textRect1 = CGRect(x: 7, y: -2, width: width-7, height: height-2)
        let textRect2 = CGRect(x: 14, y: -10, width: width-14, height: height-10)
        let textStyle = NSMutableParagraphStyle.default.mutableCopy() as! NSMutableParagraphStyle
        let font1 = NSFont.systemFont(ofSize: 12)
        let font2 = NSFont.systemFont(ofSize: 16)
        let textFontAttributes1 = [
            NSAttributedStringKey.font: font1,
            NSAttributedStringKey.foregroundColor: NSColor.gray,
            NSAttributedStringKey.paragraphStyle: textStyle
        ]
        let textFontAttributes2 = [
            NSAttributedStringKey.font: font2,
            NSAttributedStringKey.foregroundColor: NSColor.black,
            NSAttributedStringKey.paragraphStyle: textStyle
        ]
        
        image.lockFocus()
        image.draw(in: imageRect)
        description.draw(in: textRect1, withAttributes: textFontAttributes1)
        keyCode.draw(in: textRect2, withAttributes: textFontAttributes2)
        image.unlockFocus()
        
        return image
    }
}

extension C64Key {

    static var map: [Int: (String,String)] = [:]
    
    func assign(_ key: Int, _ flags: Int, _ label: String, _ font: String = "") {
        
        for i in 0 ... 7 {
            if i & flags == flags {
                C64Key.map[8 * key + i] = (label, font)
            }
        }
    }
    
    func initMap() {
        
        //
        // Function keys
        //
        
        assign(16, DEFAULT, "f 1")
        assign(16, SHIFT, "f 2")
        assign(32, DEFAULT, "f 3")
        assign(32, SHIFT, "f 4")
        assign(48, DEFAULT, "f 5")
        assign(48, SHIFT, "f 6")
        assign(64, DEFAULT, "f 7")
        assign(64, SHIFT, "f 8")
        
        //
        // First row
        //
        
        assign(0, DEFAULT, "\u{2190}" /* left arrow */)
        
        assign(1, DEFAULT, "1")
        assign(1, SHIFT, "!")
        assign(1, COMMODORE, "")

        assign(2, DEFAULT, "2")
        assign(2, SHIFT, "\"")
        assign(2, COMMODORE, "")
        
        assign(3, DEFAULT, "3")
        assign(3, SHIFT, "#")
        assign(3, COMMODORE, "")

        assign(4, DEFAULT, "4")
        assign(4, SHIFT, "$")
        assign(4, COMMODORE, "")

        assign(5, DEFAULT, "5")
        assign(5, SHIFT, "%")
        assign(5, COMMODORE, "")

        assign(6, DEFAULT, "6")
        assign(6, SHIFT, "&")
        assign(6, COMMODORE, "")

        assign(7, DEFAULT, "7")
        assign(7, SHIFT, "'")
        assign(7, COMMODORE, "")

        assign(8, DEFAULT, "8")
        assign(8, SHIFT, "(")
        assign(8, COMMODORE, "")

        assign(9, DEFAULT, "9")
        assign(9, SHIFT, ")")
        assign(9, COMMODORE, ")")

        assign(10, DEFAULT, "0")
        
        assign(11, DEFAULT, "+")
        assign(11, SHIFT, "\u{253c}", "C64")
        assign(11, COMMODORE, "\u{2592}", "C64")

        assign(12, DEFAULT, "-")
        assign(12, SHIFT, "\u{2502}", "C64")
        assign(12, COMMODORE, "\u{e0dc}", "C64")

        assign(13, DEFAULT, "\u{00a3}" /* Pound */)
        assign(13, SHIFT, "\u{25e4}", "C64")
        assign(13, LOWER | SHIFT, "\u{e1e9}", "C64")
        assign(13, COMMODORE, "\u{e0a8}", "C64")

        //
        // Second row
        //
        
        assign(18, DEFAULT, "Q")
        assign(18, SHIFT, "\u{2022}", "C64")
        assign(18, LOWER, "q")
        assign(18, LOWER | SHIFT, "Q")
        assign(18, COMMODORE, "\u{251c}", "C64")

        assign(19, DEFAULT, "W")
        assign(19, SHIFT, "\u{25cb}", "C64")
        assign(19, LOWER, "w")
        assign(19, LOWER | SHIFT, "W")
        assign(19, COMMODORE, "\u{2524}", "C64")

        assign(20, DEFAULT, "E")
        assign(20, SHIFT, "\u{e0c5}", "C64")
        assign(20, LOWER, "e")
        assign(20, LOWER | SHIFT, "E")
        assign(20, COMMODORE, "\u{2534}", "C64")

        assign(21, DEFAULT, "R")
        assign(21, SHIFT, "\u{e072}", "C64")
        assign(21, LOWER, "r")
        assign(21, LOWER | SHIFT, "R")
        assign(21, COMMODORE, "\u{252c}", "C64")

        assign(22, DEFAULT, "T")
        assign(22, SHIFT, "\u{e0d4}", "C64")
        assign(22, LOWER, "t")
        assign(22, LOWER | SHIFT, "T")
        assign(22, COMMODORE, "\u{2594}", "C64")

        assign(23, DEFAULT, "Y")
        assign(23, SHIFT, "\u{e0d9}", "C64")
        assign(23, LOWER, "y")
        assign(23, LOWER | SHIFT, "Y")
        assign(23, COMMODORE, "\u{e0b7}", "C64")

        assign(24, DEFAULT, "U")
        assign(24, SHIFT, "\u{256d}", "C64")
        assign(24, LOWER, "u")
        assign(24, LOWER | SHIFT, "U")
        assign(24, COMMODORE, "\u{e0b8}", "C64")

        assign(25, DEFAULT, "I")
        assign(25, SHIFT, "\u{256e}", "C64")
        assign(25, LOWER, "i")
        assign(25, LOWER | SHIFT, "I")
        assign(25, COMMODORE, "\u{2584}", "C64")

        assign(26, DEFAULT, "O")
        assign(26, SHIFT, "\u{e0cf}", "C64")
        assign(26, LOWER, "o")
        assign(26, LOWER | SHIFT, "O")
        assign(26, COMMODORE, "\u{2583}", "C64")

        assign(27, DEFAULT, "P")
        assign(27, SHIFT, "\u{e0d0}", "C64")
        assign(27, LOWER, "p")
        assign(27, LOWER | SHIFT, "P")
        assign(27, COMMODORE, "\u{2582}", "C64")

        assign(28, DEFAULT, "@")
        assign(28, SHIFT, "\u{e0ba}", "C64")
        assign(28, LOWER | SHIFT, "\u{e1fa}", "C64")
        assign(28, COMMODORE, "\u{2581}", "C64")

        assign(29, DEFAULT, "*")
        assign(29, SHIFT, "\u{2500}", "C64")
        assign(29, COMMODORE, "\u{25e5}", "C64")
        assign(29, LOWER | COMMODORE, "\u{e17f}", "C64")

        assign(30, DEFAULT, "\u{2191}")
        assign(30, SHIFT, "\u{03C0}")
        assign(30, COMMODORE, "\u{03C0}")
        assign(30, LOWER, "\u{e1de}", "C64")

        //
        // Third row
        //
        
        assign(35, DEFAULT, "A")
        assign(35, SHIFT, "\u{2660}", "C64")
        assign(35, LOWER, "a")
        assign(35, LOWER | SHIFT, "A")
        assign(35, COMMODORE, "\u{250c}", "C64")

        assign(36, DEFAULT, "S")
        assign(36, SHIFT, "\u{2665}", "C64")
        assign(36, LOWER, "s")
        assign(36, LOWER | SHIFT, "S")
        assign(36, COMMODORE, "\u{2510}", "C64")

        assign(37, DEFAULT, "D")
        assign(37, SHIFT, "\u{e064}", "C64")
        assign(37, LOWER, "d")
        assign(37, LOWER | SHIFT, "D")
        assign(37, COMMODORE, "\u{2597}", "C64")

        assign(38, DEFAULT, "F")
        assign(38, SHIFT, "\u{e0c6}", "C64")
        assign(38, LOWER, "f")
        assign(38, LOWER | SHIFT, "F")
        assign(38, COMMODORE, "\u{2596}", "C64")

        assign(39, DEFAULT, "G")
        assign(39, SHIFT, "\u{e0c7}", "C64")
        assign(39, LOWER, "g")
        assign(39, LOWER | SHIFT, "G")
        assign(39, COMMODORE, "\u{258e}", "C64")

        assign(40, DEFAULT, "H")
        assign(40, SHIFT, "\u{e0c8}", "C64")
        assign(40, LOWER, "h")
        assign(40, LOWER | SHIFT, "H")
        assign(40, COMMODORE, "\u{258e}", "C64")

        assign(41, DEFAULT, "J")
        assign(41, SHIFT, "\u{2570}", "C64")
        assign(41, LOWER, "j")
        assign(41, LOWER | SHIFT, "J")
        assign(41, COMMODORE, "\u{258d}", "C64")

        assign(42, DEFAULT, "K")
        assign(42, SHIFT, "\u{256f}", "C64")
        assign(42, LOWER, "k")
        assign(42, LOWER | SHIFT, "K")
        assign(42, COMMODORE, "\u{258c}", "C64")

        assign(43, DEFAULT, "L")
        assign(43, SHIFT, "\u{e0cc}", "C64")
        assign(43, LOWER, "l")
        assign(43, LOWER | SHIFT, "L")
        assign(43, COMMODORE, "\u{e0b6}", "C64")

        assign(44, DEFAULT, ":")
        assign(44, SHIFT, "[")
        assign(44, COMMODORE, "[")

        assign(45, DEFAULT, ";")
        assign(45, SHIFT, "]")
        assign(45, COMMODORE, "]")

        assign(46, DEFAULT, "=")
    
        //
        // Fourth row
        //
        
        assign(51, DEFAULT, "Z")
        assign(51, SHIFT, "\u{2666}", "C64")
        assign(51, LOWER, "z")
        assign(51, LOWER | SHIFT, "Z")
        assign(51, COMMODORE, "\u{2514}", "C64")

        assign(52, DEFAULT, "X")
        assign(52, SHIFT, "\u{2663}", "C64")
        assign(52, LOWER, "x")
        assign(52, LOWER | SHIFT, "X")
        assign(52, COMMODORE, "\u{2518}", "C64")

        assign(53, DEFAULT, "C")
        assign(53, SHIFT, "\u{2500}", "C64")
        assign(53, LOWER, "c")
        assign(53, LOWER | SHIFT, "C")
        assign(53, COMMODORE, "\u{259d}", "C64")

        assign(54, DEFAULT, "V")
        assign(54, SHIFT, "\u{2573}", "C64")
        assign(54, LOWER, "v")
        assign(54, LOWER | SHIFT, "V")
        assign(54, COMMODORE, "\u{2598}", "C64")

        assign(55, DEFAULT, "B")
        assign(55, SHIFT, "\u{2502}", "C64")
        assign(55, LOWER, "b")
        assign(55, LOWER | SHIFT, "B")
        assign(55, COMMODORE, "\u{259a}", "C64")

        assign(56, DEFAULT, "N")
        assign(56, SHIFT, "\u{2571}", "C64")
        assign(56, LOWER, "n")
        assign(56, LOWER | SHIFT, "N")
        assign(56, COMMODORE, "\u{e0aa}", "C64")

        assign(57, DEFAULT, "M")
        assign(57, SHIFT, "\u{2572}", "C64")
        assign(57, LOWER, "m")
        assign(57, LOWER | SHIFT, "M")
        assign(57, COMMODORE, "\u{e0a7}", "C64")

        assign(58, DEFAULT, ",")
        assign(58, SHIFT, "<")
        assign(58, COMMODORE, "<")

        assign(59, DEFAULT, ".")
        assign(59, SHIFT, ">")
        assign(59, COMMODORE, ">")

        assign(60, DEFAULT, "/")
        assign(60, SHIFT, "?")
        assign(60, COMMODORE, "?")

        //
        // Fifth row
        //
        
        assign(65, DEFAULT, "")
    }
}

// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

// swiftlint:disable comma

struct Modifier: OptionSet, Hashable {
    
    let rawValue: Int
    
    static let shift       = Modifier(rawValue: 1 << 0)
    static let commodore   = Modifier(rawValue: 1 << 1)
    static let control     = Modifier(rawValue: 1 << 2)
    static let lowercase   = Modifier(rawValue: 1 << 3)
}

func == (lhs: Modifier, rhs: Modifier) -> Bool {
    return lhs.hashValue == rhs.hashValue
}

/* The C64Key structure represents a physical keys on the C64 keyboard. Each of
 * the 66 keys is specified uniquely by it's number ranging from 0 to 65. When
 * a key is pressed, a row bit and a column bit is set in the keyboard matrix
 * that can be read by the CIA chip. Note that the CapsLock and the Restore key
 * behave differently. Caps lock is a switch that holds down the left shift key
 * until it is released and restore has no key matrix representation. This key
 * is connected to the NMI line.
 */
struct C64Key: Codable {
    
    // Unique key identifier (0 .. 65)
    var nr = -1
    
    // Row and column indices
    var row = -1
    var col = -1
    
    init(_ nr: Int) {
        
        precondition(nr >= 0 && nr <= 65)
        
        let rowcol = [
            // First physical row
            (7, 1), (7, 0), (7, 3), (1, 0), (1, 3), (2, 0), (2, 3), (3, 0),
            (3, 3), (4, 0), (4, 3), (5, 0), (5, 3), (6, 0), (6, 3), (0, 0),
            (0, 4),
            
            // Second physical row
            (7, 2), (7, 6), (1, 1), (1, 6), (2, 1), (2, 6), (3, 1), (3, 6),
            (4, 1), (4, 6), (5, 1), (5, 6), (6, 1), (6, 6), (9, 9), (0, 5),
            
            // Third physical row
            (7, 7), (9, 9), (1, 2), (1, 5), (2, 2), (2, 5), (3, 2), (3, 5),
            (4, 2), (4, 5), (5, 2), (5, 5), (6, 2), (6, 5), (0, 1), (0, 6),
            
            // Fourth physical row
            (7, 5), (1, 7), (1, 4), (2, 7), (2, 4), (3, 7), (3, 4), (4, 7),
            (4, 4), (5, 7), (5, 4), (6, 7), (6, 4), (0, 7), (0, 2), (0, 3),
            
            // Fifth physical row
            (7, 4)
        ]
        
        assert(rowcol.count == 66)
        
        self.nr = nr
        if nr != 31 /* RESTORE */ && nr != 34 /* SHIFT LOCK */ {
            self.row = rowcol[nr].0
            self.col = rowcol[nr].1
        } else {
            assert(rowcol[nr].0 == 9 && rowcol[nr].1 == 9)
        }
    }
    
    init(_ rowcol: (Int, Int) ) {
        
        assert(rowcol.0 >= 0 && rowcol.0 < 8)
        assert(rowcol.1 >= 0 && rowcol.1 < 8)
        
        let nr = [ 15, 47, 63, 64, 16, 32, 48, 62,
                   3, 19, 35, 4, 51, 36, 20, 50,
                   5, 21, 37, 6, 53, 38, 22, 52,
                   7, 23, 39, 8, 55, 40, 24, 54,
                   9, 25, 41, 10, 57, 42, 26, 56,
                   11, 27, 43, 12, 59, 44, 28, 58,
                   13, 29, 45, 14, 61, 46, 30, 60,
                   1, 0, 17, 2, 65, 49, 18, 33
        ]
        
        assert(nr.count == 64)
        
        self.row = rowcol.0
        self.col = rowcol.1
        self.nr = nr[8 * row + col]
    }
    
    // First row in key matrix
    static let delete       = C64Key(15)
    static let ret          = C64Key(47)
    static let curLeftRight = C64Key(63)
    static let F7F8         = C64Key(64)
    static let F1F2         = C64Key(16)
    static let F3F4         = C64Key(32)
    static let F5F6         = C64Key(48)
    static let curUpDown    = C64Key(62)
    
    // Second row in key matrix
    static let digit3       = C64Key(3)
    static let W            = C64Key(19)
    static let A            = C64Key(35)
    static let digit4       = C64Key(4)
    static let Z            = C64Key(51)
    static let S            = C64Key(36)
    static let E            = C64Key(20)
    static let shift        = C64Key(50)
    
    // Third row in key matrix
    static let digit5       = C64Key(5)
    static let R            = C64Key(21)
    static let D            = C64Key(37)
    static let digit6       = C64Key(6)
    static let C            = C64Key(53)
    static let F            = C64Key(38)
    static let T            = C64Key(22)
    static let X            = C64Key(52)
    
    // Fourth row in key matrix
    static let digit7       = C64Key(7)
    static let Y            = C64Key(23)
    static let G            = C64Key(39)
    static let digit8       = C64Key(8)
    static let B            = C64Key(55)
    static let H            = C64Key(40)
    static let U            = C64Key(24)
    static let V            = C64Key(54)
    
    // Fifth row in key matrix
    static let digit9       = C64Key(9)
    static let I            = C64Key(25)
    static let J            = C64Key(41)
    static let digit0       = C64Key(10)
    static let M            = C64Key(57)
    static let K            = C64Key(42)
    static let O            = C64Key(26)
    static let N            = C64Key(56)
    
    // Sixth row in key matrix
    static let plus         = C64Key(11)
    static let P            = C64Key(27)
    static let L            = C64Key(43)
    static let minus        = C64Key(12)
    static let period       = C64Key(59)
    static let colon        = C64Key(44)
    static let at           = C64Key(28)
    static let comma        = C64Key(58)
    
    // Seventh row in key matrix
    static let pound        = C64Key(13)
    static let asterisk     = C64Key(29)
    static let semicolon    = C64Key(45)
    static let home         = C64Key(14)
    static let rightShift   = C64Key(61)
    static let equal        = C64Key(46)
    static let upArrow      = C64Key(30)
    static let slash        = C64Key(60)
    
    // Eights row in key matrix
    static let digit1       = C64Key(1)
    static let leftArrow    = C64Key(0)
    static let control      = C64Key(17)
    static let digit2       = C64Key(2)
    static let space        = C64Key(65)
    static let commodore    = C64Key(49)
    static let Q            = C64Key(18)
    static let runStop      = C64Key(33)
    
    // Restore key
    static let restore      = C64Key(31)
}

extension C64Key: Equatable {
    static func == (lhs: C64Key, rhs: C64Key) -> Bool {
        return lhs.nr == rhs.nr
    }
}

extension C64Key: Hashable {
    func hash(into hasher: inout Hasher) {
        return hasher.combine(nr)
    }
}

//
// Symbolic key mapping
//

extension C64Key {
    
    /* Translates a character to a list of C64 keys. This function is called in
     * symbolic key mapping mode when the user hits a key.
     */
    static func translate(char: String?) -> [C64Key] {
        
        if char == nil { return [] }
        
        switch char! {
            
        // First physical row
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
            
        // Second physical row
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
            
        // Third physical row
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
            
        // Fourth physical row
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
            
        // Fifth physical row
        case " ": return [C64Key.space]
            
        default: return []
        }
    }
    
    // Returns an image for being used in the configuring panel
    func image(keyCode: String, red: Bool = false) -> NSImage {
        
        assert(nr != 31 /* RESTORE */)
        assert(nr != 34 /* SHIFT LOCK */)
        assert(row >= 0 && row < 8)
        assert(col >= 0 && col < 8)
        
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
        let name = [
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
        
        let image = plainKeyImage(width: Int(width), height: Int(height), red: red)
        let imageRect = CGRect(x: 0, y: 0, width: width, height: height)
        let textRect1 = CGRect(x: 7, y: -2, width: width-7, height: height-2)
        let textRect2 = CGRect(x: 14, y: -10, width: width-14, height: height-10)
        let textStyle = NSMutableParagraphStyle.default.mutableCopy() as? NSMutableParagraphStyle
        let font1 = NSFont.systemFont(ofSize: 12)
        let font2 = NSFont.systemFont(ofSize: 16)
        let textFontAttributes1 = [
            NSAttributedString.Key.font: font1,
            NSAttributedString.Key.foregroundColor: NSColor.gray,
            NSAttributedString.Key.paragraphStyle: textStyle!
        ]
        let textFontAttributes2 = [
            NSAttributedString.Key.font: font2,
            NSAttributedString.Key.foregroundColor: NSColor.black,
            NSAttributedString.Key.paragraphStyle: textStyle!
        ]
        
        image.lockFocus()
        image.draw(in: imageRect)
        description.draw(in: textRect1, withAttributes: textFontAttributes1)
        keyCode.draw(in: textRect2, withAttributes: textFontAttributes2)
        image.unlockFocus()
        
        return image
    }
    
    func plainKeyImage(width: Int, height: Int,
                       dark: Bool = false, red: Bool = false) -> NSImage {
        
        let name = NSImage.Name(red ? "key_red" : dark ? "key_dark" : "key")
        let background = NSImage(named: name)!
        return background.resize(width: CGFloat(width), height: CGFloat(height))
    }
}

//
// Virtual keyboard
//

extension C64Key {
    
    static var map: [Int: [Modifier: Keycap]] = [:]
    
    static func lookupKeycap(for key: Int, modifier: Modifier) -> Keycap? {
        
        // Lookup the keycap for the provided key
        if C64Key.map.isEmpty { initMap() }
        guard let map = C64Key.map[key] else { fatalError() }
        
        // Search for a perfect match
        if let keycap = map[modifier] { return keycap }
        
        // Search for a match that fits as much as possible
        if modifier.contains(.commodore) {
            if let keycap = map[.commodore] { return keycap }
        }
        if modifier.contains(.lowercase) {
            if let keycap = map[.lowercase] { return keycap }
        }
        if modifier.contains(.shift) {
            if let keycap = map[.shift] { return keycap }
        }
        if modifier.contains(.control) {
            if let keycap = map[.control] { return keycap }
        }

        // Return the generic keycap
        let none: Modifier = []
        return map[none]
    }
    
    static func initMap() {
                
        //
        // Function keys
        //
        
        C64Key.map[16] = [ []: Keycap("f 1", shape: "150x100", dark: true),
                           [.shift]: Keycap("f 2", shape: "150x100", dark: true) ]
        
        C64Key.map[32] = [ []: Keycap("f 3", shape: "150x100", dark: true),
                           [.shift]: Keycap("f 4", shape: "150x100", dark: true) ]
        
        C64Key.map[48] = [ []: Keycap("f 5", shape: "150x100", dark: true),
                           [.shift]: Keycap("f 6", shape: "150x100", dark: true) ]
        
        C64Key.map[64] = [ []: Keycap("f 7", shape: "150x100", dark: true),
                           [.shift]: Keycap("f 8", shape: "150x100", dark: true) ]
        
        //
        // First row
        //
        
        C64Key.map[0] = [ []: Keycap("\u{2190}" /* left arrow */) ]
        
        C64Key.map[1] = [ []: Keycap("1"),
                          [.shift]: Keycap("!"),
                          [.commodore]: Keycap("ORG"),
                          [.control]: Keycap("BLK") ]
        
        C64Key.map[2] = [ []: Keycap("2"),
                          [.shift]: Keycap("\""),
                          [.commodore]: Keycap("BRN"),
                          [.control]: Keycap("WHT") ]
        
        C64Key.map[3] = [ []: Keycap("3"),
                          [.shift]: Keycap("#"),
                          [.commodore]: Keycap("PNK"),
                          [.control]: Keycap("RED") ]
        
        C64Key.map[4] = [ []: Keycap("4"),
                          [.shift]: Keycap("$"),
                          [.commodore]: Keycap("DARK", "GRY"),
                          [.control]: Keycap("CYN") ]
        
        C64Key.map[5] = [ []: Keycap("5"),
                          [.shift]: Keycap("%"),
                          [.commodore]: Keycap("GRY"),
                          [.control]: Keycap("PUR") ]
        
        C64Key.map[6] = [ []: Keycap("6"),
                          [.shift]: Keycap("&"),
                          [.commodore]: Keycap("LIGHT", "GRN"),
                          [.control]: Keycap("GRN") ]
        
        C64Key.map[7] = [ []: Keycap("7"),
                          [.shift]: Keycap("'"),
                          [.commodore]: Keycap("LIGHT", "BLU"),
                          [.control]: Keycap("BLU") ]
        
        C64Key.map[8] = [ []: Keycap("8"),
                          [.shift]: Keycap("("),
                          [.commodore]: Keycap("LIGHT", "GRY"),
                          [.control]: Keycap("YEL") ]
        
        C64Key.map[9] = [ []: Keycap("9"),
                          [.shift]: Keycap(")"),
                          [.commodore]: Keycap(")"),
                          [.control]: Keycap("RVS", "ON") ]
        
        C64Key.map[10] = [ []: Keycap("0"),
                           [.control]: Keycap("RVS", "OFF") ]
        
        C64Key.map[11] = [ []: Keycap("+"),
                           [.shift]: Keycap("\u{253c}", font: "C64"),
                           [.commodore]: Keycap("\u{2592}", font: "C64") ]
        
        C64Key.map[12] = [ []: Keycap("-"),
                           [.shift]: Keycap("\u{2502}", font: "C64"),
                           [.commodore]: Keycap("\u{e0dc}", font: "C64") ]
        
        C64Key.map[13] = [ []: Keycap("\u{00a3}" /* Pound */),
                           [.shift]: Keycap("\u{25e4}", font: "C64"),
                           [.shift,.lowercase]: Keycap("\u{e1e9}", font: "C64"),
                           [.commodore]: Keycap("\u{e0a8}", font: "C64") ]
        
        C64Key.map[14] = [ []: Keycap("HOME"),
                           [.shift]: Keycap("CLR") ]
        
        C64Key.map[15] = [ []: Keycap("DEL"),
                           [.shift]: Keycap("INST") ]
        
        //
        // Second row
        //
        
        C64Key.map[17] = [ []: Keycap("C T R L", shape: "150x100") ]
        
        C64Key.map[18] = [ []: Keycap("Q"),
                           [.shift]: Keycap("\u{2022}", font: "C64"),
                           [.lowercase]: Keycap("q"),
                           [.shift,.lowercase]: Keycap("Q"),
                           [.commodore]: Keycap("\u{251c}", font: "C64") ]
        
        C64Key.map[19] = [ []: Keycap("W"),
                           [.shift]: Keycap("\u{25cb}", font: "C64"),
                           [.lowercase]: Keycap("w"),
                           [.shift,.lowercase]: Keycap("W"),
                           [.commodore]: Keycap("\u{2524}", font: "C64") ]
        
        C64Key.map[20] = [ []: Keycap("E"),
                           [.shift]: Keycap("\u{e0c5}", font: "C64"),
                           [.lowercase]: Keycap("e"),
                           [.shift,.lowercase]: Keycap("E"),
                           [.commodore]: Keycap("\u{2534}", font: "C64") ]
        
        C64Key.map[21] = [ []: Keycap("R"),
                           [.shift]: Keycap("\u{e072}", font: "C64"),
                           [.lowercase]: Keycap("r"),
                           [.shift,.lowercase]: Keycap("R"),
                           [.commodore]: Keycap("\u{252c}", font: "C64") ]
        
        C64Key.map[22] = [ []: Keycap("T"),
                           [.shift]: Keycap("\u{e0d4}", font: "C64"),
                           [.lowercase]: Keycap("t"),
                           [.shift,.lowercase]: Keycap("T"),
                           [.commodore]: Keycap("\u{2594}", font: "C64") ]
        
        C64Key.map[23] = [ []: Keycap("Y"),
                           [.shift]: Keycap("\u{e0d9}", font: "C64"),
                           [.lowercase]: Keycap("y"),
                           [.shift,.lowercase]: Keycap("Y"),
                           [.commodore]: Keycap("\u{e0b7}", font: "C64") ]
        
        C64Key.map[24] = [ []: Keycap("U"),
                           [.shift]: Keycap("\u{256d}", font: "C64"),
                           [.lowercase]: Keycap("u"),
                           [.shift,.lowercase]: Keycap("U"),
                           [.commodore]: Keycap("\u{e0b8}", font: "C64") ]
        
        C64Key.map[25] = [ []: Keycap("I"),
                           [.shift]: Keycap("\u{256e}", font: "C64"),
                           [.lowercase]: Keycap("i"),
                           [.shift,.lowercase]: Keycap("I"),
                           [.commodore]: Keycap("\u{2584}", font: "C64") ]
        
        C64Key.map[26] = [ []: Keycap("O"),
                           [.shift]: Keycap("\u{e0cf}", font: "C64"),
                           [.lowercase]: Keycap("o"),
                           [.shift,.lowercase]: Keycap("O"),
                           [.commodore]: Keycap("\u{2583}", font: "C64") ]
        
        C64Key.map[27] = [ []: Keycap("P"),
                           [.shift]: Keycap("\u{e0d0}", font: "C64"),
                           [.lowercase]: Keycap("p"),
                           [.shift,.lowercase]: Keycap("P"),
                           [.commodore]: Keycap("\u{2582}", font: "C64") ]
        
        C64Key.map[28] = [ []: Keycap("@"),
                           [.shift]: Keycap("\u{e0ba}", font: "C64"),
                           [.shift,.lowercase]: Keycap("\u{e1fa}", font: "C64"),
                           [.commodore]: Keycap("\u{2581}", font: "C64") ]
        
        C64Key.map[29] = [ []: Keycap("*"),
                           [.shift]: Keycap("\u{2500}", font: "C64"),
                           [.commodore]: Keycap("\u{25e5}", font: "C64"),
                           [.commodore,.lowercase]: Keycap("\u{e17f}", font: "C64") ]
        
        C64Key.map[30] = [ []: Keycap("\u{2191}"),
                           [.shift]: Keycap("\u{03C0}"),
                           [.commodore]: Keycap("\u{03C0}"),
                           [.lowercase]: Keycap("\u{e1de}", font: "C64") ]
        
        C64Key.map[31] = [ []: Keycap("RESTORE", shape: "150x100") ]
        
        //
        // Third row
        //
        
        C64Key.map[33] = [ []: Keycap("RUN", "STOP") ]
        
        C64Key.map[34] = [ []: Keycap("SHIFT", "LOCK") ]
        
        C64Key.map[35] = [ []: Keycap("A"),
                           [.shift]: Keycap("\u{2660}", font: "C64"),
                           [.lowercase]: Keycap("a"),
                           [.shift,.lowercase]: Keycap("A"),
                           [.commodore]: Keycap("\u{250c}", font: "C64") ]
        
        C64Key.map[36] = [ []: Keycap("S"),
                           [.shift]: Keycap("\u{2665}", font: "C64"),
                           [.lowercase]: Keycap("s"),
                           [.shift,.lowercase]: Keycap("S"),
                           [.commodore]: Keycap("\u{2510}", font: "C64") ]
        
        C64Key.map[37] = [ []: Keycap("D"),
                           [.shift]: Keycap("\u{e064}", font: "C64"),
                           [.lowercase]: Keycap("d"),
                           [.shift,.lowercase]: Keycap("D"),
                           [.commodore]: Keycap("\u{2597}", font: "C64") ]
        
        C64Key.map[38] = [ []: Keycap("F"),
                           [.shift]: Keycap("\u{e0c6}", font: "C64"),
                           [.lowercase]: Keycap("f"),
                           [.shift,.lowercase]: Keycap("F"),
                           [.commodore]: Keycap("\u{2596}", font: "C64") ]
        
        C64Key.map[39] = [ []: Keycap("G"),
                           [.shift]: Keycap("\u{e0c7}", font: "C64"),
                           [.lowercase]: Keycap("g"),
                           [.shift,.lowercase]: Keycap("G"),
                           [.commodore]: Keycap("\u{258e}", font: "C64") ]
        
        C64Key.map[40] = [ []: Keycap("H"),
                           [.shift]: Keycap("\u{e0c8}", font: "C64"),
                           [.lowercase]: Keycap("h"),
                           [.shift,.lowercase]: Keycap("H"),
                           [.commodore]: Keycap("\u{258e}", font: "C64") ]
        
        C64Key.map[41] = [ []: Keycap("J"),
                           [.shift]: Keycap("\u{2570}", font: "C64"),
                           [.lowercase]: Keycap("j"),
                           [.shift,.lowercase]: Keycap("J"),
                           [.commodore]: Keycap("\u{258d}", font: "C64") ]
        
        C64Key.map[42] = [ []: Keycap("K"),
                           [.shift]: Keycap("\u{256f}", font: "C64"),
                           [.lowercase]: Keycap("k"),
                           [.shift,.lowercase]: Keycap("K"),
                           [.commodore]: Keycap("\u{258c}", font: "C64") ]
        
        C64Key.map[43] = [ []: Keycap("L"),
                           [.shift]: Keycap("\u{e0cc}", font: "C64"),
                           [.lowercase]: Keycap("l"),
                           [.shift,.lowercase]: Keycap("L"),
                           [.commodore]: Keycap("\u{e0b6}", font: "C64") ]
        
        C64Key.map[44] = [ []: Keycap(":"),
                           [.shift]: Keycap("["),
                           [.commodore]: Keycap("[") ]
        
        C64Key.map[45] = [ []: Keycap(";"),
                           [.shift]: Keycap("]"),
                           [.commodore]: Keycap("]") ]
        
        C64Key.map[46] = [ []: Keycap("=") ]
        
        C64Key.map[47] = [ []: Keycap("RETURN", shape: "200x100") ]
        
        //
        // Fourth row
        //
        
        C64Key.map[49] = [ []: Keycap(" ", shape: "Commodore") ]
        
        C64Key.map[50] = [ []: Keycap("SHIFT", shape: "150x100") ]
        
        C64Key.map[51] = [ []: Keycap("Z"),
                           [.shift]: Keycap("\u{2666}", font: "C64"),
                           [.lowercase]: Keycap("z"),
                           [.shift,.lowercase]: Keycap("Z"),
                           [.commodore]: Keycap("\u{2514}", font: "C64") ]
        
        C64Key.map[52] = [ []: Keycap("X"),
                           [.shift]: Keycap("\u{2663}", font: "C64"),
                           [.lowercase]: Keycap("x"),
                           [.shift,.lowercase]: Keycap("X"),
                           [.commodore]: Keycap("\u{2518}", font: "C64") ]
        
        C64Key.map[53] = [ []: Keycap("C"),
                           [.shift]: Keycap("\u{2500}", font: "C64"),
                           [.lowercase]: Keycap("c"),
                           [.shift,.lowercase]: Keycap("C"),
                           [.commodore]: Keycap("\u{259d}", font: "C64") ]
        
        C64Key.map[54] = [ []: Keycap("V"),
                           [.shift]: Keycap("\u{2573}", font: "C64"),
                           [.lowercase]: Keycap("v"),
                           [.shift,.lowercase]: Keycap("V"),
                           [.commodore]: Keycap("\u{2598}", font: "C64") ]
        
        C64Key.map[55] = [ []: Keycap("B"),
                           [.shift]: Keycap("\u{2502}", font: "C64"),
                           [.lowercase]: Keycap("b"),
                           [.shift,.lowercase]: Keycap("B"),
                           [.commodore]: Keycap("\u{259a}", font: "C64") ]
        
        C64Key.map[56] = [ []: Keycap("N"),
                           [.shift]: Keycap("\u{2571}", font: "C64"),
                           [.lowercase]: Keycap("n"),
                           [.shift,.lowercase]: Keycap("N"),
                           [.commodore]: Keycap("\u{e0aa}", font: "C64") ]
        
        C64Key.map[57] = [ []: Keycap("M"),
                           [.shift]: Keycap("\u{2572}", font: "C64"),
                           [.lowercase]: Keycap("m"),
                           [.shift,.lowercase]: Keycap("N"),
                           [.commodore]: Keycap("\u{e0a7}", font: "C64") ]
        
        C64Key.map[58] = [ []: Keycap(","),
                           [.shift]: Keycap("<"),
                           [.commodore]: Keycap("<") ]
        
        C64Key.map[59] = [ []: Keycap("."),
                           [.shift]: Keycap(">"),
                           [.commodore]: Keycap(">") ]
        
        C64Key.map[60] = [ []: Keycap("/"),
                           [.shift]: Keycap("?"),
                           [.commodore]: Keycap("?") ]
        
        C64Key.map[61] = [ []: Keycap("SHIFT", shape: "150x100") ]
        
        C64Key.map[62] = [ []: Keycap("\u{21e9}"),
                           [.shift]: Keycap("\u{21e7}") ]
        
        C64Key.map[63] = [ []: Keycap("\u{21e8}"),
                           [.shift]: Keycap("\u{21e6}") ]
        
        //
        // Fifth row
        //
        
        C64Key.map[65] = [ []: Keycap("", shape: "900x100") ]
    }
}

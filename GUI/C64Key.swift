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
    static let delete       = C64Key.init(15)
    static let ret          = C64Key.init(47)
    static let curLeftRight = C64Key.init(63)
    static let F7F8         = C64Key.init(64)
    static let F1F2         = C64Key.init(16)
    static let F3F4         = C64Key.init(32)
    static let F5F6         = C64Key.init(48)
    static let curUpDown    = C64Key.init(62)
    
    // Second row in key matrix
    static let digit3       = C64Key.init(3)
    static let W            = C64Key.init(19)
    static let A            = C64Key.init(35)
    static let digit4       = C64Key.init(4)
    static let Z            = C64Key.init(51)
    static let S            = C64Key.init(36)
    static let E            = C64Key.init(20)
    static let shift        = C64Key.init(50)
    
    // Third row in key matrix
    static let digit5       = C64Key.init(5)
    static let R            = C64Key.init(21)
    static let D            = C64Key.init(37)
    static let digit6       = C64Key.init(6)
    static let C            = C64Key.init(53)
    static let F            = C64Key.init(38)
    static let T            = C64Key.init(22)
    static let X            = C64Key.init(52)
    
    // Fourth row in key matrix
    static let digit7       = C64Key.init(7)
    static let Y            = C64Key.init(23)
    static let G            = C64Key.init(39)
    static let digit8       = C64Key.init(8)
    static let B            = C64Key.init(55)
    static let H            = C64Key.init(40)
    static let U            = C64Key.init(24)
    static let V            = C64Key.init(54)
    
    // Fifth row in key matrix
    static let digit9       = C64Key.init(9)
    static let I            = C64Key.init(25)
    static let J            = C64Key.init(41)
    static let digit0       = C64Key.init(10)
    static let M            = C64Key.init(57)
    static let K            = C64Key.init(42)
    static let O            = C64Key.init(26)
    static let N            = C64Key.init(56)
    
    // Sixth row in key matrix
    static let plus         = C64Key.init(11)
    static let P            = C64Key.init(27)
    static let L            = C64Key.init(43)
    static let minus        = C64Key.init(12)
    static let period       = C64Key.init(59)
    static let colon        = C64Key.init(44)
    static let at           = C64Key.init(28)
    static let comma        = C64Key.init(58)
    
    // Seventh row in key matrix
    static let pound        = C64Key.init(13)
    static let asterisk     = C64Key.init(29)
    static let semicolon    = C64Key.init(45)
    static let home         = C64Key.init(14)
    static let rightShift   = C64Key.init(61)
    static let equal        = C64Key.init(46)
    static let upArrow      = C64Key.init(30)
    static let slash        = C64Key.init(60)
    
    // Eights row in key matrix
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
        
        track()
        
        //
        // Function keys
        //
        
        C64Key.map[16] = [ []: Keycap.init("f 1", shape: "150x100", dark: true),
                           [.shift]: Keycap.init("f 2", shape: "150x100", dark: true) ]
        
        C64Key.map[32] = [ []: Keycap.init("f 3", shape: "150x100", dark: true),
                           [.shift]: Keycap.init("f 4", shape: "150x100", dark: true) ]
        
        C64Key.map[48] = [ []: Keycap.init("f 5", shape: "150x100", dark: true),
                           [.shift]: Keycap.init("f 6", shape: "150x100", dark: true) ]
        
        C64Key.map[64] = [ []: Keycap.init("f 7", shape: "150x100", dark: true),
                           [.shift]: Keycap.init("f 8", shape: "150x100", dark: true) ]
        
        //
        // First row
        //
        
        C64Key.map[0] = [ []: Keycap.init("\u{2190}" /* left arrow */) ]
        
        C64Key.map[1] = [ []: Keycap.init("1"),
                          [.shift]: Keycap.init("!"),
                          [.commodore]: Keycap.init("ORG"),
                          [.control]: Keycap.init("BLK") ]
        
        C64Key.map[2] = [ []: Keycap.init("2"),
                          [.shift]: Keycap.init("\""),
                          [.commodore]: Keycap.init("BRN"),
                          [.control]: Keycap.init("WHT") ]
        
        C64Key.map[3] = [ []: Keycap.init("3"),
                          [.shift]: Keycap.init("#"),
                          [.commodore]: Keycap.init("PNK"),
                          [.control]: Keycap.init("RED") ]
        
        C64Key.map[4] = [ []: Keycap.init("4"),
                          [.shift]: Keycap.init("$"),
                          [.commodore]: Keycap.init("DARK", "GRY"),
                          [.control]: Keycap.init("CYN") ]
        
        C64Key.map[5] = [ []: Keycap.init("5"),
                          [.shift]: Keycap.init("%"),
                          [.commodore]: Keycap.init("GRY"),
                          [.control]: Keycap.init("PUR") ]
        
        C64Key.map[6] = [ []: Keycap.init("6"),
                          [.shift]: Keycap.init("&"),
                          [.commodore]: Keycap.init("LIGHT", "GRN"),
                          [.control]: Keycap.init("GRN") ]
        
        C64Key.map[7] = [ []: Keycap.init("7"),
                          [.shift]: Keycap.init("'"),
                          [.commodore]: Keycap.init("LIGHT", "BLU"),
                          [.control]: Keycap.init("BLU") ]
        
        C64Key.map[8] = [ []: Keycap.init("8"),
                          [.shift]: Keycap.init("("),
                          [.commodore]: Keycap.init("LIGHT", "GRY"),
                          [.control]: Keycap.init("YEL") ]
        
        C64Key.map[9] = [ []: Keycap.init("9"),
                          [.shift]: Keycap.init(")"),
                          [.commodore]: Keycap.init(")"),
                          [.control]: Keycap.init("RVS", "ON") ]
        
        C64Key.map[10] = [ []: Keycap.init("0"),
                           [.control]: Keycap.init("RVS", "OFF") ]
        
        C64Key.map[11] = [ []: Keycap.init("+"),
                           [.shift]: Keycap.init("\u{253c}", font: "C64"),
                           [.commodore]: Keycap.init("\u{2592}", font: "C64") ]
        
        C64Key.map[12] = [ []: Keycap.init("-"),
                           [.shift]: Keycap.init("\u{2502}", font: "C64"),
                           [.commodore]: Keycap.init("\u{e0dc}", font: "C64") ]
        
        C64Key.map[13] = [ []: Keycap.init("\u{00a3}" /* Pound */),
                           [.shift]: Keycap.init("\u{25e4}", font: "C64"),
                           [.shift,.lowercase]: Keycap.init("\u{e1e9}", font: "C64"),
                           [.commodore]: Keycap.init("\u{e0a8}", font: "C64") ]
        
        C64Key.map[14] = [ []: Keycap.init("HOME"),
                           [.shift]: Keycap.init("CLR") ]
        
        C64Key.map[15] = [ []: Keycap.init("DEL"),
                           [.shift]: Keycap.init("INST") ]
        
        //
        // Second row
        //
        
        C64Key.map[17] = [ []: Keycap.init("C T R L", shape: "150x100") ]
        
        C64Key.map[18] = [ []: Keycap.init("Q"),
                           [.shift]: Keycap.init("\u{2022}", font: "C64"),
                           [.lowercase]: Keycap.init("q"),
                           [.shift,.lowercase]: Keycap.init("Q"),
                           [.commodore]: Keycap.init("\u{251c}", font: "C64") ]
        
        C64Key.map[19] = [ []: Keycap.init("W"),
                           [.shift]: Keycap.init("\u{25cb}", font: "C64"),
                           [.lowercase]: Keycap.init("w"),
                           [.shift,.lowercase]: Keycap.init("W"),
                           [.commodore]: Keycap.init("\u{2524}", font: "C64") ]
        
        C64Key.map[20] = [ []: Keycap.init("E"),
                           [.shift]: Keycap.init("\u{e0c5}", font: "C64"),
                           [.lowercase]: Keycap.init("e"),
                           [.shift,.lowercase]: Keycap.init("E"),
                           [.commodore]: Keycap.init("\u{2534}", font: "C64") ]
        
        C64Key.map[21] = [ []: Keycap.init("R"),
                           [.shift]: Keycap.init("\u{e072}", font: "C64"),
                           [.lowercase]: Keycap.init("r"),
                           [.shift,.lowercase]: Keycap.init("R"),
                           [.commodore]: Keycap.init("\u{252c}", font: "C64") ]
        
        C64Key.map[22] = [ []: Keycap.init("T"),
                           [.shift]: Keycap.init("\u{e0d4}", font: "C64"),
                           [.lowercase]: Keycap.init("t"),
                           [.shift,.lowercase]: Keycap.init("T"),
                           [.commodore]: Keycap.init("\u{2594}", font: "C64") ]
        
        C64Key.map[23] = [ []: Keycap.init("Y"),
                           [.shift]: Keycap.init("\u{e0d9}", font: "C64"),
                           [.lowercase]: Keycap.init("y"),
                           [.shift,.lowercase]: Keycap.init("Y"),
                           [.commodore]: Keycap.init("\u{e0b7}", font: "C64") ]
        
        C64Key.map[24] = [ []: Keycap.init("U"),
                           [.shift]: Keycap.init("\u{256d}", font: "C64"),
                           [.lowercase]: Keycap.init("u"),
                           [.shift,.lowercase]: Keycap.init("U"),
                           [.commodore]: Keycap.init("\u{e0b8}", font: "C64") ]
        
        C64Key.map[25] = [ []: Keycap.init("I"),
                           [.shift]: Keycap.init("\u{256e}", font: "C64"),
                           [.lowercase]: Keycap.init("i"),
                           [.shift,.lowercase]: Keycap.init("I"),
                           [.commodore]: Keycap.init("\u{2584}", font: "C64") ]
        
        C64Key.map[26] = [ []: Keycap.init("O"),
                           [.shift]: Keycap.init("\u{e0cf}", font: "C64"),
                           [.lowercase]: Keycap.init("o"),
                           [.shift,.lowercase]: Keycap.init("O"),
                           [.commodore]: Keycap.init("\u{2583}", font: "C64") ]
        
        C64Key.map[27] = [ []: Keycap.init("P"),
                           [.shift]: Keycap.init("\u{e0d0}", font: "C64"),
                           [.lowercase]: Keycap.init("p"),
                           [.shift,.lowercase]: Keycap.init("P"),
                           [.commodore]: Keycap.init("\u{2582}", font: "C64") ]
        
        C64Key.map[28] = [ []: Keycap.init("@"),
                           [.shift]: Keycap.init("\u{e0ba}", font: "C64"),
                           [.shift,.lowercase]: Keycap.init("\u{e1fa}", font: "C64"),
                           [.commodore]: Keycap.init("\u{2581}", font: "C64") ]
        
        C64Key.map[29] = [ []: Keycap.init("*"),
                           [.shift]: Keycap.init("\u{2500}", font: "C64"),
                           [.commodore]: Keycap.init("\u{25e5}", font: "C64"),
                           [.commodore,.lowercase]: Keycap.init("\u{e17f}", font: "C64") ]
        
        C64Key.map[30] = [ []: Keycap.init("\u{2191}"),
                           [.shift]: Keycap.init("\u{03C0}"),
                           [.commodore]: Keycap.init("\u{03C0}"),
                           [.lowercase]: Keycap.init("\u{e1de}", font: "C64") ]
        
        C64Key.map[31] = [ []: Keycap.init("RESTORE", shape: "150x100") ]
        
        //
        // Third row
        //
        
        C64Key.map[33] = [ []: Keycap.init("RUN", "STOP") ]
        
        C64Key.map[34] = [ []: Keycap.init("SHIFT", "LOCK") ]
        
        C64Key.map[35] = [ []: Keycap.init("A"),
                           [.shift]: Keycap.init("\u{2660}", font: "C64"),
                           [.lowercase]: Keycap.init("a"),
                           [.shift,.lowercase]: Keycap.init("A"),
                           [.commodore]: Keycap.init("\u{250c}", font: "C64") ]
        
        C64Key.map[36] = [ []: Keycap.init("S"),
                           [.shift]: Keycap.init("\u{2665}", font: "C64"),
                           [.lowercase]: Keycap.init("s"),
                           [.shift,.lowercase]: Keycap.init("S"),
                           [.commodore]: Keycap.init("\u{2510}", font: "C64") ]
        
        C64Key.map[37] = [ []: Keycap.init("D"),
                           [.shift]: Keycap.init("\u{e064}", font: "C64"),
                           [.lowercase]: Keycap.init("d"),
                           [.shift,.lowercase]: Keycap.init("D"),
                           [.commodore]: Keycap.init("\u{2597}", font: "C64") ]
        
        C64Key.map[38] = [ []: Keycap.init("F"),
                           [.shift]: Keycap.init("\u{e0c6}", font: "C64"),
                           [.lowercase]: Keycap.init("f"),
                           [.shift,.lowercase]: Keycap.init("F"),
                           [.commodore]: Keycap.init("\u{2596}", font: "C64") ]
        
        C64Key.map[39] = [ []: Keycap.init("G"),
                           [.shift]: Keycap.init("\u{e0c7}", font: "C64"),
                           [.lowercase]: Keycap.init("g"),
                           [.shift,.lowercase]: Keycap.init("G"),
                           [.commodore]: Keycap.init("\u{258e}", font: "C64") ]
        
        C64Key.map[40] = [ []: Keycap.init("H"),
                           [.shift]: Keycap.init("\u{e0c8}", font: "C64"),
                           [.lowercase]: Keycap.init("h"),
                           [.shift,.lowercase]: Keycap.init("H"),
                           [.commodore]: Keycap.init("\u{258e}", font: "C64") ]
        
        C64Key.map[41] = [ []: Keycap.init("J"),
                           [.shift]: Keycap.init("\u{2570}", font: "C64"),
                           [.lowercase]: Keycap.init("j"),
                           [.shift,.lowercase]: Keycap.init("J"),
                           [.commodore]: Keycap.init("\u{258d}", font: "C64") ]
        
        C64Key.map[42] = [ []: Keycap.init("K"),
                           [.shift]: Keycap.init("\u{256f}", font: "C64"),
                           [.lowercase]: Keycap.init("k"),
                           [.shift,.lowercase]: Keycap.init("K"),
                           [.commodore]: Keycap.init("\u{258c}", font: "C64") ]
        
        C64Key.map[43] = [ []: Keycap.init("L"),
                           [.shift]: Keycap.init("\u{e0cc}", font: "C64"),
                           [.lowercase]: Keycap.init("l"),
                           [.shift,.lowercase]: Keycap.init("L"),
                           [.commodore]: Keycap.init("\u{e0b6}", font: "C64") ]
        
        C64Key.map[44] = [ []: Keycap.init(":"),
                           [.shift]: Keycap.init("["),
                           [.commodore]: Keycap.init("[") ]
        
        C64Key.map[45] = [ []: Keycap.init(";"),
                           [.shift]: Keycap.init("]"),
                           [.commodore]: Keycap.init("]") ]
        
        C64Key.map[46] = [ []: Keycap.init("=") ]
        
        C64Key.map[47] = [ []: Keycap.init("RETURN", shape: "200x100") ]
        
        //
        // Fourth row
        //
        
        C64Key.map[49] = [ []: Keycap.init(" ", shape: "Commodore") ]
        
        C64Key.map[50] = [ []: Keycap.init("SHIFT", shape: "150x100") ]
        
        C64Key.map[51] = [ []: Keycap.init("Z"),
                           [.shift]: Keycap.init("\u{2666}", font: "C64"),
                           [.lowercase]: Keycap.init("z"),
                           [.shift,.lowercase]: Keycap.init("Z"),
                           [.commodore]: Keycap.init("\u{2514}", font: "C64") ]
        
        C64Key.map[52] = [ []: Keycap.init("X"),
                           [.shift]: Keycap.init("\u{2663}", font: "C64"),
                           [.lowercase]: Keycap.init("x"),
                           [.shift,.lowercase]: Keycap.init("X"),
                           [.commodore]: Keycap.init("\u{2518}", font: "C64") ]
        
        C64Key.map[53] = [ []: Keycap.init("C"),
                           [.shift]: Keycap.init("\u{2500}", font: "C64"),
                           [.lowercase]: Keycap.init("c"),
                           [.shift,.lowercase]: Keycap.init("C"),
                           [.commodore]: Keycap.init("\u{259d}", font: "C64") ]
        
        C64Key.map[54] = [ []: Keycap.init("V"),
                           [.shift]: Keycap.init("\u{2573}", font: "C64"),
                           [.lowercase]: Keycap.init("v"),
                           [.shift,.lowercase]: Keycap.init("V"),
                           [.commodore]: Keycap.init("\u{2598}", font: "C64") ]
        
        C64Key.map[55] = [ []: Keycap.init("B"),
                           [.shift]: Keycap.init("\u{2502}", font: "C64"),
                           [.lowercase]: Keycap.init("b"),
                           [.shift,.lowercase]: Keycap.init("B"),
                           [.commodore]: Keycap.init("\u{259a}", font: "C64") ]
        
        C64Key.map[56] = [ []: Keycap.init("N"),
                           [.shift]: Keycap.init("\u{2571}", font: "C64"),
                           [.lowercase]: Keycap.init("n"),
                           [.shift,.lowercase]: Keycap.init("N"),
                           [.commodore]: Keycap.init("\u{e0aa}", font: "C64") ]
        
        C64Key.map[57] = [ []: Keycap.init("M"),
                           [.shift]: Keycap.init("\u{2572}", font: "C64"),
                           [.lowercase]: Keycap.init("m"),
                           [.shift,.lowercase]: Keycap.init("N"),
                           [.commodore]: Keycap.init("\u{e0a7}", font: "C64") ]
        
        C64Key.map[58] = [ []: Keycap.init(","),
                           [.shift]: Keycap.init("<"),
                           [.commodore]: Keycap.init("<") ]
        
        C64Key.map[59] = [ []: Keycap.init("."),
                           [.shift]: Keycap.init(">"),
                           [.commodore]: Keycap.init(">") ]
        
        C64Key.map[60] = [ []: Keycap.init("/"),
                           [.shift]: Keycap.init("?"),
                           [.commodore]: Keycap.init("?") ]
        
        C64Key.map[61] = [ []: Keycap.init("SHIFT", shape: "150x100") ]
        
        C64Key.map[62] = [ []: Keycap.init("\u{21e9}"),
                           [.shift]: Keycap.init("\u{21e7}") ]
        
        C64Key.map[63] = [ []: Keycap.init("\u{21e8}"),
                           [.shift]: Keycap.init("\u{21e6}") ]
        
        //
        // Fifth row
        //
        
        C64Key.map[65] = [ []: Keycap.init("", shape: "900x100") ]
    }
}

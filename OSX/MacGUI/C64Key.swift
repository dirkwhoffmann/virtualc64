//
//  C64Key.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 18.02.18.
//

import Foundation

/// The C64Key structure represents a physical keys on the C64 keyboard.
/// The key is specified by its row and column position in the C64 keyboard matrix.
struct C64Key : Codable {
    
    // Row index
    var row = 0
    
    // Column index
    var col = 0
    
    // Textual description of this key
    var description: String = ""
    
    init(row: Int, col: Int, characters: String) {
        
        self.row = row
        self.col = col
        self.description = characters
    }
    
    init(row: Int, col: Int) {
        
        precondition(row >= 0 && row <= 8 && col >= 0 && col <= 8)
        
        let curUD = "CU \u{21c5}" // "\u{2191}\u{2193}"
        let curLR = "CU \u{21c6}" // "\u{2190}\u{2192}"
        let shiftL = "\u{21e7}"
        let shiftR = "      \u{21e7}"
        var name = [
            ["DEL",      "\u{21b5}", curLR,      "F7",   "F1",       "F3", "F5",      curUD],
            ["3",        "W",        "A",        "4",    "Z",        "S",  "E",       shiftL],
            ["5",        "R",        "D",        "6",    "C",        "F",  "T",       "X"],
            ["7",        "Y",        "G",        "8",    "B",        "H",  "U",       "V"],
            ["9",        "I",        "J",        "0",    "M",        "K",  "O",       "N"],
            ["+",        "P",        "L",        "-",    ".",        ":",  "@",       ","],
            ["\u{00a3}", "*",        ";",        "HOME", shiftR,     "=",  "\u{2191}", "/"],
            ["1",        "\u{2190}", "CTRL",     "2",    "\u{23b5}", "C=", "Q",        "STOP"]]

        self.init(row: row, col: col, characters: name[row][col])
    }
    
    /// Image representation of this key
    func image(auxiliaryText: NSString = "") -> NSImage {
        
        let background = NSImage(named: NSImage.Name(rawValue: "key"))!
        let width = 48.0
        let height = 48.0
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
        let outImage = NSImage(size: NSSize.init(width: width, height: height))
        let rep:NSBitmapImageRep = NSBitmapImageRep(bitmapDataPlanes: nil,
                                                    pixelsWide: Int(width),
                                                    pixelsHigh: Int(height),
                                                    bitsPerSample: 8,
                                                    samplesPerPixel: 4,
                                                    hasAlpha: true,
                                                    isPlanar: false,
                                                    colorSpaceName: NSColorSpaceName.calibratedRGB,
                                                    bytesPerRow: 0,
                                                    bitsPerPixel: 0)!
        outImage.addRepresentation(rep)
        outImage.lockFocus()
        background.draw(in: imageRect)
        description.draw(in: textRect1, withAttributes: textFontAttributes1)
        auxiliaryText.draw(in: textRect2, withAttributes: textFontAttributes2)
        outImage.unlockFocus()
        return outImage
    }
}

extension C64Key: Equatable {
    static func ==(lhs: C64Key, rhs: C64Key) -> Bool {
        return lhs.row == rhs.row && lhs.col == rhs.col
    }
}

extension C64Key: Hashable {
    var hashValue: Int {
        return col + (8 * row)
    }
}

extension C64Key {
    
    // First row
    static let delete       = C64Key.init(row: 0, col: 0)
    static let ret          = C64Key.init(row: 0, col: 1)
    static let curLeftRight = C64Key.init(row: 0, col: 2)
    static let F7F8         = C64Key.init(row: 0, col: 3)
    static let F1F2         = C64Key.init(row: 0, col: 4)
    static let F3F4         = C64Key.init(row: 0, col: 5)
    static let F5F6         = C64Key.init(row: 0, col: 6)
    static let curUpDown    = C64Key.init(row: 0, col: 7)
    
    // Second row
    static let digit3       = C64Key.init(row: 1, col: 0)
    static let W            = C64Key.init(row: 1, col: 1)
    static let A            = C64Key.init(row: 1, col: 2)
    static let digit4       = C64Key.init(row: 1, col: 3)
    static let Z            = C64Key.init(row: 1, col: 4)
    static let S            = C64Key.init(row: 1, col: 5)
    static let E            = C64Key.init(row: 1, col: 6)
    static let shift        = C64Key.init(row: 1, col: 7)
    
    // Third row
    static let digit5       = C64Key.init(row: 2, col: 0)
    static let R            = C64Key.init(row: 2, col: 1)
    static let D            = C64Key.init(row: 2, col: 2)
    static let digit6       = C64Key.init(row: 2, col: 3)
    static let C            = C64Key.init(row: 2, col: 4)
    static let F            = C64Key.init(row: 2, col: 5)
    static let T            = C64Key.init(row: 2, col: 6)
    static let X            = C64Key.init(row: 2, col: 7)
    
    // Fourth row
    static let digit7       = C64Key.init(row: 3, col: 0)
    static let Y            = C64Key.init(row: 3, col: 1)
    static let G            = C64Key.init(row: 3, col: 2)
    static let digit8       = C64Key.init(row: 3, col: 3)
    static let B            = C64Key.init(row: 3, col: 4)
    static let H            = C64Key.init(row: 3, col: 5)
    static let U            = C64Key.init(row: 3, col: 6)
    static let V            = C64Key.init(row: 3, col: 7)
    
    // Fifth row
    static let digit9       = C64Key.init(row: 4, col: 0)
    static let I            = C64Key.init(row: 4, col: 1)
    static let J            = C64Key.init(row: 4, col: 2)
    static let digit0       = C64Key.init(row: 4, col: 3)
    static let M            = C64Key.init(row: 4, col: 4)
    static let K            = C64Key.init(row: 4, col: 5)
    static let O            = C64Key.init(row: 4, col: 6)
    static let N            = C64Key.init(row: 4, col: 7)
    
    // Sixth row
    static let plus         = C64Key.init(row: 5, col: 0)
    static let P            = C64Key.init(row: 5, col: 1)
    static let L            = C64Key.init(row: 5, col: 2)
    static let minus        = C64Key.init(row: 5, col: 3)
    static let period       = C64Key.init(row: 5, col: 4)
    static let colon        = C64Key.init(row: 5, col: 5)
    static let at           = C64Key.init(row: 5, col: 6)
    static let comma        = C64Key.init(row: 5, col: 7)
    
    // Seventh row
    static let pound        = C64Key.init(row: 6, col: 0)
    static let asterisk     = C64Key.init(row: 6, col: 1)
    static let semicolon    = C64Key.init(row: 6, col: 2)
    static let home         = C64Key.init(row: 6, col: 3)
    static let rightShift   = C64Key.init(row: 6, col: 4)
    static let equal        = C64Key.init(row: 6, col: 5)
    static let upArrow      = C64Key.init(row: 6, col: 6)
    static let slash        = C64Key.init(row: 6, col: 7)
    
    
    // Eights row
    static let digit1       = C64Key.init(row: 7, col: 0)
    static let leftArrow    = C64Key.init(row: 7, col: 1)
    static let control      = C64Key.init(row: 7, col: 2)
    static let digit2       = C64Key.init(row: 7, col: 3)
    static let space        = C64Key.init(row: 7, col: 4) // \u{2423}
    static let commodore    = C64Key.init(row: 7, col: 5)
    static let Q            = C64Key.init(row: 7, col: 6)
    static let runStop      = C64Key.init(row: 7, col: 7)
    
    // Restore key
    static let restore      = C64Key.init(row: 9, col: 9, characters: "")

    
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

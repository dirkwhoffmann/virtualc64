//
//  C64Key.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 18.02.18.
//

import Foundation

/// The C64Key structure represents a physical keys on the C64 keyboard.
/// The key is specified by its row and coloumn position in the C64 keyboard matrix.
struct C64Key : Codable {
    
    // Row index
    var row = 0
    
    // Column index
    var col = 0
    
    // Textual description of this key
    var description: String?
    
    init(row: Int, col: Int, characters: String?) {
        
        self.row = row
        self.col = col
        self.description = characters
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
    static let delete       = C64Key.init(row: 0, col: 0, characters: "DEL")
    static let ret          = C64Key.init(row: 0, col: 1, characters: "\u{21b5}")
    static let curLeftRight = C64Key.init(row: 0, col: 2, characters: "\u{21c6}")
    static let F7F8         = C64Key.init(row: 0, col: 3, characters: "F7")
    static let F1F2         = C64Key.init(row: 0, col: 4, characters: "F1")
    static let F3F4         = C64Key.init(row: 0, col: 5, characters: "F3")
    static let F5F6         = C64Key.init(row: 0, col: 6, characters: "F5")
    static let curUpDown    = C64Key.init(row: 0, col: 7, characters: "\u{21c5}")
    
    // Second row
    static let digit3       = C64Key.init(row: 1, col: 0, characters: "3")
    static let W            = C64Key.init(row: 1, col: 1, characters: "W")
    static let A            = C64Key.init(row: 1, col: 2, characters: "A")
    static let digit4       = C64Key.init(row: 1, col: 3, characters: "4")
    static let Z            = C64Key.init(row: 1, col: 4, characters: "Z")
    static let S            = C64Key.init(row: 1, col: 5, characters: "S")
    static let E            = C64Key.init(row: 1, col: 6, characters: "E")
    static let shift        = C64Key.init(row: 1, col: 7, characters: "SHIFT")
    
    // Third row
    static let digit5       = C64Key.init(row: 2, col: 0, characters: "5")
    static let R            = C64Key.init(row: 2, col: 1, characters: "R")
    static let D            = C64Key.init(row: 2, col: 2, characters: "D")
    static let digit6       = C64Key.init(row: 2, col: 3, characters: "6")
    static let C            = C64Key.init(row: 2, col: 4, characters: "C")
    static let F            = C64Key.init(row: 2, col: 5, characters: "F")
    static let T            = C64Key.init(row: 2, col: 6, characters: "T")
    static let X            = C64Key.init(row: 2, col: 7, characters: "X")
    
    // Fourth row
    static let digit7       = C64Key.init(row: 3, col: 0, characters: "7")
    static let Y            = C64Key.init(row: 3, col: 1, characters: "Y")
    static let G            = C64Key.init(row: 3, col: 2, characters: "G")
    static let digit8       = C64Key.init(row: 3, col: 3, characters: "8")
    static let B            = C64Key.init(row: 3, col: 4, characters: "B")
    static let H            = C64Key.init(row: 3, col: 5, characters: "H")
    static let U            = C64Key.init(row: 3, col: 6, characters: "U")
    static let V            = C64Key.init(row: 3, col: 7, characters: "V")
    
    // Fifth row
    static let digit9       = C64Key.init(row: 4, col: 0, characters: "9")
    static let I            = C64Key.init(row: 4, col: 1, characters: "I")
    static let J            = C64Key.init(row: 4, col: 2, characters: "J")
    static let digit0       = C64Key.init(row: 4, col: 3, characters: "0")
    static let M            = C64Key.init(row: 4, col: 4, characters: "M")
    static let K            = C64Key.init(row: 4, col: 5, characters: "K")
    static let O            = C64Key.init(row: 4, col: 6, characters: "O")
    static let N            = C64Key.init(row: 4, col: 7, characters: "N")
    
    // Sixth row
    static let plus         = C64Key.init(row: 5, col: 0, characters: "+")
    static let P            = C64Key.init(row: 5, col: 1, characters: "P")
    static let L            = C64Key.init(row: 5, col: 2, characters: "L")
    static let minus        = C64Key.init(row: 5, col: 3, characters: "-")
    static let period       = C64Key.init(row: 5, col: 4, characters: ".")
    static let colon        = C64Key.init(row: 5, col: 5, characters: ":")
    static let at           = C64Key.init(row: 5, col: 6, characters: "@")
    static let comma        = C64Key.init(row: 5, col: 7, characters: ",")
    
    // Seventh row
    static let pound        = C64Key.init(row: 6, col: 0, characters: "\u{00a3}")
    static let asterisk     = C64Key.init(row: 6, col: 1, characters: "*")
    static let semicolon    = C64Key.init(row: 6, col: 2, characters: ";")
    static let home         = C64Key.init(row: 6, col: 3, characters: "HOME")
    static let rightShift   = C64Key.init(row: 6, col: 4, characters: "SHIFT")
    static let equal        = C64Key.init(row: 6, col: 5, characters: "=")
    static let upArrow      = C64Key.init(row: 6, col: 6, characters: "\u{2191}")
    static let slash        = C64Key.init(row: 6, col: 7, characters: "/")
    
    
    // Eights row
    static let digit1       = C64Key.init(row: 6, col: 0, characters: "1")
    static let leftArrow    = C64Key.init(row: 6, col: 1, characters: "\u{2190}")
    static let control      = C64Key.init(row: 6, col: 2, characters: "CONTROL")
    static let digit2       = C64Key.init(row: 6, col: 3, characters: "2")
    static let space        = C64Key.init(row: 6, col: 4, characters: "\u{23b5}") // \u{2423}
    static let commodore    = C64Key.init(row: 6, col: 5, characters: "C=")
    static let Q            = C64Key.init(row: 6, col: 6, characters: "Q")
    static let runStop      = C64Key.init(row: 6, col: 7, characters: "RUNSTOP")
    
    // Restore key
    static let restore      = C64Key.init(row: 9, col: 9, characters: "RESTORE" )
}

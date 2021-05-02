// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

//
// Logging / Debugging
// 

public func track(_ message: String = "",
                  path: String = #file, function: String = #function, line: Int = #line ) {
    
    if let file = URL.init(string: path)?.deletingPathExtension().lastPathComponent {
        if message == "" {
            print("\(file).\(line)::\(function)")
        } else {
            print("\(file).\(line)::\(function): \(message)")
        }
    }
}

//
// NSFont
//

extension NSFont {
 
    static func monospaced(ofSize fontSize: CGFloat, weight: Weight) -> NSFont {
        
        if #available(macOS 10.15, *) {
            return NSFont.monospacedSystemFont(ofSize: fontSize, weight: weight)
        } else {
            return NSFont.systemFont(ofSize: fontSize)
        }
    }
}

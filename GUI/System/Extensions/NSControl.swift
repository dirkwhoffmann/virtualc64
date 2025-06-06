// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension NSControl {
    
    func assignFormatter(_ formatter: Formatter) {
        
        abortEditing()
        self.formatter = formatter
        self.alignment = .right
        needsDisplay = true
    }
}

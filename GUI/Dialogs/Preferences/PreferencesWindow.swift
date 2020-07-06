// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class PreferencesWindow: NSWindow {
    
    func respondToEvents() {
        DispatchQueue.main.async {
            self.makeFirstResponder(self)
        }
    }
    
    override func awakeFromNib() {
        
        track()
        respondToEvents()
    }
    
    override func keyDown(with event: NSEvent) {
        
        let controller = delegate as? UserDialogController
        controller?.keyDown(with: MacKey.init(with: event))
    }
    
    override func flagsChanged(with event: NSEvent) {
        
        track()
        let controller = delegate as? UserDialogController
        if event.modifierFlags.contains(.shift) && event.keyCode == 56 {
            controller?.keyDown(with: MacKey.leftShift)
        } else if event.modifierFlags.contains(.shift) && event.keyCode == 60 {
            controller?.keyDown(with: MacKey.rightShift)
        } else if event.modifierFlags.contains(.control) {
            controller?.keyDown(with: MacKey.control)
        } else if event.modifierFlags.contains(.option) {
            controller?.keyDown(with: MacKey.option)
        }
    }
}

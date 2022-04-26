// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import Carbon.HIToolbox

class PreferencesWindow: NSWindow {
    
    /*
    func respondToEvents() {
        DispatchQueue.main.async {
            self.makeFirstResponder(self)
        }
    }
    */
    
    override func keyDown(with event: NSEvent) {
                
        if let parent = delegate as? PreferencesController {
            if parent.keyDown(with: MacKey(event: event)) {
                return
            }
        }
        
        // The controller wasn't interested. Process it as usual
        interpretKeyEvents([event])
    }
    
    override func flagsChanged(with event: NSEvent) {
                
        let controller = delegate as? PreferencesController
        
        switch Int(event.keyCode) {
            
        case kVK_Shift where event.modifierFlags.contains(.shift):
            controller?.keyDown(with: MacKey.shift)
        case kVK_RightShift where event.modifierFlags.contains(.shift):
            controller?.keyDown(with: MacKey.rightShift)
            
        /*
        case kVK_Control where event.modifierFlags.contains(.control):
            controller?.keyDown(with: MacKey.control)
        case kVK_RightControl where event.modifierFlags.contains(.control):
            controller?.keyDown(with: MacKey.rightControl)
        */
        
        case kVK_Option where event.modifierFlags.contains(.option):
            controller?.keyDown(with: MacKey.option)
        case kVK_RightOption where event.modifierFlags.contains(.option):
            controller?.keyDown(with: MacKey.rightOption)
            
        default:
            break
        }
    }
}

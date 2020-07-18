// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension PreferencesController {
    
    func refreshMediaTab() {
        
        track()
        
        func refresh(_ s: String, _ p: NSPopUpButton, _ b: NSButton, _ t: NSTextField) {
            
            let mountAction = pref.mountAction[s]?.rawValue ?? 0
            let autoType = pref.autoType[s] ?? false
            let autoText = pref.autoText[s] ?? ""
            p.selectItem(withTag: mountAction)
            b.isEnabled = mountAction != 0
            b.state = autoType ? .on : .off
            t.isEnabled = mountAction != 0 && autoType
            t.stringValue = autoText
        }
        refresh("D64", medD64Popup, medD64AutoTypeButton, medD64AutoTypeText)
        refresh("PRG", medPrgPopup, medPrgAutoTypeButton, medPrgAutoTypeText)
        refresh("T64", medT64Popup, medT64AutoTypeButton, medT64AutoTypeText)
        refresh("TAP", medTapPopup, medTapAutoTypeButton, medTapAutoTypeText)
        refresh("CRT", medCrtPopup, medCrtAutoTypeButton, medCrtAutoTypeText)
    }
}

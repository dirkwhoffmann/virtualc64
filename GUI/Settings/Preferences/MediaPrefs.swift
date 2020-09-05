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
    
    private func mediaFileType(_ tag: Int) -> String? {
        switch tag {
        case 0: return "D64"
        case 1: return "PRG"
        case 2: return "T64"
        case 3: return "TAP"
        case 4: return "CRT"
        default: return nil
        }
    }
    
    @IBAction func medMountAction(_ sender: NSPopUpButton!) {
        
        if let fileType = mediaFileType(sender.tag) {
            let action = AutoMountAction(rawValue: sender.selectedTag())
            pref.mountAction[fileType] = action
            refresh()
        }
    }
    
    @IBAction func medAutoTypeAction(_ sender: NSButton!) {
        
        if let fileType = mediaFileType(sender.tag) {
            pref.autoType[fileType] = (sender.intValue == 0) ? false : true
            refresh()
        }
    }
    
    @IBAction func medAutoTextAction(_ sender: NSTextField!) {
        
        if let fileType = mediaFileType(sender.tag) {
            pref.autoText[fileType] = sender.stringValue
            refresh()
        }
    }
    
    @IBAction func medPresetAction(_ sender: NSPopUpButton!) {
        
        track()
        assert(sender.selectedTag() == 0)
        
        UserDefaults.resetMediaDefaults()
        pref.loadMediaUserDefaults()
        refresh()
    }
}

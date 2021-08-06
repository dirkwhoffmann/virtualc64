// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class KeyViewItem: NSCollectionViewItem {
    
    override func viewDidLoad() {
        
        super.viewDidLoad()
        view.wantsLayer = true
    }
}

extension PreferencesController {
        
    func refreshKeyboardTab() {
            
        track()
        
        // Set up reference and image caches
        if keyView[0] == nil {
            for nr in 0 ... 65 {
                keyImage[nr] = C64Key.lookupKeycap(for: nr, modifier: [])?.image
                pressedKeyImage[nr] = keyImage[nr]?.copy() as? NSImage
                pressedKeyImage[nr]?.pressed()
                mappedKeyImage[nr] = keyImage[nr]?.copy() as? NSImage
                mappedKeyImage[nr]?.mapped()
                keyView[nr] = window!.contentView!.viewWithTag(nr + 100) as? RecordButton
                keyView[nr]!.image = keyImage[nr]
            }
        }
        
        keyMappingPopup.selectItem(withTag: pref.mapKeysByPosition ? 1 : 0)
    
        if pref.mapKeysByPosition {
            
            keyMappingPopup.selectItem(withTag: 1)
            keyMappingText.stringValue = "In positonal assignment mode, the Mac keys are assigned to the C64 keys according to the following mapping table:"
            keyBox.isHidden = false
            
        } else {
            
            keyMappingPopup.selectItem(withTag: 0)
            keyMappingText.stringValue = "In symbolic assignment mode, the Mac keys are assigned to C64 keys according to the symbols they represent."
            keyBox.isHidden = true
        }
        
        var reverseMap: [C64Key: MacKey] = [:]
        for (macKey, c64Key) in pref.keyMap {
            reverseMap[c64Key] = macKey
        }

        // Update images
        for nr in 0 ... 65 {
            
            let c64Key = C64Key(nr)
            
            if c64Key == selectedKey {
                keyView[nr]!.image = pressedKeyImage[nr]
            } else if reverseMap[c64Key] != nil {
                keyView[nr]!.image = mappedKeyImage[nr]
            } else {
                keyView[nr]!.image = keyImage[nr]
            }
        }

        // Update key description
        if selectedKey != nil {

            keyMapText.isHidden = false
            if let macKey = reverseMap[selectedKey!] {
                keyMapText.stringValue = "Mapped to Mac key \(macKey.keyCode)"
                if macKey.stringValue != "" {
                    keyMapText.stringValue.append(" ('\(macKey.stringValue)')")
                }
            } else {
                keyMapText.stringValue = "Hit a key on the Mac keyboard to map this key"
            }
            
        } else {
            
            keyMapText.isHidden = true
        }
    }
        
    func selectKeyboardTab() {

        track()
        refreshKeyboardTab()
    }
    
    func selectKey(nr: Int) {
        
        track()

        let oldKey = selectedKey
        
        // Deselect the old key (if any)
        if let nr = selectedKey?.nr {
            keyView[nr]?.image = keyImage[nr]
            selectedKey = nil
        }

        // Select the new key if it doesn't match the old one
        if oldKey?.nr != nr {
            selectedKey = C64Key(nr)
            keyView[selectedKey!.nr]?.image = pressedKeyImage[selectedKey!.nr]
        }
        
        refresh()
    }
    
    func trashKey(nr: Int) {
        
        track()
        
        // Remove old key assignment (if any)
        for (macKey, key) in pref.keyMap where key.nr == nr {
            pref.keyMap[macKey] = nil
        }
        
        refresh()
    }
    
    func mapSelectedKey(to macKey: MacKey) -> Bool {
                
        track()
        
        // Only proceed if a key has been selected
        if selectedKey == nil { return false }
        
        track()
        
        // Check for the ESC key
        if macKey == MacKey.escape {
            cancelAction(self)
            return false
        }
        
        // Remove old key assignment (if any)
        for (macKey, key) in pref.keyMap where key == selectedKey {
            pref.keyMap[macKey] = nil
        }
        
        // Assign new key
        pref.keyMap[macKey] = selectedKey
        
        refresh()
        return true
    }
    
    override func mouseDown(with event: NSEvent) {
        
        track()
        selectedKey = nil
        refresh()
    }

    @IBAction func mapKeyMappingAction(_ sender: NSPopUpButton!) {
        
        let value = (sender.selectedTag() == 1) ? true : false
        pref.mapKeysByPosition = value
        refresh()
    }

    @IBAction func kbPresetAction(_ sender: NSPopUpButton!) {
        
        track()
        assert(sender.selectedTag() == 0)

        UserDefaults.resetKeyMapUserDefaults()
        pref.loadKeyboardUserDefaults()
        refresh()
    }
}

//
// Subclass of NSButton for all record buttons
//

class RecordButton: NSButton {
    
    override func mouseDown(with event: NSEvent) {
        
        if let controller = window?.delegate as? PreferencesController {
                        
            controller.selectKey(nr: self.tag - 100)
        }
    }
    
    override func rightMouseDown(with event: NSEvent) {
        
        if let controller = window?.delegate as? PreferencesController {

            controller.trashKey(nr: self.tag - 100)
        }
    }
}

//
// This file is part of VirtualC64 - A cycle accurate Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

class KeyViewItem: NSCollectionViewItem {
    
    override func viewDidLoad() {
        
        super.viewDidLoad()
        view.wantsLayer = true
    }
}

extension PreferencesController {
    
    func awakeKeymapPrefsFromNib() {
     
        keyMap = parent.keyboardcontroller.keyMap
        updateImages()
    }
    
    func refreshKeyboardTab() {
        
        track()
        
        if selectedKey == nil {
            icon.isHidden = true
            // info1.isHidden = true
            // info2.isHidden = true
        } else {
            icon.isHidden = false
            // info1.isHidden = false
            // info2.isHidden = false
        }
        
        updateImages()
        
        devCancelButton.isHidden = hideCancelButton
        devOkButton.title = okButtonTitle
    }
    
    func updateImages() {
        
        // Clear old images
        for row in 0...7 {
            for col in 0...7 {
                keyImage[row][col] = nil
            }
        }
        
        // Create images for all mapped keys
        for (macKey,c64Key) in keyMap {
            let keyCodeString = String.init(format: "%02X", macKey.keyCode) as NSString
            keyImage[c64Key.row][c64Key.col] = c64Key.image(keyCode: keyCodeString)
        }
        
        // Create images for all unmapped keys
        for row in 0...7 {
            for col in 0...7 {
                if keyImage[row][col] == nil {
                    keyImage[row][col] = C64Key.init( (row,col) ).image(keyCode: "")
                }
            }
        }
        
        keyMatrix.reloadData()
    }
    
    func mapKeyDown(with macKey: MacKey) {
        
        track()
        
        // Check for ESC key
        if macKey == MacKey.escape {
            cancelAction(self)
            return
        }
        
        // Remove old key assignment (if any)
        for (macKey, key) in keyMap {
            if key == selectedKey {
                keyMap[macKey] = nil
            }
        }
        
        // Assign new key
        keyMap[macKey] = selectedKey
        
        // Update  view
        icon.image = selectedKey?.image(keyCode: macKey.keyCodeStr as NSString)
        refresh()
    }
    
    @IBAction func mapFactorySettingsAction(_ sender: Any!) {
        
        parent.keyboardcontroller.keyMap = Defaults.keyMap
        refresh()
    }
}

//
// NSCollectionView data source and delegate
//

extension PreferencesController : NSCollectionViewDataSource {
    
    func numberOfSections(in collectionView: NSCollectionView) -> Int {
        
        return 8
    }
    
    func collectionView(_ collectionView: NSCollectionView, numberOfItemsInSection section: Int) -> Int {
        
        return 8
    }
    
    func collectionView(_ itemForRepresentedObjectAtcollectionView: NSCollectionView, itemForRepresentedObjectAt indexPath: IndexPath) -> NSCollectionViewItem {
        
        let id = NSUserInterfaceItemIdentifier(rawValue: "KeyViewItem")
        let item = keyMatrix.makeItem(withIdentifier: id, for: indexPath)
        guard let keyViewItem = item as? KeyViewItem else {
            return item
        }
        
        let row = indexPath.section
        let col = indexPath.item
        keyViewItem.imageView?.image = keyImage[row][col]
        return keyViewItem
    }
}

extension PreferencesController : NSCollectionViewDelegate {
    
    func collectionView(_ collectionView: NSCollectionView, didSelectItemsAt indexPaths: Set<IndexPath>) {
        
        guard let indexPath = indexPaths.first else { return }
        
        let row = indexPath.section
        let col = indexPath.item
        selectedKey = C64Key( (row,col) )
        icon.image = selectedKey?.image()
        refresh()
        
        // Hey, CollectionView, why do you steal first responder status? Give it back!
        (window as! PreferencesWindow).respondToEvents()
    }
}

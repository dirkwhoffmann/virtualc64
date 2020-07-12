// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class KeyViewItem: NSCollectionViewItem {
    
    override func viewDidLoad() {
        
        super.viewDidLoad()
        view.wantsLayer = true
    }
}

extension ConfigurationController {
    
    /*
    func awakeKeymapPrefsFromNib() {
     
        updateImages()
    }
    */
    
    func refreshKeyboardTab() {
    
        // guard let controller = myController else { return }
        guard let kbController = myController?.keyboard else { return }
        guard let c64 = proxy else { return }
        
        keyMappingPopup.selectItem(withTag: kbController.mapKeysByPosition ? 1 : 0)
    
        if kbController.mapKeysByPosition {
            
            keyMappingPopup.selectItem(withTag: 1)
            info.stringValue = "In positonal assignment mode, the Mac keys are assigned to the C64 keys according to the following mapping table:"
            keyMatrixScrollView.isHidden = false
            updateImages()
            
        } else {
            
            keyMappingPopup.selectItem(withTag: 0)
            info.stringValue = "In symbolic assignment mode, the Mac keys are assigned to C64 keys according to the symbols they represent."
            keyMatrixScrollView.isHidden = true
        }
        
        keyOkButton.title = c64.isReady() ? "OK" : "Quit"
    }
    
    func updateImages() {
        
        guard let kbController = myController?.keyboard else { return }
        let keyMap = kbController.keyMap
        
        // Create labels
        var labels = Array(repeating: Array(repeating: "", count: 8), count: 8)
        for (macKey, c64Key) in keyMap {
            labels[c64Key.row][c64Key.col] = String.init(format: "%02X", macKey.keyCode)
        }
        
        // Create labeled images
        for row in 0...7 {
            for col in 0...7 {
                let c64key = C64Key.init( (row, col) )
                let selected = (c64key == selectedKey)
                keyImage[row][col] = c64key.image(keyCode: labels[row][col], red: selected)
            }
        }

        keyMatrixCollectionView.reloadData()
    }
    
    func mapKeyDown(with macKey: MacKey) {
        
        guard let kbController = myController?.keyboard else { return }
        let keyMap = kbController.keyMap
        
        // Check for ESC key
        if macKey == MacKey.escape {
            cancelAction(self)
            return
        }
        
        // Remove old key assignment (if any)
        for (macKey, key) in keyMap where key == selectedKey {
            kbController.keyMap[macKey] = nil
        }
        
        // Assign new key
        kbController.keyMap[macKey] = selectedKey
        
        // Update  view
        selectedKey = nil
        refresh()
    }
    
    @IBAction func mapKeyMappingAction(_ sender: NSPopUpButton!) {
        
        let value = (sender.selectedTag() == 1) ? true : false
        myController?.keyboard.mapKeysByPosition = value
        refresh()
    }
    
    @IBAction func kbPresetAction(_ sender: NSPopUpButton!) {
        
        track()
        assert(sender.selectedTag() == 0)

        UserDefaults.resetKeyMapUserDefaults()
        refresh()
    }
    
    @IBAction func kbDefaultsAction(_ sender: NSButton!) {
        
        track()
    }
}

//
// NSCollectionView data source and delegate
//

extension ConfigurationController: NSCollectionViewDataSource {
    
    func numberOfSections(in collectionView: NSCollectionView) -> Int {
        
        return 8
    }
    
    func collectionView(_ collectionView: NSCollectionView,
                        numberOfItemsInSection section: Int) -> Int {
        
        return 8
    }
    
    func collectionView(_ itemForRepresentedObjectAtcollectionView: NSCollectionView,
                        itemForRepresentedObjectAt indexPath: IndexPath) -> NSCollectionViewItem {
        
        let id = NSUserInterfaceItemIdentifier(rawValue: "KeyViewItem")
        let item = keyMatrixCollectionView.makeItem(withIdentifier: id, for: indexPath)
        guard let keyViewItem = item as? KeyViewItem else {
            return item
        }
        
        let row = indexPath.section
        let col = indexPath.item
        keyViewItem.imageView?.image = keyImage[row][col]
        return keyViewItem
    }
}

extension ConfigurationController: NSCollectionViewDelegate {
    
    func collectionView(_ collectionView: NSCollectionView,
                        didSelectItemsAt indexPaths: Set<IndexPath>) {
        
        if let indexPath = indexPaths.first {
            
            selectedKey = C64Key( (indexPath.section, indexPath.item) )
            refresh()
            
            // Make sure that we can receive keyboard events
            (window as? PreferencesWindow)?.respondToEvents()
        }
    }
}

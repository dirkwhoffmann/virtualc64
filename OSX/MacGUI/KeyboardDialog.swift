//
//  KeyboardDialogController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 19.02.18.
//

import Foundation

class KeyboardDialog : UserDialogController {

    // Outlets
    @IBOutlet weak var keyCode: NSTextField!
    @IBOutlet weak var keyChar: NSTextField!
    @IBOutlet weak var keyMatrix: NSCollectionView!

    // Double array of key images, indexed by their row and column number
    var keyImage = Array(repeating: Array(repeating: nil as NSImage?, count: 8), count: 8)
    
    // Keymap that is going to be customized
    var keyMap: [MacKey:C64Key] = [:]
    
    override public func awakeFromNib() {
        
        track()
        
        // Get current KeyMap from KeyboardController
        keyMap = parent.keyboardcontroller.keyMap
        updateImages()
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
            keyImage[c64Key.row][c64Key.col] = c64Key.image(auxiliaryText: keyCodeString)
        }
        
        // Create images for unmapped keys
        for row in 0...7 {
            for col in 0...7 {
                if keyImage[row][col] == nil {
                    keyImage[row][col] = C64Key.init(row: row, col: col).image()
                }
            }
        }
    }

    //
    // Keyboard events
    //

    
    //
    // Action methods
    //

    @IBAction func factorySettingskAction(_ sender: Any!) {
        
        // Revert to standard map from KeyboardController
        keyMap = KeyboardController.standardKeyMap
        updateImages()
    }
    
    @IBAction func okAction(_ sender: Any!) {
        
        // Write customized KeyMap back to KeyboardController
        parent.keyboardcontroller.keyMap = keyMap
        hideSheet()
    }
}

//
// NSCollectionView delegate and data source
//

extension KeyboardDialog : NSCollectionViewDataSource {
    
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
        
        track("row = \(row) col = \(col)")
        keyViewItem.imageView?.image = keyImage[row][col]
        return keyViewItem
    }
}





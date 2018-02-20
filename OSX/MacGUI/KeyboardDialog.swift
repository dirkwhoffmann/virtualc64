//
//  KeyboardDialogController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 19.02.18.
//

import Foundation

class KeyboardDialogWindow : NSWindow {
    
    @IBOutlet weak var controller: KeyboardDialog!
    
    // override var acceptsFirstResponder: Bool { return true }
    
    func respondToEvents() {
        DispatchQueue.main.async {
            self.makeFirstResponder(self)
        }
    }
    
    override func awakeFromNib() {
        
        // Make window receive keyboard events
        respondToEvents()
    }

    override func keyDown(with event: NSEvent) {
        
        let controller = delegate as! KeyboardDialog
        controller.keyDown(with: MacKey.init(keyCode: event.keyCode))
    }
    
    override func flagsChanged(with event: NSEvent) {
        
        let controller = delegate as! KeyboardDialog
        if event.modifierFlags.contains(.shift) {
            controller.keyDown(with: MacKey.shift)
        } else if event.modifierFlags.contains(.control) {
            controller.keyDown(with: MacKey.control)
        } else if event.modifierFlags.contains(.option) {
            controller.keyDown(with: MacKey.option)
        }
    }
}

class KeyboardDialog : UserDialogController {

    // Outlets
    @IBOutlet weak var info1: NSTextField!
    @IBOutlet weak var info2: NSTextField!
    @IBOutlet weak var icon: NSImageView!
    @IBOutlet weak var keyMatrix: NSCollectionView!

    // Double array of key images, indexed by their row and column number
    var keyImage = Array(repeating: Array(repeating: nil as NSImage?, count: 8), count: 8)
    
    // Keymap that is going to be customized
    var keyMap: [MacKey:C64Key] = [:]
    
    // Recorded key
    var recordedKey: MacKey? = nil
    
    override var acceptsFirstResponder: Bool { return true }
    
    override public func awakeFromNib() {
        
        keyMap = parent.keyboardcontroller.keyMap
        update()
    }
    
    func update() {
        
        if recordedKey == nil {
            icon.isHidden = true
            info1.isHidden = true
            info2.isHidden = true
        } else {
            icon.isHidden = false
            info1.isHidden = false
            info2.isHidden = false
        }
        
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
        
        keyMatrix.reloadData()
    }

    //
    // Keyboard events
    //

    func keyDown(with macKey: MacKey) {
        
        track()
        recordedKey = macKey
        let plainKey = C64Key(row: 0, col: 0, characters: "")
        icon.image = plainKey.image(auxiliaryText: macKey.keyCodeStr as NSString)
        update()
    }
        
    //
    // Action methods
    //

    @IBAction func factorySettingsAction(_ sender: Any!) {
        
        // Revert to standard map from KeyboardController
        keyMap = KeyboardController.standardKeyMap
        update()
    }
    
    @IBAction func okAction(_ sender: Any!) {
        
        // Write customized KeyMap back to KeyboardController
        parent.keyboardcontroller.keyMap = keyMap
        hideSheet()
    }
}

//
// NSCollectionView data source and delegate
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
        keyViewItem.imageView?.image = keyImage[row][col]
        return keyViewItem
    }
}

extension KeyboardDialog : NSCollectionViewDelegate {

    func collectionView(_ collectionView: NSCollectionView, didSelectItemsAt indexPaths: Set<IndexPath>) {
        
        guard let indexPath = indexPaths.first else { return }
        
        if recordedKey != nil {
            
            let row = indexPath.section
            let col = indexPath.item
            let c64Key = C64Key(row: row, col: col)
            
            // Remove old key assignment (if any)
            for (macKey, key) in keyMap {
                if key == c64Key {
                    keyMap[macKey] = nil
                }
            }
            
            // Assign new key
            keyMap[recordedKey!] = C64Key(row: row, col: col)
            
            // Update view
            recordedKey = nil
            update()

            // Hey, CollectionView, why do you steal the first responder status. Give it back!
            (window as! KeyboardDialogWindow).respondToEvents()
        }
    }
}





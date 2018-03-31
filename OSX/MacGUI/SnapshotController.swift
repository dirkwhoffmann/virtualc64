//
//  SnapshotController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 31.03.18.
//

import Foundation

class SnapshotViewItem: NSCollectionViewItem {
    
    override func viewDidLoad() {
        
        super.viewDidLoad()
        view.wantsLayer = true
    }
}

class SnapshotDialog : UserDialogController {
    
    // Outlets
    @IBOutlet weak var autoSnapshots: NSCollectionView!
    @IBOutlet weak var userSnapshots: NSCollectionView!

    override public func awakeFromNib() {
        
        update()
    }
    
    func update() {
        autoSnapshots.reloadData()
        userSnapshots.reloadData()
    }
    
    @IBAction func okAction(_ sender: Any!) {
        
        // Restore selected snapshot
        track("TODO: Restore selected snapshot")
        
        // parent. ...
        hideSheet()
    }
}

//
// NSCollectionView data source and delegate
//

extension SnapshotDialog : NSCollectionViewDataSource {
    
    func numberOfSections(in collectionView: NSCollectionView) -> Int {
        
        return 1
    }
    
    func collectionView(_ collectionView: NSCollectionView, numberOfItemsInSection section: Int) -> Int {
        
        return 8 // TODO get info from Emulator
    }
    
    func collectionView(_ itemForRepresentedObjectAtcollectionView: NSCollectionView, itemForRepresentedObjectAt indexPath: IndexPath) -> NSCollectionViewItem {
    
        let id = NSUserInterfaceItemIdentifier(rawValue: "SnapshotViewItem")
        let item = autoSnapshots.makeItem(withIdentifier: id, for: indexPath)
    
        guard let snapshotViewItem = item as? SnapshotViewItem else {
            return item
        }
        
        snapshotViewItem.imageView?.image = NSImage.init(named: NSImage.Name(rawValue: "metal.png"))
        snapshotViewItem.textField?.stringValue = "23 seconds ago"
        return snapshotViewItem
    }
}

extension SnapshotDialog : NSCollectionViewDelegate {
    
    func collectionView(_ collectionView: NSCollectionView, didSelectItemsAt indexPaths: Set<IndexPath>) {
        
        guard let indexPath = indexPaths.first else { return }
        
        let row = indexPath.section
        let col = indexPath.item
        
        track("selected: row = \(row) col = \(col)")
    }
}


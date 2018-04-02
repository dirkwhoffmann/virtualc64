//
//  SnapshotController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 31.03.18.
//

import Foundation

class SnapshotTableCellView: NSTableCellView {
    
    @IBOutlet weak var preview: NSImageView!
    @IBOutlet weak var text: NSTextField!
    @IBOutlet weak var subText: NSTextField!
    @IBOutlet weak var delete: NSButton!
    // var slot = 0
}

class SnapshotDialog : UserDialogController  {
    
    // Outlets
    
    @IBOutlet weak var autoTableView: NSTableView!
    @IBOutlet weak var userTableView: NSTableView!

    var now: Date = Date()
    
    // Auto-saved snapshot cache
    var numAutoSnapshots = -1
    var autoSnapshotImage: [Int:NSImage] = [:]
    var autoTimeStamp: [Int:String] = [:]
    var autoTimeDiff: [Int:String] = [:]
    
    // User-saved snapshot cache
    var numUserSnapshots = -1
    var userSnapshotImage: [Int:NSImage] = [:]
    var userTimeStamp: [Int:String] = [:]
    var userTimeDiff: [Int:String] = [:]

    override public func awakeFromNib() {
        
        if numAutoSnapshots == -1 {
            
            // Disable auto snapshot saving while dialog is open
            c64.setAutoSaveSnapshots(false)
            
            // Setup snapshot caches
            reloadAutoSnapshotCache()
            reloadUserSnapshotCache()
        }
    }
    
    func timeInfo(timeStamp: TimeInterval) -> String {
        
        let formatter = DateFormatter()
        formatter.timeZone = TimeZone.current
        formatter.dateFormat = "HH:mm:ss" // "yyyy-MM-dd HH:mm"
        
        return formatter.string(from: Date(timeIntervalSince1970: timeStamp))
    }
    
    func timeDiffInfo(timeStamp: TimeInterval) -> String {
        
        var diff = Int(round(now.timeIntervalSince1970 - Double(timeStamp)))
        let min = diff / 60;
        let hrs = diff / 3600;
        if (diff) < 60 {
            let s = (diff == 1) ? "" : "s"
            return "\(diff) second\(s) ago"
        } else if (diff) < 3600 {
            diff = diff % 60
            return "\(min):\(diff) minutes ago"
        } else {
            diff = diff % 60
            return "\(hrs):\(min) hours ago"
        }
    }
    
    func reloadAutoSnapshotCache() {
        
        c64.suspend()
        numAutoSnapshots = c64.numAutoSnapshots()
        for n in 0..<numAutoSnapshots {
            let takenAt = TimeInterval(c64.autoSnapshotTimestamp(n))
            autoSnapshotImage[n] = c64.autoSnapshotImage(n)
            autoTimeStamp[n] = timeInfo(timeStamp: takenAt)
            autoTimeDiff[n] = timeDiffInfo(timeStamp: takenAt)
        }
        c64.resume()
        autoTableView.reloadData()
    }
    
    func reloadUserSnapshotCache() {
        
        c64.suspend()
        numUserSnapshots = c64.numUserSnapshots()
        for n in 0..<numUserSnapshots {
            let takenAt = TimeInterval(c64.userSnapshotTimestamp(n))
            userSnapshotImage[n] = c64.userSnapshotImage(n)
            userTimeStamp[n] = timeInfo(timeStamp: takenAt)
            userTimeDiff[n] = timeDiffInfo(timeStamp: takenAt)
        }
        c64.resume()
        userTableView.reloadData()
    }
    
    @IBAction func deleteAction(_ sender: Any!) {
        
        let sender = sender as! NSButton
        let index = numUserSnapshots - sender.tag - 1
        track("Deleting item \(index) tag \(sender.tag)")
        c64.deleteUserSnapshot(numUserSnapshots - sender.tag - 1)
        reloadUserSnapshotCache()
    }
    
    @IBAction override func cancelAction(_ sender: Any!) {
        
        track()
        c64.setAutoSaveSnapshots(true)
        hideSheet()
    }
    
    /*
    @IBAction func okAction(_ sender: Any!) {
        
        // Restore selected snapshot
        track("TODO: Restore selected snapshot")
        hideSheet()
    }
    */
    
    @IBAction func autoDoubleClick(_ sender: Any!) {
        
        let sender = sender as! NSTableView
        let index = numAutoSnapshots - sender.selectedRow - 1
        c64.restoreAutoSnapshot(index)
        hideSheet()
    }
    
    @IBAction func userDoubleClick(_ sender: Any!) {
        
        let sender = sender as! NSTableView
        let index = numUserSnapshots - sender.selectedRow - 1
        c64.restoreUserSnapshot(index)
        hideSheet()
    }
}

//
// NSTableViewDataSource, NSTableViewDelegate
//

extension SnapshotDialog : NSTableViewDataSource, NSTableViewDelegate {
    
    func numberOfRows(in tableView: NSTableView) -> Int {

        if (tableView == autoTableView) {
            return numAutoSnapshots
        }
            
        else if (tableView == userTableView) {
            return numUserSnapshots
        }
        
        fatalError();
    }
    
    func tableView(_ tableView: NSTableView, viewFor tableColumn: NSTableColumn?, row: Int) -> NSView?{
        
        let id = NSUserInterfaceItemIdentifier(rawValue: "defaultRow")
        let result = tableView.makeView(withIdentifier: id, owner: self) as! SnapshotTableCellView
        
        if (tableView == autoTableView) {
            
            let index = numAutoSnapshots - row - 1 // reverse display order
            result.preview.image = autoSnapshotImage[index]
            result.text.stringValue = autoTimeStamp[index]!
            result.subText.stringValue = autoTimeDiff[index]!
            result.delete.isHidden = true
            result.delete.tag = row
            return result;
        }
        
        else if (tableView == userTableView) {
            
            let index = numUserSnapshots - row - 1 // reverse display order
            result.preview.image = userSnapshotImage[index]
            result.text.stringValue = userTimeStamp[index]!
            result.subText.stringValue = userTimeDiff[index]!
            result.delete.isHidden = false
            result.delete.tag = row
            return result;
        }
    
        fatalError();
    }
}

//
// Drag and drop
//

extension SnapshotDialog {
    
    func tableView(_ tableView: NSTableView,
                   writeRowsWith rowIndexes: IndexSet,
                   to pboard: NSPasteboard) -> Bool {
        
        // Get index of dragged item
        guard let index = rowIndexes.first else {
            track("Cannot get table index for drag and drop")
            return false
        }
        
        // Get snapshot data
        var data : Data
        if (tableView == autoTableView) {
            data = c64.autoSnapshotData(numAutoSnapshots - index - 1)
        }
        else {
            precondition(tableView == userTableView)
            data = c64.userSnapshotData(numUserSnapshots - index - 1)
        }
        
        let pboardType = NSPasteboard.PasteboardType.fileContents
        pboard.declareTypes([pboardType], owner: self)
        let fileWrapper = FileWrapper.init(regularFileWithContents: data)
        fileWrapper.preferredFilename = "Snapshot.VC64"
        pboard.write(fileWrapper)
        
        return true;
    }
}


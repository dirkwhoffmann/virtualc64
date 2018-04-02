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
}

class SnapshotDialog : UserDialogController  {
    
    // Outlets
    
    @IBOutlet weak var autoTableView: NSTableView!
    @IBOutlet weak var userTableView: NSTableView!

    // Number of snapshots and image caches
    var numAutoSnapshots = -1
    var numUserSnapshots = -1
    var autoSnapshotImage : [Int:NSImage] = [:]
    var autoTimeStamp : [Int:String] = [:]
    var autoTimeDiff : [Int:String] = [:]
    var userSnapshotImage : [Int:NSImage] = [:]
    var userTimeStamp : [Int:String] = [:]
    var userTimeDiff : [Int:String] = [:]

    override public func awakeFromNib() {
        
        if numAutoSnapshots == -1 {
            c64.suspend() // Stop emulation while dialog is open
            setup()
        }
    }
    
    func timeInfo(timeStamp: TimeInterval) -> String {
        
        let formatter = DateFormatter()
        formatter.timeZone = TimeZone.current
        formatter.dateFormat = "HH:mm:ss" // "yyyy-MM-dd HH:mm"
        
        return formatter.string(from: Date(timeIntervalSince1970: timeStamp))
    }
    
    func timeDiffInfo(timeStamp: TimeInterval) -> String {
        
        let now = Date()
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
    
    func setup() {
        
        numAutoSnapshots = c64.numAutoSnapshots()
        numUserSnapshots = c64.numUserSnapshots()
        
        // Cache snapshot images and time stamps
        for n in 0..<numAutoSnapshots {
            let takenAt = TimeInterval(c64.autoSnapshotTimestamp(n))
            autoSnapshotImage[n] = c64.autoSnapshotImage(n)
            autoTimeStamp[n] = timeInfo(timeStamp: takenAt)
            autoTimeDiff[n] = timeDiffInfo(timeStamp: takenAt)
        }
        for n in 0..<numUserSnapshots {
            let takenAt = TimeInterval(c64.userSnapshotTimestamp(n))
            userSnapshotImage[n] = c64.userSnapshotImage(n)
            userTimeStamp[n] = timeInfo(timeStamp: takenAt)
            userTimeDiff[n] = timeDiffInfo(timeStamp: takenAt)
        }
    }
    
    @IBAction override func cancelAction(_ sender: Any!) {
        
        c64.resume()
        hideSheet()
    }
    
    @IBAction func okAction(_ sender: Any!) {
        
        // Restore selected snapshot
        track("TODO: Restore selected snapshot")
        
        c64.resume()
        hideSheet()
    }
    
    @IBAction func autoDoubleClick(_ sender: Any!) {
        
        let sender = sender as! NSTableView
        c64.restoreAutoSnapshot(sender.selectedRow)
        hideSheet()
    }
    
    @IBAction func userDoubleClick(_ sender: Any!) {
        
        let sender = sender as! NSTableView
        c64.restoreUserSnapshot(sender.selectedRow)
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
            
            result.preview.image = autoSnapshotImage[row]
            result.text.stringValue = autoTimeStamp[row]!
            result.subText.stringValue = autoTimeDiff[row]!
            result.delete.isHidden = true
            return result;
        }
        
        else if (tableView == userTableView) {
            
            result.preview.image = userSnapshotImage[row]
            result.text.stringValue = userTimeStamp[row]!
            result.subText.stringValue = userTimeDiff[row]!
            result.delete.isHidden = false
            return result;
        }
        
        fatalError();
    }
}


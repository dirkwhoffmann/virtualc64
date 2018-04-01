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
            setup()
        }
    }
    
    func setup() {
        
        numAutoSnapshots = c64.historicSnapshots()
        
        // Cache snapshot images
        for n in 0..<numAutoSnapshots {
            autoSnapshotImage[n] = c64.timetravelSnapshotImage(n)
        }
        
        // Compute time stamps
        let now = Date()
        let formatter = DateFormatter()
        formatter.timeZone = TimeZone.current
        formatter.dateFormat = "HH:mm:ss" // "yyyy-MM-dd HH:mm"
        
        for n in 0..<numAutoSnapshots {
            let takenAt = TimeInterval(c64.historicSnapshotTimestamp(n))
            let timeStamp = formatter.string(from: Date(timeIntervalSince1970: takenAt))
            autoTimeStamp[n] = "\(timeStamp)"

            var diff = Int(round(now.timeIntervalSince1970 - Double(takenAt)))
            diff += 140
            let min = diff / 60;
            let hrs = diff / 3600;
            if (diff) < 60 {
                let s = (diff == 1) ? "" : "s"
                autoTimeDiff[n] = "\(diff) second\(s) ago"
            } else if (diff) < 3600 {
                diff = diff % 60
                autoTimeDiff[n] = "\(min):\(diff) minutes ago"
            } else {
                diff = diff % 60
                autoTimeDiff[n] = "\(hrs):\(min) hours ago"
            }
        }
    }
    
    @IBAction func okAction(_ sender: Any!) {
        
        // Restore selected snapshot
        track("TODO: Restore selected snapshot")
        
        // parent. ...
        hideSheet()
    }
}

//
// NSTableViewDataSource, NSTableViewDelegate
//

extension SnapshotDialog : NSTableViewDataSource, NSTableViewDelegate {
    
    func numberOfRows(in tableView: NSTableView) -> Int {

        return numAutoSnapshots
    }
    
    func tableView(_ tableView: NSTableView, viewFor tableColumn: NSTableColumn?, row: Int) -> NSView?{
        
        let id = NSUserInterfaceItemIdentifier(rawValue: "defaultRow")
        let result: SnapshotTableCellView = tableView.makeView(withIdentifier: id, owner: self) as! SnapshotTableCellView
        result.preview.image = autoSnapshotImage[row]
        result.text.stringValue = autoTimeStamp[row]!
        result.subText.stringValue = autoTimeDiff[row]!
        return result;
    }
}


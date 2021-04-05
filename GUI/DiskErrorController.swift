// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class DiskErrorController: NSWindowController {
    
    var parent: Inspector!

    @IBOutlet weak var title: NSTextField!
    @IBOutlet weak var log: NSTableView!

    override func awakeFromNib() {

        let count = numberOfRows(in: log)
        if count == 1 {
            title.stringValue = "1 error has been found"
        } else {
            title.stringValue = "\(count) errors have been found"
        }
    }
    
    func cleanup() {
        track()
    }
    
    @IBAction func errorOkAction(_ sender: NSButton!) {
        
        track()
        window!.orderOut(self)
    }
}

extension DiskErrorController: NSTableViewDataSource {
    
    func numberOfRows(in tableView: NSTableView) -> Int {
        
        return parent.drive.disk.numErrors()
    }
    
    func tableView(_ tableView: NSTableView, objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        
        switch tableColumn?.identifier.rawValue {
            
        case "nr":
            return row + 1
            
        case "description":
            return parent.drive.disk.errorMessage(row)
            
        default:
            fatalError()
        }
    }
}

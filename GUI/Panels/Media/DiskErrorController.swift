// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class DiskErrorController: NSWindowController {
    
    var parent: DiskInspector!
    var analyzer: DiskAnalyzerProxy? { return parent.analyzer }
    
    @IBOutlet weak var title: NSTextField!
    @IBOutlet weak var log: NSTableView!

    var logbook: String?
    var errors = [String]()

    override func awakeFromNib() {

        if let ht = parent.halftrack { logbook = analyzer?.getLogbook(ht) }
        logbook?.enumerateLines { line, _ in self.errors.append(line) }

        // let count = numberOfRows(in: log)
        let count = errors.count
        if count == 1 {
            title.stringValue = "1 error has been found"
        } else {
            title.stringValue = "\(count) errors have been found"
        }
    }

    func cleanup() {

    }

    @IBAction func errorOkAction(_ sender: NSButton!) {
        
        window!.orderOut(self)
    }
}

extension DiskErrorController: NSTableViewDataSource {
    
    func numberOfRows(in tableView: NSTableView) -> Int {
        
        return errors.count
        /*
        if let ht = parent.halftrack {
            return analyzer?.numErrors(ht) ?? 0
        } else {
            return 0
        }
        */
    }
    
    func tableView(_ tableView: NSTableView, objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        
        switch tableColumn?.identifier.rawValue {
            
        case "nr":
            return row + 1
            
        case "description":
            if let ht = parent.halftrack {
                return  analyzer?.getLogbook(ht) ?? ""
            } else {
                return ""
            }
            
        default:
            fatalError()
        }
    }
}

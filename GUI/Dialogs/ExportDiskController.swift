// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class ExportDiskController: DialogController {

    @IBOutlet weak var button: NSPopUpButton!
    var type = FileType.FILETYPE_D64
    var savePanel: NSSavePanel!
    var selectedURL: URL?
    
    func showSheet(forDrive drive: DriveID) {
        
        precondition(drive == .DRIVE8 || drive == .DRIVE9)
        
        // Create save panel
        savePanel = NSSavePanel()
        savePanel.allowedFileTypes = ["d64"]
        savePanel.prompt = "Export"
        savePanel.title = "Export"
        savePanel.nameFieldLabel = "Export As:"
        savePanel.accessoryView = window?.contentView

        // Run panel as sheet
        if let win = myWindow {
            savePanel.beginSheetModal(for: win, completionHandler: { result in
                if result == .OK {
                    
                    myDocument?.export(drive: drive, to: self.savePanel.url)
                }
            })
        }
    }
    
    @IBAction func selectD64(_ sender: Any!) {
        track()
        savePanel.allowedFileTypes = ["d64"]
        type = FileType.FILETYPE_D64
    }

    @IBAction func selectG64(_ sender: Any!) {
        track()
        savePanel.allowedFileTypes = ["g64"]
        type = FileType.FILETYPE_G64
    }
    
    @IBAction func selectPRG(_ sender: Any!) {
        track()
        savePanel.allowedFileTypes = ["prg"]
        type = FileType.FILETYPE_PRG
    }
    
    @IBAction func selectP00(_ sender: Any!) {
        track()
        savePanel.allowedFileTypes = ["p00"]
        type = FileType.FILETYPE_P00
    }
    
    @IBAction func selectT64(_ sender: Any!) {
        track()
        savePanel.allowedFileTypes = ["t64"]
        type = FileType.FILETYPE_T64
    }
}

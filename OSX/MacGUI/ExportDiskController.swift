//
//  ExportPanel.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 09.02.18.
//

import Foundation

class ExportDiskController : UserDialogController {

    @IBOutlet weak var button: NSPopUpButton!
    var type: ContainerType = D64_CONTAINER
    var savePanel: NSSavePanel!
    var selectedURL: URL?
    
    /*
    override func awakeFromNib() {
    }
    */
    
    func showSheet(withParent: MyController, drive nr: Int) {
        
        precondition(nr == 1 || nr == 2)
        
        parent = withParent
        parentWindow = parent.window
        c64 = parent.mydocument.c64
       
        // Create save panel
        savePanel = NSSavePanel()
        savePanel.allowedFileTypes = ["d64"]
        savePanel.prompt = "Export"
        savePanel.title = "Export"
        savePanel.nameFieldLabel = "Export As:"
        savePanel.accessoryView = window?.contentView

        // Run panel as sheet
        savePanel.beginSheetModal(for: parent.window!, completionHandler: { result in
            if result == .OK {
                self.parent.mydocument.export(drive: nr, to: self.savePanel.url)
            }
        })
    }
    
    @IBAction func selectD64(_ sender: Any!) {
        track()
        savePanel.allowedFileTypes = ["d64"]
        type = D64_CONTAINER
    }

    @IBAction func selectG64(_ sender: Any!) {
        track()
        savePanel.allowedFileTypes = ["g64"]
        type = G64_CONTAINER
    }
    
    @IBAction func selectPRG(_ sender: Any!) {
        track()
        savePanel.allowedFileTypes = ["prg"]
        type = PRG_CONTAINER
    }
    
    @IBAction func selectP00(_ sender: Any!) {
        track()
        savePanel.allowedFileTypes = ["p00"]
        type = P00_CONTAINER
    }
    
    @IBAction func selectT64(_ sender: Any!) {
        track()
        savePanel.allowedFileTypes = ["t64"]
        type = T64_CONTAINER
    }
}

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
    var d64archive: D64Proxy!
    var selectedURL: URL?
    
    override func awakeFromNib() {
        
        // Export format PRG and P00 will only be available if disk is not empty
        let items = d64archive.numberOfItems()
        if items == 0 {
            track()
            print("\(button)")
            button.item(at: 2)?.isHidden = true // PRG
            button.item(at: 3)?.isHidden = true // P00
        }
    }
    
    override func showSheet(withParent: MyController, completionHandler:(() -> Void)? = nil) {
        
        parent = withParent
        parentWindow = parent.window
        let mydocument = parent.document as! MyDocument
        c64 = mydocument.c64
        
        
        // Convert inserted disk to D64 archive
        d64archive = D64Proxy.make(withDrive: c64.vc1541)
        
        // Create save panel
        savePanel = NSSavePanel()
        savePanel.allowedFileTypes = ["D64"]
        savePanel.prompt = "Export"
        savePanel.title = "Export"
        savePanel.nameFieldLabel = "Export As:"
        savePanel.accessoryView = window?.contentView
        // savePanel.canSelectHiddenExtension = true

        // Run panel as sheet
        // savePanel.beginSheetModal(for: parent.window!, completionHandler: { _ in })
        savePanel.beginSheetModal(for: parent.window!, completionHandler: { result in
            if result == .OK {
                self.parent.export(to: self.savePanel.url)
            }
        })
    }
    
    @IBAction func selectD64(_ sender: Any!) {
        track()
        savePanel.allowedFileTypes = ["d64"]
        type = D64_CONTAINER
    }
    
    @IBAction func selectT64(_ sender: Any!) {
        track()
        savePanel.allowedFileTypes = ["t64"]
        type = T64_CONTAINER
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
}

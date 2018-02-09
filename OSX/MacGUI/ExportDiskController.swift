//
//  ExportPanel.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 09.02.18.
//

import Foundation

class ExportDiskController : NSWindowController {

    @IBOutlet weak var button: NSPopUpButton!
    var type: ContainerType = D64_CONTAINER
    var savePanel: NSSavePanel!
    var d64archive: D64Proxy!
    var parent: MyController!
    
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
    func runPanel(_ d64archive: D64Proxy, parent: MyController) {
        
        self.d64archive = d64archive
        self.parent = parent
        
        // Create save panel
        savePanel = NSSavePanel()
        // savePanel.canSelectHiddenExtension = true
        savePanel.allowedFileTypes = ["D64"]
        savePanel.prompt = "Export"
        savePanel.title = "Export"
        savePanel.nameFieldLabel = "Export As:"
        
        // Add file type selector
        savePanel.accessoryView = window?.contentView
        
        // Run panel as sheet
        savePanel.beginSheetModal(for: parent.window!, completionHandler: { result in
            if result == .OK {
                self.export()
                parent.c64.vc1541.setDiskModified(false)
            }
        })
    }
    
    @discardableResult
    func export() -> Bool {
        
        // Create archive to write on disk
        var archive: ArchiveProxy?
        switch type {
        case D64_CONTAINER:
            track("Exporting to D64 format")
            archive = d64archive
            break;
            
        case T64_CONTAINER:
            track("Exporting to T64 format")
            archive = T64Proxy.make(withAnyArchive: d64archive)
            break;
            
        case PRG_CONTAINER:
            track("Exporting to PRG format")
            if d64archive.numberOfItems() > 1  {
                parent.showDiskHasMultipleFilesAlert(format: "PRG")
            }
            archive = PRGProxy.make(withAnyArchive: d64archive)
            break;
            
        case P00_CONTAINER:
            track("Exporting to P00 format")
            if d64archive.numberOfItems() > 1  {
                parent.showDiskHasMultipleFilesAlert(format: "P00")
            }
            archive = P00Proxy.make(withAnyArchive: d64archive)
            break;
            
        default:
            track("Unknown format")
            break;
        }
        
        // Serialize archive
        let data = NSMutableData.init(length: archive!.sizeOnDisk())
        let ptr = data!.mutableBytes
        archive!.write(toBuffer: ptr)
        
        // Get URL from panel
        guard let url = savePanel.url else {
            return false
        }

        // Export
        track("Exporting to file \(url)")
        data!.write(to: url, atomically: true)
        return true
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
        savePanel.allowedFileTypes = ["P00"]
        type = P00_CONTAINER
    }
}

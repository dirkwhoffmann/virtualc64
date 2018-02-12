//
//  File.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 09.02.18.
//

import Foundation

extension NSBitmapImageRep {
    var png: Data? {
        return representation(using: .png, properties: [:])
    }
    var jpg: Data? {
        return representation(using: .jpeg, properties: [:])
    }
}
extension Data {
    var bitmap: NSBitmapImageRep? {
        return NSBitmapImageRep(data: self)
    }
}
extension NSImage {
    var pngRepresentation: Data? {
        return tiffRepresentation?.bitmap?.png
    }
    var jpgRepresentation: Data? {
        return tiffRepresentation?.bitmap?.jpg
    }
}

class ExportScreenshotController : UserDialogController {
    
    static let PNG = 0
    static let JPG = 1
    static let TIFF = 2
    
    var type = 0
    var savePanel: NSSavePanel!
    
    override func showSheet(withParent: MyController, completionHandler:(() -> Void)? = nil) {
        
        parent = withParent
        // mydocument = parent.document as! MyDocument
        parentWindow = parent.window
        c64 = (parent.document as! MyDocument).c64
        
        // Create save panel
        savePanel = NSSavePanel()
        // savePanel.canSelectHiddenExtension = true
        savePanel.allowedFileTypes = ["png"]
        savePanel.prompt = "Export"
        savePanel.title = "Export"
        savePanel.nameFieldLabel = "Export As:"
        savePanel.accessoryView = window?.contentView
        
        // Run panel as sheet
        savePanel.beginSheetModal(for: parent.window!, completionHandler: { result in
            if result == .OK {
                self.export()
            }
        })
    }
    
    @discardableResult func export() -> Bool {
        
        let image = parent.screenshot()
        var data: Data?
        
        print("targetFormat = \(type)")
        
        switch type {
        case ExportScreenshotController.PNG:
            track("Exporting to PNG")
            data = image?.pngRepresentation
            break;
            
        case ExportScreenshotController.JPG:
            track("Exporting to JPG")
            data = image?.jpgRepresentation
            break;

        case ExportScreenshotController.TIFF:
            track("Exporting to TIFF")
            data = image?.tiffRepresentation
            break;

        default:
            track("Unknown format")
            break;
        }
        
        // Get URL from panel
        guard let url = savePanel.url else {
            return false
        }
        
        // Export
        track("Exporting to file \(url)")
        do {
            try data?.write(to: url, options: .atomic)
            return true
        } catch {
            track("Cannot export screenshot")
            return false
        }
    }
    
    @IBAction func selectPNG(_ sender: Any!) {
        track()
        savePanel.allowedFileTypes = ["png"]
        type = ExportScreenshotController.PNG
    }
    
    @IBAction func selectJPG(_ sender: Any!) {
        track()
        savePanel.allowedFileTypes = ["jpg"]
        type = ExportScreenshotController.JPG
    }
    
    @IBAction func selectTIFF(_ sender: Any!) {
        track()
        savePanel.allowedFileTypes = ["tiff"]
        type = ExportScreenshotController.TIFF
    }
}

//
//  DragAndDrop.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 14.01.18.
//

import Foundation

extension NSPasteboard.PasteboardType {
    static let compatibleFileURL = NSPasteboard.PasteboardType(kUTTypeFileURL as String)
}

public extension MetalView {
    
    //! Returns a list of supported drag and drop types
    func acceptedTypes() -> [NSPasteboard.PasteboardType] {
        return [.compatibleFileURL, .string, .fileContents]
    }
    
    //! Register supported drag and drop types
    func setupDragAndDrop() {
    
        registerForDraggedTypes(acceptedTypes())
    }

    override func draggingEntered(_ sender: NSDraggingInfo) -> NSDragOperation {
        
        let pasteBoard = sender.draggingPasteboard()
        guard let type = pasteBoard.availableType(from: acceptedTypes()) else {
            return NSDragOperation()
        }
        
        switch (type) {
            
        case .string:
            
            print ("Dragged in string")
            return NSDragOperation.copy
        
        case .fileContents:
            
            print ("Dragged in file contents")
            return NSDragOperation.copy
            
        case .compatibleFileURL:
            
            print ("Dragged in filename")
            return NSDragOperation.copy
            
        default:
            
            return NSDragOperation()
        }
    }
    
    override func draggingExited(_ sender: NSDraggingInfo?) {
    
    }
    
    override func prepareForDragOperation(_ sender: NSDraggingInfo) -> Bool {
        
        return true
    }
    
    override func performDragOperation(_ sender: NSDraggingInfo) -> Bool {
        
        let document = controller.document as! MyDocument
        
        let pasteBoard = sender.draggingPasteboard()
        guard let type = pasteBoard.availableType(from: acceptedTypes()) else {
            return false
        }
        
        switch (type) {
            
        case .string:
            
            // Type text on virtual keyboard
            guard let text = pasteBoard.string(forType: .string) else {
                return false
            }
            controller.keyboardcontroller.typeOnKeyboard(string: text, completion: nil)
            return true
            
        case .fileContents:
            
            // Check if we got another virtual machine dragged in
            let fileWrapper = pasteBoard.readFileWrapper()
            let fileData = fileWrapper?.regularFileContents
            let length = fileData!.count
            let nsData = fileData! as NSData
            let rawPtr = nsData.bytes
            let snapshot = SnapshotProxy.make(withBuffer: rawPtr, length: length)
            
            controller.c64.load(fromSnapshot: snapshot)
            return true
            
        case .compatibleFileURL:
            
            guard let url = NSURL.init(from: pasteBoard) as URL? else {
               return false
            }
            let  path = url.path
            track("Processing dragged in file \(path)")
            
            // Is it a snapshot from a different version?
            if SnapshotProxy.isUnsupportedSnapshotFile(path) {
                document.showSnapshotVersionAlert()
                return false
            }
            
            // Is it a snapshop with a matching version number?
            document.attachment = SnapshotProxy.make(withFile: path)
            if document.attachment != nil {
                controller.processAttachment()
                document.fileURL = nil // Make document 'Untitled'
                return true
            }
            
            // Is it an archive?
            document.attachment = ArchiveProxy.make(withFile: path)
            if document.attachment != nil {
                controller.processAttachment()
                return true
            }
        
            // Is it a band tape?
            document.attachment = TAPProxy.make(withFile: path)
            if document.attachment != nil {
                controller.processAttachment()
                return true
            }
            
            // Is it a cartridge?
            document.attachment = CRTProxy.make(withFile: path)
            if document.attachment != nil {
                controller.processAttachment()
                return true
            }
        
            // We haven't found any known file format. We could attach an archive
            // of type FileArchive which would copy the file's raw data in memory
            // at the location where normal programs start.
            /*
            document.attachedArchive = FileArchiveProxy.makeFileArchive(withFile: path)
            if document.attachedArchive != nil {
                track("Successfully read archive.")
                controller.processAttachment()
                return true
            }
            */
            
            // However, it seems better to reject the drag operation.
            track("Unsupported file type dragged in.")
            return false
            
        default:
            break
        }
        
        return false
    }
    
    override func concludeDragOperation(_ sender: NSDraggingInfo?) {
        
    }
}

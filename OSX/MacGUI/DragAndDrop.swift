//
//  MyMetalViewResponder.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 14.01.18.
//

import Foundation

struct DragType {
    static let string = NSPasteboard.PasteboardType.string
    static let contents = NSPasteboard.PasteboardType.fileContents
    static let filenames = NSPasteboard.PasteboardType(rawValue: "NSFilenamesPboardType")
}

public extension MetalView {
    
    //! Returns a list of supported drag and drop types
    func acceptedTypes() -> [NSPasteboard.PasteboardType] {
    
        return [DragType.filenames, DragType.string, DragType.contents]
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
            
        case DragType.string:
            
            print ("Dragged in string")
            return NSDragOperation.copy
        
        case DragType.contents:
            
            print ("Dragged in file contents")
            return NSDragOperation.copy
            
        case DragType.filenames:
            
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
            
        case DragType.string:
            
            // Type text on virtual keyboard
            guard let text = pasteBoard.string(forType:DragType.string) else {
                return false
            }
            // document.c64.keyboard.typeText(text)
            controller.simulateUserTypingText(text, withInitialDelay: 0) 
            return true
            
        case DragType.contents:
            
            // Check if we got another virtual machine dragged in
            let fileWrapper = pasteBoard.readFileWrapper()
            let fileData = fileWrapper?.regularFileContents
            let length = fileData!.count
            let nsData = fileData! as NSData
            let rawPtr = nsData.bytes
            let snapshot = SnapshotProxy.make(withBuffer: rawPtr, length: length)
            
            controller.c64.load(fromSnapshot: snapshot)
            return true
            
        case DragType.filenames:
            
            let paths = pasteBoard.propertyList(forType: DragType.filenames) as! [String]
            let path = paths[0]
            
            track("Processing dragged in file \(path)")
            
            // Is it a ROM file?
            if document.loadRom(path) {
                return true
            }
            
            // Is it a snapshot from a different version?
            if SnapshotProxy.isUsupportedSnapshotFile(path) {
                document.showSnapshotVersionAlert()
                return false
            }
            
            // Is it a snapshop with a matching version number?
            if let snapshot = SnapshotProxy.make(withFile: path) {
                controller.c64.load(fromSnapshot: snapshot)
                document.fileURL = nil // Make document 'Untitled'
                return true
            }
            
            // Is it an archive?
            document.attachment = ArchiveProxy.make(withFile: path)
            if document.attachment != nil {
                
                track("Successfully read archive.")
                controller.showMountDialog()
                return true
            }
        
            // Is it a band tape?
            document.attachment = TAPProxy.make(withFile: path)
            if document.attachment != nil {
                track("Successfully read tape.")
                controller.showMountDialog()
                return true
            }
            
            // Is it a cartridge?
            document.attachment = CRTProxy.make(withFile: path)
            if document.attachment != nil {
                track("Successfully read cartridge.")
                controller.showMountDialog()
                return true
            }
        
            // We haven't found any known file format. We could attach an archive
            // of type FileArchive which would copy the file's raw data in memory
            // at the location where normal programs start.
            /*
            document.attachedArchive = FileArchiveProxy.makeFileArchive(withFile: path)
            if document.attachedArchive != nil {
                track("Successfully read archive.")
                controller.showMountDialog()
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

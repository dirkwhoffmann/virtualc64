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
            
            if let snapshot = SnapshotProxy.make(withBuffer: rawPtr, length: length) {
                if controller.proceedWithUnsafedDisk() {
                    controller.c64.load(fromSnapshot: snapshot)
                }
            }
            return true
            
        case .compatibleFileURL:
            
            if let url = NSURL.init(from: pasteBoard) as URL? {
                return controller.processFile(url: url,
                                              warnAboutUnsafedDisk: true,
                                              showMountDialog: !controller.autoMount)
            } else {
                return false
            }
   
        default:
            break
        }
        
        return false
    }
    
    override func concludeDragOperation(_ sender: NSDraggingInfo?) {
        
    }
}

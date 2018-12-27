//
// This file is part of VirtualC64 - A cycle accurate Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
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
        
        let pasteBoard = sender.draggingPasteboard
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
        
        let document = controller.mydocument
        let pasteBoard = sender.draggingPasteboard
        guard let type = pasteBoard.availableType(from: acceptedTypes()) else {
            return false
        }
        
        switch (type) {
            
        case .string:
            
            // Type text on virtual keyboard
            guard let text = pasteBoard.string(forType: .string) else {
                return false
            }
            controller.keyboardcontroller.type(string: text, completion: nil)
            return true
            
        case .fileContents:
            
            // Check if we got another virtual machine dragged in
            let fileWrapper = pasteBoard.readFileWrapper()
            let fileData = fileWrapper?.regularFileContents
            let length = fileData!.count
            let nsData = fileData! as NSData
            let rawPtr = nsData.bytes
            
            guard let snapshot = SnapshotProxy.make(withBuffer: rawPtr, length: length) else {
                return false
            }
            if document.proceedWithUnexportedDisk() {
                DispatchQueue.main.async {
                    let snap = snapshot
                    self.controller.c64.flash(snap)
                }
                return true
            } else {
                return false
            }
            
        case .compatibleFileURL:
            
            if let url = NSURL.init(from: pasteBoard) as URL? {
                do {
                    try document.createAttachment(from: url)
                    return document.mountAttachment()
                    
                } catch {
                    let dragAndDropError = error
                    let deadline = DispatchTime.now() + .milliseconds(200)
                    DispatchQueue.main.asyncAfter(deadline: deadline) {
                        NSApp.presentError(dragAndDropError)
                    }
                }
            }
            return false
            
        default:
            return false
        }
    }
    
    override func concludeDragOperation(_ sender: NSDraggingInfo?) {
    }
}

// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension NSPasteboard.PasteboardType {
    static let compatibleFileURL = NSPasteboard.PasteboardType(kUTTypeFileURL as String)
}

public extension MetalView {
    
    // Returns a list of supported drag and drop types
    func acceptedTypes() -> [NSPasteboard.PasteboardType] {
        
        return [.compatibleFileURL, .string, .fileContents]
    }

    // Registers the supported drag and drop types
    func setupDragAndDrop() {
    
        registerForDraggedTypes(acceptedTypes())
    }

    override func draggingEntered(_ sender: NSDraggingInfo) -> NSDragOperation {
        
        let pasteBoard = sender.draggingPasteboard
        guard let type = pasteBoard.availableType(from: acceptedTypes()) else {
            return NSDragOperation()
        }
        
        switch type {
            
        case .string:
            track("Dragged in string")
            return NSDragOperation.copy
        
        case .fileContents:
            track("Dragged in file contents")
            return NSDragOperation.copy
            
        case .compatibleFileURL:
            track("Dragged in filename")
            
            if let url = NSURL(from: pasteBoard) as URL? {
            
                // Open the drop zone layer
                let type = AnyFileProxy.type(of: url)
                parent.renderer.dropZone.open(type: type, delay: 0.25)
            }
                
            return NSDragOperation.copy
            
        default:
            track("Unsupported type")
            return NSDragOperation()
        }
    }

    override func draggingUpdated(_ sender: NSDraggingInfo) -> NSDragOperation {
        
        parent.renderer.dropZone.draggingUpdated(sender)
        return NSDragOperation.copy
    }
    
    override func draggingExited(_ sender: NSDraggingInfo?) {
    
        parent.renderer.dropZone.close(delay: 0.25)
    }
    
    override func prepareForDragOperation(_ sender: NSDraggingInfo) -> Bool {
        
        parent.renderer.dropZone.close(delay: 0.25)
        return true
    }
    
    override func performDragOperation(_ sender: NSDraggingInfo) -> Bool {
        
        let pasteBoard = sender.draggingPasteboard
        
        guard let type = pasteBoard.availableType(from: acceptedTypes()),
            let document = parent.mydocument else {
                return false
        }
        
        switch type {
            
        case .string:
            
            // Type text on virtual keyboard
            guard let text = pasteBoard.string(forType: .string) else {
                return false
            }
            parent.keyboard.type(text)
            return true
            
        case .fileContents:
            
            // Check if we got another virtual machine dragged in
            let fileWrapper = pasteBoard.readFileWrapper()
            let fileData = fileWrapper?.regularFileContents
            let length = fileData!.count
            let nsData = fileData! as NSData
            let rawPtr = nsData.bytes
            
            let snapshot: SnapshotProxy? = try? Proxy.make(buffer: rawPtr, length: length)
            if snapshot == nil { return false }
            
            if document.proceedWithUnexportedDisk() {
                DispatchQueue.main.async {
                    try? self.parent.c64.flash(snapshot!)
                }
                return true
            } else {
                return false
            }
            
        case .compatibleFileURL:
            
            if let url = NSURL(from: pasteBoard) as URL? {
                            
                do {
                    try document.createAttachment(from: url)
                    
                    // Check drop zone for drive 8
                    if parent.renderer.dropZone.isInside(sender, zone: 0) {
                        return document.mountAttachmentAsDisk(drive: .DRIVE8)
                    }
                    // Check drop zone for drive 9
                    if parent.renderer.dropZone.isInside(sender, zone: 1) {
                        return document.mountAttachmentAsDisk(drive: .DRIVE9)
                    }
                    // Check drop zone for the expansion port
                    if parent.renderer.dropZone.isInside(sender, zone: 2) {
                        return document.mountAttachmentAsCartridge()
                    }
                    // Check drop zone for the datasette
                    if parent.renderer.dropZone.isInside(sender, zone: 3) {
                        return document.mountAttachmentAsTape()
                    }

                    return document.mountAttachment()
                    
                } catch {
                    (error as? VC64Error)?.cantOpen(url: url)
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

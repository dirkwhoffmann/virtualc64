// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension NSPasteboard.PasteboardType {

    static let compatibleFileURL =
    NSPasteboard.PasteboardType(kUTTypeFileURL as String)
}

public extension MetalView {

    func setupDragAndDrop() {

        registerForDraggedTypes(acceptedTypes())
    }

    func acceptedTypes() -> [NSPasteboard.PasteboardType] {
        
        return [.compatibleFileURL, .string, .fileContents]
    }

    override func draggingEntered(_ sender: NSDraggingInfo) -> NSDragOperation {
        
        let pasteBoard = sender.draggingPasteboard
        guard let type = pasteBoard.availableType(from: acceptedTypes()) else {
            return NSDragOperation()
        }

        dropZone = nil
        dropUrl = nil
        dropType = nil
        
        switch type {
            
        case .string:
            return NSDragOperation.copy
        
        case .fileContents:
            return NSDragOperation.copy
            
        case .compatibleFileURL:
            
            if let url = NSURL(from: pasteBoard) as URL? {
            
                // Unpack the file if it is compressed
                dropUrl = url.unpacked(maxSize: 2048 * 1024)
                
                // Analyze the file type
                let type = AnyFileProxy.type(of: dropUrl)

                // Open the drop zone layer
                parent.renderer.dropZone.open(type: type, delay: 0.25)
            }
                
            return NSDragOperation.copy
            
        default:
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

        if let type = pasteBoard.availableType(from: acceptedTypes()) {

            switch type {

            case .string:
                return performStringDrag(sender)

            case .compatibleFileURL:
                return performUrlDrag(sender)

            default:
                break
            }
        }

        return false
    }

    func performStringDrag(_ sender: NSDraggingInfo) -> Bool {

        let pasteBoard = sender.draggingPasteboard

        // Type text on virtual keyboard
        guard let text = pasteBoard.string(forType: .string) else {
            return false
        }
        parent.keyboard.type(text)
        return true
    }

    func performUrlDrag(_ sender: NSDraggingInfo) -> Bool {

        // Only proceed if an URL is given
        if dropUrl == nil { return false }

        // Only proceed if a file type can be derived
        guard let type = FileType(url: dropUrl) else { return false }

        // Only proceed if a draggable type is given
        if !FileType.draggable.contains(type) { return false }

        // Check all drop zones
        var zone: Int?
        for i in 0...4 {
            if renderer.dropZone.isInside(sender, zone: i) {
                if renderer.dropZone.enabled[i] {
                    zone = i
                } else {
                    return false
                }
            }
        }

        dropZone = zone
        dropType = type
        return true
    }

    override func concludeDragOperation(_ sender: NSDraggingInfo?) {
    }
}

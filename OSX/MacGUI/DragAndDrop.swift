//
//  MyMetalViewResponder.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 14.01.18.
//

import Foundation

/*
extension NSPasteboard.PasteboardType {
    
    static let backwardsCompatibleFileURL: NSPasteboard.PasteboardType = {
        
        if #available(OSX 10.13, *) {
            return NSPasteboard.PasteboardType.fileURL
        } else {
            return NSPasteboard.PasteboardType(kUTTypeFileURL as String)
        }
        
    } ()
}
*/

struct DragType {
    static let string = NSPasteboard.PasteboardType.string
    static let contents = NSPasteboard.PasteboardType.fileContents
    static let filenames = NSPasteboard.PasteboardType(rawValue: "NSFilenamesPboardType")
}

public extension MyMetalView {
    
    //! Returns a list of supported drag and drop types
    func acceptedTypes() -> [NSPasteboard.PasteboardType] {
    
        return [DragType.filenames, DragType.string, DragType.contents]
    }
    
    //! Register supported drag and drop types
    @objc public func setupDragAndDrop() {
    
        registerForDraggedTypes(acceptedTypes())
    }

    @objc public func swiftDraggingEntered(_ sender: NSDraggingInfo) -> NSDragOperation {
        
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
    
    @objc public func swiftDraggingExited(_ sender: NSDraggingInfo?) {
    
    }
    
    @objc public func swiftPrepareForDragOperation(_ sender: NSDraggingInfo) -> Bool {
        
        return true
    }
    
    @objc public func swiftPerformDragOperation(_ sender: NSDraggingInfo,
                                                controller: MyController) -> Bool {
    
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
            document.c64.keyboard.typeText(text)
            return true
            
        case DragType.contents:
            
            // Check if we got another virtual machine dragged in
            let fileWrapper = pasteBoard.readFileWrapper()
            let fileData = fileWrapper?.regularFileContents
            let length = UInt32(fileData!.count)
            let nsData = fileData! as NSData
            let rawPtr = nsData.bytes
            let snapshot = SnapshotProxy.snapshot(fromBuffer: rawPtr, length: length)
            
            controller.c64.load(fromSnapshot: snapshot)
            return true
            
        case DragType.filenames:
            
            let paths = pasteBoard.propertyList(forType: DragType.filenames) as! [String]
            let path = paths[0]
            print("Processing file \(path)")
            
            // Is it a snapshot?
            if SnapshotProxy.isSnapshotFile(path) {
                
                print("  Snapshot found");
                // Do the version numbers match?
                if SnapshotProxy.isSnapshotFile(path,
                                                major: V_MAJOR,
                                                minor: V_MINOR,
                                                subminor: V_SUBMINOR) {
                    if let snapshot = SnapshotProxy.snapshot(fromFile: path) {
                        controller.c64.load(fromSnapshot: snapshot)
                        return true
                    }
                } else {
                    
                    print("  ERROR: Version number must be \(V_MAJOR).\(V_MINOR)")
                    
                    document.showVersionNumberAlert()
                    return false
                }
            }
            
            // Is it a ROM file?
            if document.loadRom(path) {
                return true
            }
            
            // Is it a cartridge?
            if document.setCartridgeWithName(path) {
                controller.mountCartridge()
                return true
            }
            
            // Is it a NIB archive?
            if document.setNIBArchiveWithName(path) {
                controller.showMountDialog()
                return true
            }
            
            // Is it a G64 archive?
            if document.setG64ArchiveWithName(path) {
                controller.showMountDialog()
                return true
            }
            
            // Is it a TAP container?
            if document.setTAPContainerWithName(path) {
                controller.showTapeDialog()
                return true
            }
            
            // Is it an archive other than G64?
            if document.setArchiveWithName(path) {
                controller.showMountDialog()
                return true
            }
        
        default:
            
            return false
        }
        
    return false
    }
    
    @objc public func swiftconcludeDragOperation(_ sender: NSDraggingInfo?) {
        
    }
}

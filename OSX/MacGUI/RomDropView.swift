//
//  RomDropView.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 12.02.18.
//

import Foundation

extension NSDraggingInfo
{
    var path: String? {
        let pasteBoard = draggingPasteboard()
        if let _ = pasteBoard.availableType(from: [DragType.filenames]) {
            if let paths = pasteBoard.propertyList(forType: DragType.filenames) as? [String] {
                return paths[0]
            }
        }
        return nil
    }
}

class RomDropView : NSImageView
{
    @IBOutlet weak var controller: RomDialogController!
    // var oldImage: NSImage? = nil
    
    override func awakeFromNib()
    {
        registerForDraggedTypes([DragType.filenames])
    }
    
    override func draggingEntered(_ sender: NSDraggingInfo) -> NSDragOperation
    {
        track()
        if let path = sender.path {
            if controller.c64.isRom(path) {
                // oldImage = image
                image = controller.romImage
                return .copy
            }
        }
        return NSDragOperation()
    }
    
    override func draggingExited(_ sender: NSDraggingInfo?)
    {
        track()
        controller.refresh()
    }
    
    override func prepareForDragOperation(_ sender: NSDraggingInfo) -> Bool
    {
        track()
        return true
    }

    override func performDragOperation(_ sender: NSDraggingInfo) -> Bool
    {
        track()
        if let path = sender.path {
            return controller.c64.loadRom(path)
        }
        return false
    }
    
    override func concludeDragOperation(_ sender: NSDraggingInfo?)
    {
        track()
        controller.refresh()
    }
}


//
//  RomDropView.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 12.02.18.
//

import Foundation

extension NSDraggingInfo
{
    var url: URL? {
        let pasteBoard = draggingPasteboard()
        if let _ = pasteBoard.availableType(from: [DragType.filenames]) {
            if let paths = pasteBoard.propertyList(forType: DragType.filenames) as? [String] {
                return URL.init(string: paths[0])
            }
        }
        return nil
    }
}

class RomDropView : NSImageView
{
    @IBOutlet var dialogController: RomDialogController!
    
    override func awakeFromNib()
    {
        registerForDraggedTypes([DragType.filenames])
    }
    
    override func draggingEntered(_ sender: NSDraggingInfo) -> NSDragOperation
    {
        track()
        if let url = sender.url {
            if dialogController.c64.isRom(url) {
                image = dialogController.romImageMedium
                return .copy
            }
        }
        return NSDragOperation()
    }
    
    override func draggingExited(_ sender: NSDraggingInfo?)
    {
        track()
        dialogController.refresh()
    }
    
    override func prepareForDragOperation(_ sender: NSDraggingInfo) -> Bool
    {
        track()
        return true
    }

    override func performDragOperation(_ sender: NSDraggingInfo) -> Bool
    {
        track()
        
        guard let url = sender.url else {
            return false
        }
        if !dialogController.c64.loadRom(url) {
            return false
        }
        
        // Check if all ROMs are loaded
        if dialogController.c64.isRunnable() {
            dialogController.hideSheet()
        }
        return true
    }
    
    override func concludeDragOperation(_ sender: NSDraggingInfo?)
    {
        track()
        dialogController.refresh()
    }
}


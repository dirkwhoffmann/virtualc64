//
//  SnapshotImageView.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 02.04.18.
//

import Foundation

class SnapshotImageView: NSImageView
{
    override func mouseDown(with theEvent: NSEvent) {
        
        track()
        
        //1.
        let stringType = NSPasteboard.PasteboardType.string
        let pasteboardItem = NSPasteboardItem()
        pasteboardItem.setDataProvider(self, forTypes: [stringType])
        
        //2.
        let draggingItem = NSDraggingItem(pasteboardWriter: pasteboardItem)
        draggingItem.setDraggingFrame(self.bounds, contents: "Hallo")
        
        //3.
        beginDraggingSession(with: [draggingItem], event: theEvent, source: self)
    }
    
    override func awakeFromNib()
    {
        track()
       // registerForDraggedTypes([NSPasteboard.PasteboardType.compatibleFileURL])
    }
}

extension SnapshotImageView: NSDraggingSource {
    //1.
    func draggingSession(_ session: NSDraggingSession, sourceOperationMaskFor context: NSDraggingContext) -> NSDragOperation {
        
        track()
        return .generic
    }
}

// MARK: - NSDraggingSource
extension SnapshotImageView: NSPasteboardItemDataProvider {
   
    func pasteboard(_ pasteboard: NSPasteboard?, item: NSPasteboardItem, provideDataForType type: NSPasteboard.PasteboardType) {
        
        track()
        
        //1.
        if let pasteboard = pasteboard, type.rawValue == String(kUTTypeTIFF), let image = NSImage(named:NSImage.Name(rawValue: "metal")) {
            //2.
            let finalImage = image
            //3.
            let tiffdata = finalImage.tiffRepresentation
            pasteboard.setData(tiffdata, forType:type)
        }
        

    }
}

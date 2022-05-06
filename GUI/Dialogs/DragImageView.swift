// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import Cocoa

class DragImageView: NSImageView, NSDraggingSource {

    @IBOutlet weak var exporter: NSFilePromiseProviderDelegate!

    var mouseDownEvent: NSEvent?

    override init(frame frameRect: NSRect) {
        
        debug(level: 2)

        super.init(frame: frameRect)
        isEditable = true
    }

    required init?(coder: NSCoder) {

        debug(level: 2)

        super.init(coder: coder)
        isEditable = true
    }

    override func draw(_ dirtyRect: NSRect) {
        super.draw(dirtyRect)
    }

    //
    // Dragging source
    //

    func draggingSession(_: NSDraggingSession,
                         sourceOperationMaskFor _: NSDraggingContext) -> NSDragOperation {
        
        return NSDragOperation.copy
    }

    func draggingSession(_: NSDraggingSession,
                         endedAt _: NSPoint, operation: NSDragOperation) {

        if operation == .delete { image = nil }
    }

    override func mouseDown(with theEvent: NSEvent) {
        
        debug(level: 2)
        
        // Create a file promise provider
        let provider = NSFilePromiseProvider(fileType: "public.data",
                                             delegate: exporter)
        
        // Embed the provider into a dragging item
        let draggingItem = NSDraggingItem(pasteboardWriter: provider)
        draggingItem.setDraggingFrame(self.bounds, contents: image!)
        
        // Write the dragging item into the pasteboard
        beginDraggingSession(with: [draggingItem], event: theEvent, source: self)
    }
}

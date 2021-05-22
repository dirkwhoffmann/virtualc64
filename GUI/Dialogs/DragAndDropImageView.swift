import Cocoa

class DragDropImageView: NSImageView, NSDraggingSource {

    // Last mouse down event
    var mouseDownEvent: NSEvent?

    override init(frame frameRect: NSRect) {
        
        track()

        super.init(frame: frameRect)
        isEditable = true
    }

    required init?(coder: NSCoder) {

        track()

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
        
        track()
        
        // Create a file promise provider
        let provider = NSFilePromiseProvider.init(fileType: "public.data",
                                                  delegate: self)
        
        // Embed the provider into a dragging item
        let draggingItem = NSDraggingItem(pasteboardWriter: provider)
        draggingItem.setDraggingFrame(self.bounds, contents: image!)
        
        // Write the dragging item into the pasteboard
        beginDraggingSession(with: [draggingItem], event: theEvent, source: self)
    }
}

extension DragDropImageView: NSFilePromiseProviderDelegate {
   
    func filePromiseProvider(_ filePromiseProvider: NSFilePromiseProvider, fileNameForType fileType: String) -> String {
        
        return "virtualc64.mp4"
    }
    
    func filePromiseProvider(_ filePromiseProvider: NSFilePromiseProvider, writePromiseTo url: URL, completionHandler: @escaping (Error?) -> Void) {
        
        track("\(url)")

        let source = URL.init(fileURLWithPath: "/tmp/virtualc64.mp4")
        
        do {
            if FileManager.default.fileExists(atPath: url.path) {
                try FileManager.default.removeItem(at: url)
            }
            try FileManager.default.copyItem(at: source, to: url)
            completionHandler(nil)
            
        } catch let error {
            print("Failed to copy \(source) to \(url): \(error)")
            completionHandler(error)
        }
    }
}

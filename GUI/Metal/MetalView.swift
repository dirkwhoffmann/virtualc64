// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

public class MetalView: MTKView {
    
    @IBOutlet weak var parent: MyController!
    
    var myDocument: MyDocument { return parent.mydocument! }
    var renderer: Renderer { return parent.renderer }
    var prefs: Preferences { return parent.pref }
    
    // Reference to the first mouse (internal, always connected)
    var mouse1: GamePad?

    // Reference to the second mouse (USB, usually unconnected)
    var mouse2: GamePad?

    // Shows whether the emulator possesses of the mouse
    var gotMouse = false

    /* Tracking area for trapping the mouse. The tracking area is utilized to
     * determine when the mouse is moved into or out of the emulator window.
     */
    var trackingArea: NSTrackingArea?
    
    // Indicates whether the mouse is currently within the tracking area
    var insideTrackingArea = false
        
    // Time stamp needed to detect a shaking mouse
    var lastShake = DispatchTime(uptimeNanoseconds: 0)

    // Temporary storage of the properties of a dragged in file
    var dropZone: Int?
    var dropUrl: URL?
    var dropType: FileType?

    required public init(coder: NSCoder) {
    
        super.init(coder: coder)
    }
    
    required public override init(frame frameRect: CGRect, device: MTLDevice?) {
        
        super.init(frame: frameRect, device: device)
    }
    
    override open func awakeFromNib() {

        setupDragAndDrop()
    }
    
    override public var acceptsFirstResponder: Bool { return true }
    override public func resignFirstResponder() -> Bool { return false }
    
    // Adjusts view height by a certain number of pixels
    func adjustHeight(_ height: CGFloat) {
    
        var newFrame = frame
        newFrame.origin.y -= height
        newFrame.size.height += height
        frame = newFrame
    }    
}

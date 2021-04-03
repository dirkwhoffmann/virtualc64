// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

public class MetalView: MTKView {
    
    @IBOutlet weak var parent: MyController!
    
    var prefs: Preferences { return parent.pref }
    
    // Reference to the first mouse (internal, always connected)
    var mouse: GamePad?

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
    
    // Shrinks view vertically by the height of the status bar
    public func shrink() { adjustHeight(-26.0) }
    
    // Expand view vertically by the height of the status bar
    public func expand() { adjustHeight(26.0) }
}

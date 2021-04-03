// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class Layer: NSObject {
    
    let renderer: Renderer

    var device: MTLDevice { return renderer.device }
    var view: MTKView { return renderer.view }
    var c64: C64Proxy { return renderer.parent.c64 }
    
    // Alpha channel
    var alpha: AnimatedFloat = AnimatedFloat.init()

    //
    // Initializing
    //
    
    init(renderer: Renderer) {
        
        self.renderer = renderer
        super.init()
    }
    
    //
    // Querying the visual state
    //
        
    var isVisible: Bool { return alpha.current > 0.0 }
    var isOpaque: Bool { return alpha.current == 1.0 }
    var isTransparent: Bool { return alpha.current < 1.0 }
    var isAnimating: Bool { return alpha.animates() }
    var isFadingIn: Bool { return alpha.target > alpha.current }
    var isFadingOut: Bool { return alpha.target < alpha.current }
    
    //
    // Opening and closing
    //
    
    func open(delay: Double) { alpha.steps = Int(60 * delay); open(); }
    func close(delay: Double) { alpha.steps = Int(60 * delay); close(); }
    func open() { alpha.target = 1.0 }
    func close() { alpha.target = 0.0 }
    func toggle() { isVisible ? close() : open(); }

    //
    // Performing continuous tasks
    //
    
    func update(frames: Int64) {
        
        if alpha.animates() {

            alpha.move()
            alphaDidChange()
        }
    }

    func alphaDidChange() { }
}

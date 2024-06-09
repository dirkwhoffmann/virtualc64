// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class Layer: NSObject {
    
    let renderer: Renderer

    var controller: MyController { return renderer.parent }
    var ressourceManager: RessourceManager { return renderer.ressourceManager }
    var device: MTLDevice { return renderer.device }
    var view: MTKView { return renderer.view }
    var emu: EmulatorProxy? { return renderer.parent.emu }
    
    // Alpha channel of this layer
    var alpha: AnimatedFloat = AnimatedFloat(0.0)

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
    var isAnimating: Bool { return alpha.animates }
    var isFadingIn: Bool { return alpha.target > alpha.current }
    var isFadingOut: Bool { return alpha.target < alpha.current }
    
    //
    // Opening and closing
    //
    
    func open(delay: Double) { alpha.steps = Int(60 * delay); open() }
    func close(delay: Double) { alpha.steps = Int(60 * delay); close() }
    func open() { alpha.target = 1.0 }
    func close() { alpha.target = 0.0 }
    func toggle() { if isVisible { close() } else { open() } }

    //
    // Performing continuous tasks
    //
    
    func update(frames: Int64) {
        
        if alpha.animates {

            alpha.move()
            alphaDidChange()
            
            if !alpha.animates {
                if isVisible { layerDidOpen() } else { layerDidClose() }
            }
        }
    }
    
    func alphaDidChange() { }
    func layerDidOpen() { }
    func layerDidClose() { }
}

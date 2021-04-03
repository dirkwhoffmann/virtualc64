// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import simd

struct AnimationType {
    
    static let geometry = 1
    static let color = 2
    static let texture = 4
}

class AnimatedFloat {

    var current: Float
    var delta = Float(0.0)
    var steps = 1 { didSet { delta = (target - current) / Float(steps) } }
    var target: Float { didSet { delta = (target - current) / Float(steps) } }

    init(current: Float = 0.0, target: Float = 0.0) {

        self.current = current
        self.target = target
    }

    convenience init(_ value: Float) {

        self.init(current: value, target: value)
    }

    func set(_ value: Float) {

        current = value
        target = value
    }

    func animates() -> Bool {

        return current != target
    }

    func move() {

        if abs(current - target) < abs(delta) {
            current = target
        } else {
            current += delta
        }
    }
}

extension Renderer {
    
    func performAnimationStep() {
        
        assert(animates != 0)
        
        var cont: Bool
        
        // Check for geometry animation
        if (animates & AnimationType.geometry) != 0 {
            
            angleX.move()
            angleY.move()
            angleZ.move()
            cont = angleX.animates() || angleY.animates() || angleZ.animates()
            
            shiftX.move()
            shiftY.move()
            shiftZ.move()
            cont = cont || shiftX.animates() || shiftY.animates() || shiftZ.animates()
            
            // Check if animation has terminated
            if !cont {
                animates -= AnimationType.geometry
                angleX.set(0)
                angleY.set(0)
                angleZ.set(0)
            }
            
            buildMatrices3D()
        }
        
        // Check for alpha channel animation
        /*
        if (animates & AnimationType.alpha) != 0 {
            
            alpha.move()
            noise.move()
            cont = alpha.animates() || noise.animates()
            
            // Check if animation has terminated
            if !cont {
                animates -= AnimationType.alpha
            }
        }
        */
        
        // Check for texture animation
        if (animates & AnimationType.texture) != 0 {
            
            cutoutX1.move()
            cutoutY1.move()
            cutoutX2.move()
            cutoutY2.move()
            cont = cutoutX1.animates() || cutoutY1.animates() || cutoutX2.animates() || cutoutY2.animates()
            
            // Update texture cutout
            let x = CGFloat(cutoutX1.current)
            let y = CGFloat(cutoutY1.current)
            let w = CGFloat(cutoutX2.current - cutoutX1.current)
            let h = CGFloat(cutoutY2.current - cutoutY1.current)
            canvas.textureRect = CGRect.init(x: x, y: y, width: w, height: h)
            buildVertexBuffers()
            
            // Check if animation has terminated
            if !cont {
                animates -= AnimationType.texture
            }
        }
    }
    
    //
    // Texture animations
    //
    
    func zoomTextureIn(steps: Int = 30) {

        track("Zooming texture in...")

        let target = canvas.visibleNormalized
        
        cutoutX1.target = Float(target.minX)
        cutoutY1.target = Float(target.minY)
        cutoutX2.target = Float(target.maxX)
        cutoutY2.target = Float(target.maxY)

        cutoutX1.steps = steps
        cutoutY1.steps = steps
        cutoutX2.steps = steps
        cutoutY2.steps = steps

        animates |= AnimationType.texture
    }

    func zoomTextureOut(steps: Int = 30) {

        track("Zooming texture out...")
        
        let current = canvas.textureRect
        let target = canvas.entireNormalized
        
        cutoutX1.current = Float(current.minX)
        cutoutY1.current = Float(current.minY)
        cutoutX2.current = Float(current.maxX)
        cutoutY2.current = Float(current.maxY)

        cutoutX1.target = Float(target.minX)
        cutoutY1.target = Float(target.minY)
        cutoutX2.target = Float(target.maxX)
        cutoutY2.target = Float(target.maxY)

        cutoutX1.steps = steps
        cutoutY1.steps = steps
        cutoutX2.steps = steps
        cutoutY2.steps = steps

        animates |= AnimationType.texture
    }
    
    //
    // Geometry animations
    //
    
    func zoomIn(steps: Int = 60) {
        
        track("Zooming in...")
        
        shiftZ.current = 6.0
        shiftZ.target = 0.0
        angleX.target = 0.0
        angleY.target = 0.0
        angleZ.target = 0.0
        
        shiftZ.steps = steps
        angleX.steps = steps
        angleY.steps = steps
        angleZ.steps = steps
        
        animates |= AnimationType.geometry
    }
    
    func zoomOut(steps: Int = 40) {
        
        track("Zooming out...")
        
        shiftZ.target = 6.0
        angleX.target = 0.0
        angleY.target = 0.0
        angleZ.target = 0.0
        
        shiftZ.steps = steps
        angleX.steps = steps
        angleY.steps = steps
        angleZ.steps = steps
        
        animates |= AnimationType.geometry
    }
    
    func rotate(x: Float = 0.0, y: Float = 0.0, z: Float = 0.0) {
        
        track("Rotating x: \(x) y: \(y) z: \(z)...")
        
        angleX.target = x
        angleY.target = y
        angleZ.target = z
        
        let steps = 60
        angleX.steps = steps
        angleY.steps = steps
        angleZ.steps = steps
        
        animates |= AnimationType.geometry
    }
    
    func rotateRight() { rotate(y: -90) }
    func rotateLeft() { rotate(y: 90) }
    func rotateDown() { rotate(x: 90) }
    func rotateUp() { rotate(x: -90) }
    
    func scroll(steps: Int = 120) {
        
        track("Scrolling...")
        
        shiftY.current = -1.5
        shiftY.target = 0
        angleX.target = 0.0
        angleY.target = 0.0
        angleZ.target = 0.0
        
        shiftY.steps = steps
        angleX.steps = steps
        angleY.steps = steps
        angleZ.steps = steps
        
        animates |= AnimationType.geometry
    }
    
    func snapToFront() {
        
        track("Snapping to front...")
        
        shiftZ.current = -0.05
        shiftZ.target = 0
        shiftZ.steps = 10
        
        animates |= AnimationType.geometry
    }
    
    //
    // Alpha channel animations
    //
    
    /*
    func blend(from: Float, to: Float, steps: Int) {
        
        track("Blending...")
        
        angleX.target = 0
        angleY.target = 0
        angleZ.target = 0
        
        angleX.steps = steps
        angleY.steps = steps
        angleZ.steps = steps
        
        animates |= AnimationType.alpha
    }
    */
    /*
    func blendIn(steps: Int = 40) {
        
        noise.target = 0.0
        noise.steps = steps
        blend(from: 0.0, to: 1.0, steps: steps)
    }
    
    func blendOut(steps: Int = 40) {
        
        noise.target = 1.0
        noise.steps = steps
        blend(from: 1.0, to: 0.0, steps: steps)
    }
    */
}

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
    static let alpha = 2
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
        if (animates & AnimationType.alpha) != 0 {
            
            alpha.move()
            noise.move()
            cont = alpha.animates() || noise.animates()
            
            // Check if animation has terminated
            if !cont {
                animates -= AnimationType.alpha
            }
        }
        
        // Check for texture animation
        if (animates & AnimationType.texture) != 0 {
            
            cutoutX1.move()
            cutoutY1.move()
            cutoutX2.move()
            cutoutY2.move()
            cont = cutoutX1.animates() || cutoutY1.animates() || cutoutX2.animates() || cutoutY2.animates()
            
            // Update texture cutout
            textureRect = CGRect.init(x: CGFloat(cutoutX1.current),
                                      y: CGFloat(cutoutY1.current),
                                      width: CGFloat(cutoutX2.current - cutoutX1.current),
                                      height: CGFloat(cutoutY2.current - cutoutY1.current))
            buildVertexBuffer()
            
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
        
        let targetRect = computeTextureRect()
        
        cutoutX1.target = Float(targetRect.minX)
        cutoutY1.target = Float(targetRect.minY)
        cutoutX2.target = Float(targetRect.maxX)
        cutoutY2.target = Float(targetRect.maxY)
        
        cutoutX1.steps = steps
        cutoutY1.steps = steps
        cutoutX2.steps = steps
        cutoutY2.steps = steps
        
        animates |= AnimationType.texture
    }
    
    func zoomTextureOut(steps: Int = 30) {
        
        track("Zooming texture out...")
        
        cutoutX1.current = Float(textureRect.minX)
        cutoutY1.current = Float(textureRect.minY)
        cutoutX2.current = Float(textureRect.maxX)
        cutoutY2.current = Float(textureRect.maxY)
        
        cutoutX1.target = 0.0
        cutoutY1.target = 0.0
        cutoutX2.target = 1.0
        cutoutY2.target = 1.0
        
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
        alpha.current = 0.0
        alpha.target = 1.0
        noise.current = 0.0
        noise.target = 0.0
        
        shiftZ.steps = steps
        angleX.steps = steps
        angleY.steps = steps
        angleZ.steps = steps
        alpha.steps = steps
        noise.steps = steps
        
        animates |= AnimationType.geometry + AnimationType.alpha
    }
    
    func zoomOut(steps: Int = 40) {
        
        track("Zooming out...")
        
        shiftZ.target = 6.0
        angleX.target = 0.0
        angleY.target = 0.0
        angleZ.target = 0.0
        alpha.target = 0.0
        noise.target = 1.0
        
        shiftZ.steps = steps
        angleX.steps = steps
        angleY.steps = steps
        angleZ.steps = steps
        alpha.steps = steps
        noise.steps = steps
        
        animates |= AnimationType.geometry + AnimationType.alpha
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
        alpha.target = 1.0
        
        shiftY.steps = steps
        angleX.steps = steps
        angleY.steps = steps
        angleZ.steps = steps
        alpha.steps = 1
        
        animates |= AnimationType.geometry + AnimationType.alpha
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
    
    func blend(from: Float, to: Float, steps: Int) {
        
        track("Blending...")
        
        angleX.target = 0
        angleY.target = 0
        angleZ.target = 0
        alpha.current = from
        alpha.target = to
        
        angleX.steps = steps
        angleY.steps = steps
        angleZ.steps = steps
        alpha.steps = steps
        
        animates |= AnimationType.alpha
    }
    
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
    
    
    
    
    
    
    
    
    //! Returns true iff an animation is in progress
    func animatesDeprecated() -> Bool {

        return
            currentXAngle != targetXAngle ||
            currentYAngle != targetYAngle ||
            currentZAngle != targetZAngle ||
            currentEyeX != targetEyeX ||
            currentEyeY != targetEyeY ||
            currentEyeZ != targetEyeZ ||
            currentAlpha != targetAlpha
    }
    
    func eyeX() -> Float {
        
        return currentEyeX
    }
    
    func setEyeX(_ newX: Float) {
        
        currentEyeX = newX
        targetEyeX = newX
        self.buildMatrices3D()
    }
    
    func eyeY() -> Float {
        
        return currentEyeY
    }
    
    func setEyeY(_ newY: Float) {
    
        currentEyeY = newY
        targetEyeY = newY
        self.buildMatrices3D()
    }
    
    func eyeZ() -> Float {
        
        return currentEyeZ
    }
    
    func setEyeZ(_ newZ: Float) {
    
        currentEyeZ = newZ
        targetEyeZ = newZ
        self.buildMatrices3D()
    }
    
    func updateAngles() {
    
        if abs(currentXAngle - targetXAngle) < abs(deltaXAngle) {
            currentXAngle = targetXAngle
        } else {
            currentXAngle += deltaXAngle
        }
    
        if abs(currentYAngle - targetYAngle) < abs(deltaYAngle) {
            currentYAngle = targetYAngle
        } else {
            currentYAngle += deltaYAngle
        }
    
        if abs(currentZAngle - targetZAngle) < abs(deltaZAngle) {
            currentZAngle = targetZAngle
        } else {
            currentZAngle += deltaZAngle
        }
    
        if abs(currentEyeX - targetEyeX) < abs(deltaEyeX) {
            currentEyeX = targetEyeX
        } else {
            currentEyeX += deltaEyeX
        }
    
        if abs(currentEyeY - targetEyeY) < abs(deltaEyeY) {
            currentEyeY = targetEyeY
        } else {
            currentEyeY += deltaEyeY
        }
    
        if abs(currentEyeZ - targetEyeZ) < abs(deltaEyeZ) {
            currentEyeZ = targetEyeZ
        } else {
            currentEyeZ += deltaEyeZ
        }
    
        if abs(currentAlpha - targetAlpha) < abs(deltaAlpha) {
            currentAlpha = targetAlpha
        } else {
            currentAlpha += deltaAlpha
        }
    
        currentXAngle -= (currentXAngle >= 360.0) ? 360 : 0
        currentXAngle += (currentXAngle < 0.0) ? 360 : 0
        currentYAngle -= (currentYAngle >= 360.0) ? 360 : 0
        currentYAngle += (currentYAngle < 0.0) ? 360 : 0
        currentZAngle -= (currentZAngle >= 360.0) ? 360 : 0
        currentZAngle += (currentZAngle < 0.0) ? 360 : 0
    }
    
    func computeAnimationDeltaSteps(animationCycles: Int) {
    
        let cycles = Float(animationCycles)
        deltaXAngle = (targetXAngle - currentXAngle) / cycles
        deltaYAngle = (targetYAngle - currentYAngle) / cycles
        deltaZAngle = (targetZAngle - currentZAngle) / cycles
        deltaEyeX = (targetEyeX - currentEyeX) / cycles
        deltaEyeY = (targetEyeY - currentEyeY) / cycles
        deltaEyeZ = (targetEyeZ - currentEyeZ) / cycles
        deltaAlpha = (targetAlpha - currentAlpha) / cycles
    }
    
    func oldZoom() {
    
        track("Zooming in...")
    
        currentEyeZ  = 6
        targetXAngle = 0
        targetYAngle = 0
        targetZAngle = 0
    
        self.computeAnimationDeltaSteps(animationCycles: 120 /* 2 sec */)
    }
    
    func oldRotateBack() {
    
        track("Rotating back...")
    
        targetXAngle = 0
        targetZAngle = 0
        targetYAngle += 90

        self.computeAnimationDeltaSteps(animationCycles: 60 /* 1 sec */)
  
        targetYAngle -= (targetYAngle >= 360) ? 360 : 0
    }
    
    func oldRotate() {
    
        track("Rotating...")
    
        targetXAngle = 0
        targetZAngle = 0
        targetYAngle -= 90
    
        self.computeAnimationDeltaSteps(animationCycles: 60 /* 1 sec */)
        
        targetYAngle += (targetYAngle < 0) ? 360 : 0
    }
    
    func oldScroll() {
        
        track("Scrolling...")
    
        currentEyeY = -1.5
        targetXAngle = 0
        targetYAngle = 0
        targetZAngle = 0
    
        self.computeAnimationDeltaSteps(animationCycles: 120 /* 2 sec */)
    }
    
    func oldFadeIn() {
    
        track("Fading in...")
    
        currentXAngle = -90
        currentEyeZ   = 5.0
        currentEyeY   = 4.5
        targetXAngle  = 0
        targetYAngle  = 0
        targetZAngle  = 0
    
        self.computeAnimationDeltaSteps(animationCycles: 120 /* 2 sec */)
    }
    
    func oldBlendIn() {
    
        track("Blending in...")
    
        targetXAngle = 0
        targetYAngle = 0
        targetZAngle = 0
        currentAlpha = 0.0
        targetAlpha  = 1.0
    
        self.computeAnimationDeltaSteps(animationCycles: 120 /* 2 sec */)
    }

    func oldSnapToFront() {
        
        track("Snapping to front...")
        
        currentEyeZ   = -0.05
        
        self.computeAnimationDeltaSteps(animationCycles: 15 /* 0.25 sec */)
    }
}

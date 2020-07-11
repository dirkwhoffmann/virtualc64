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
    
    func zoom() {
    
        track("Zooming in...")
    
        currentEyeZ  = 6
        targetXAngle = 0
        targetYAngle = 0
        targetZAngle = 0
    
        self.computeAnimationDeltaSteps(animationCycles: 120 /* 2 sec */)
    }
    
    func rotateBack() {
    
        track("Rotating back...")
    
        targetXAngle = 0
        targetZAngle = 0
        targetYAngle += 90

        self.computeAnimationDeltaSteps(animationCycles: 60 /* 1 sec */)
  
        targetYAngle -= (targetYAngle >= 360) ? 360 : 0
    }
    
    func rotate() {
    
        track("Rotating...")
    
        targetXAngle = 0
        targetZAngle = 0
        targetYAngle -= 90
    
        self.computeAnimationDeltaSteps(animationCycles: 60 /* 1 sec */)
        
        targetYAngle += (targetYAngle < 0) ? 360 : 0
    }
    
    func scroll() {
        
        track("Scrolling...")
    
        currentEyeY = -1.5
        targetXAngle = 0
        targetYAngle = 0
        targetZAngle = 0
    
        self.computeAnimationDeltaSteps(animationCycles: 120 /* 2 sec */)
    }
    
    func fadeIn() {
    
        track("Fading in...")
    
        currentXAngle = -90
        currentEyeZ   = 5.0
        currentEyeY   = 4.5
        targetXAngle  = 0
        targetYAngle  = 0
        targetZAngle  = 0
    
        self.computeAnimationDeltaSteps(animationCycles: 120 /* 2 sec */)
    }
    
    func blendIn() {
    
        track("Blending in...")
    
        targetXAngle = 0
        targetYAngle = 0
        targetZAngle = 0
        currentAlpha = 0.0
        targetAlpha  = 1.0
    
        self.computeAnimationDeltaSteps(animationCycles: 120 /* 2 sec */)
    }

    func snapToFront() {
        
        track("Snapping to front...")
        
        currentEyeZ   = -0.05
        
        self.computeAnimationDeltaSteps(animationCycles: 15 /* 0.25 sec */)
    }
    
    // -------------------------------------------------------------------------
    //                                Matrix utilities
    // -------------------------------------------------------------------------
    
    /*
    func matrix_from_perspective(fovY: Float,
                                 aspect: Float,
                                 nearZ: Float,
                                 farZ: Float) -> matrix_float4x4 {
        
        // Variant 1: Keeps correct aspect ratio independent of window size
        let yscale = 1.0 / tanf(fovY * 0.5) // 1 / tan == cot
        let xscale = yscale / aspect
        let q = farZ / (farZ - nearZ)
    
        // Alternative: Adjust to window size
        // float yscale = 1.0f / tanf(fovY * 0.5f);
        // float xscale = 0.75 * yscale;
        // float q = farZ / (farZ - nearZ);
        
        var m = matrix_float4x4()
        m.columns.0 = SIMD4<Float>(xscale, 0.0, 0.0, 0.0)
        m.columns.1 = SIMD4<Float>(0.0, yscale, 0.0, 0.0)
        m.columns.2 = SIMD4<Float>(0.0, 0.0, q, 1.0)
        m.columns.3 = SIMD4<Float>(0.0, 0.0, q * -nearZ, 0.0)
    
        return m
    }
    
    func matrix_from_translation(x: Float,
                                 y: Float,
                                 z: Float) -> matrix_float4x4 {
        
        var m = matrix_identity_float4x4
        m.columns.3 = SIMD4<Float>(x, y, z, 1.0)
    
        return m
    }
    
    func matrix_from_rotation(radians: Float,
                              x: Float,
                              y: Float,
                              z: Float) -> matrix_float4x4 {
        
        var v = vector_float3(x, y, z)
        v = normalize(v)
        let cos = cosf(radians)
        let cosp = 1.0 - cos
        let sin = sinf(radians)
    
        var m = matrix_float4x4()
        m.columns.0 = SIMD4<Float>(cos + cosp * v.x * v.x,
                                   cosp * v.x * v.y + v.z * sin,
                                   cosp * v.x * v.z - v.y * sin,
                                   0.0)
        m.columns.1 = SIMD4<Float>(cosp * v.x * v.y - v.z * sin,
                                   cos + cosp * v.y * v.y,
                                   cosp * v.y * v.z + v.x * sin,
                                   0.0)
        m.columns.2 = SIMD4<Float>(cosp * v.x * v.z + v.y * sin,
                                   cosp * v.y * v.z - v.x * sin,
                                   cos + cosp * v.z * v.z,
                                   0.0)
        m.columns.3 = SIMD4<Float>(0.0,
                                   0.0,
                                   0.0,
                                   1.0)
        return m
    }
    */
}

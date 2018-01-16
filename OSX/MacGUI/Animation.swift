//
//  Animation.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 14.01.18.
//

import Foundation

public extension MetalView {
    /*
    @objc public func initAnimation() {
        
        currentEyeX   = 0.0
        targetEyeX    = 0.0
        deltaEyeX     = 0.0
        currentEyeY   = 0.0
        targetEyeY    = 0.0
        deltaEyeY     = 0.0
        currentEyeZ   = 0.0
        targetEyeZ    = 0.0
        deltaEyeZ     = 0.0
        currentXAngle = 0.0
        targetXAngle  = 0.0
        deltaXAngle   = 0.0
        currentYAngle = 0.0
        targetYAngle  = 0.0
        deltaYAngle   = 0.0
        currentZAngle = 0.0
        targetZAngle  = 0.0
        deltaZAngle   = 0.0
        currentAlpha  = 0.0
        targetAlpha   = 0.0
        deltaAlpha    = 0.0
    }
    */
    
    //! Returns true iff an animation is in progress
    @objc public func animates() -> Bool {

        return
            currentXAngle != targetXAngle ||
            currentYAngle != targetYAngle ||
            currentZAngle != targetZAngle ||
            currentEyeX != targetEyeX ||
            currentEyeY != targetEyeY ||
            currentEyeZ != targetEyeZ ||
            currentAlpha != targetAlpha
    }
    
    @objc public func eyeX() -> Float {
        
        return currentEyeX
    }
    
    @objc public func setEyeX(_ newX: Float) {
        
        currentEyeX = newX
        targetEyeX = newX
        self.buildMatrices3D()
    }
    
    @objc public func eyeY() -> Float {
        
        return currentEyeY
    }
    
    @objc public func setEyeY(_ newY: Float) {
    
        currentEyeY = newY
        targetEyeY = newY
        self.buildMatrices3D()
    }
    
    @objc public func eyeZ() -> Float {
        
        return currentEyeZ
    }
    
    @objc public func setEyeZ(_ newZ: Float) {
    
        currentEyeZ = newZ
        targetEyeZ = newZ
        self.buildMatrices3D()
    }
    
    @objc public func updateAngles() {
    
        if fabs(currentXAngle - targetXAngle) < fabs(deltaXAngle) {
            currentXAngle = targetXAngle
        } else {
            currentXAngle += deltaXAngle
        }
    
        if fabs(currentYAngle - targetYAngle) < fabs(deltaYAngle) {
            currentYAngle = targetYAngle
        } else {
            currentYAngle += deltaYAngle
        }
    
        if fabs(currentZAngle - targetZAngle) < fabs(deltaZAngle) {
            currentZAngle = targetZAngle
        } else {
            currentZAngle += deltaZAngle
        }
    
        if fabs(currentEyeX - targetEyeX) < fabs(deltaEyeX) {
            currentEyeX = targetEyeX
        } else {
            currentEyeX += deltaEyeX
        }
    
        if fabs(currentEyeY - targetEyeY) < fabs(deltaEyeY) {
            currentEyeY = targetEyeY
        } else {
            currentEyeY += deltaEyeY
        }
    
        if fabs(currentEyeZ - targetEyeZ) < fabs(deltaEyeZ) {
            currentEyeZ = targetEyeZ
        } else {
            currentEyeZ += deltaEyeZ
        }
    
        if fabs(currentAlpha - targetAlpha) < fabs(deltaAlpha) {
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
    
    @objc public func computeAnimationDeltaSteps(animationCycles: Int) {
    
        let cycles = Float(animationCycles)
        deltaXAngle = (targetXAngle - currentXAngle) / cycles
        deltaYAngle = (targetYAngle - currentYAngle) / cycles
        deltaZAngle = (targetZAngle - currentZAngle) / cycles
        deltaEyeX = (targetEyeX - currentEyeX) / cycles
        deltaEyeY = (targetEyeY - currentEyeY) / cycles
        deltaEyeZ = (targetEyeZ - currentEyeZ) / cycles
        deltaAlpha = (targetAlpha - currentAlpha) / cycles
    }
    
    @objc public func zoom() {
    
        print("Zooming in...\n\n")
    
        currentEyeZ  = 6
        targetXAngle = 0
        targetYAngle = 0
        targetZAngle = 0
    
        self.computeAnimationDeltaSteps(animationCycles: 120 /* 2 sec */)
    }
    
    @objc public func rotateBack() {
    
        print("Rotating back...\n\n")
    
        targetXAngle = 0
        targetZAngle = 0
        targetYAngle += 90

        self.computeAnimationDeltaSteps(animationCycles: 60 /* 1 sec */)
  
        targetYAngle -= (targetYAngle >= 360) ? 360 : 0
    }
    
    @objc public func rotate() {
    
        print("Rotating...\n\n")
    
        targetXAngle = 0
        targetZAngle = 0
        targetYAngle -= 90
    
        self.computeAnimationDeltaSteps(animationCycles: 60 /* 1 sec */)
        
        targetYAngle += (targetYAngle < 0) ? 360 : 0
    }
    
    @objc public func scroll() {
        
        print("Scrolling...\n\n")
    
        currentEyeY = -1.5
        targetXAngle = 0
        targetYAngle = 0
        targetZAngle = 0
    
        self.computeAnimationDeltaSteps(animationCycles: 120 /* 2 sec */)
    }
    
    @objc public func fadeIn() {
    
        print("Fading in...\n\n")
    
        currentXAngle = -90
        currentEyeZ   = 5.0
        currentEyeY   = 4.5
        targetXAngle  = 0
        targetYAngle  = 0
        targetZAngle  = 0
    
        self.computeAnimationDeltaSteps(animationCycles: 120 /* 2 sec */)
    }
    
    @objc public func blendIn() {
    
        print("Blending in...\n\n")
    
        targetXAngle = 0
        targetYAngle = 0
        targetZAngle = 0
        currentAlpha = 0.0
        targetAlpha  = 1.0
    
        self.computeAnimationDeltaSteps(animationCycles: 120 /* 2 sec */)
    }

    // --------------------------------------------------------------------------------
    //                                Matrix utilities
    // --------------------------------------------------------------------------------

    /*
    @objc public func matrix_identity() -> matrix_float4x4 {
        
        let X = float4(1.0, 0.0, 0.0, 0.0)
        let Y = float4(0.0, 1.0, 0.0, 0.0)
        let Z = float4(0.0, 0.0, 1.0, 0.0)
        let W = float4(0.0, 0.0, 0.0, 1.0)
    
        let identity =  matrix_float4x4(X,Y,Z,W)
    
        return identity
    }
    */
    
    @objc public func matrix_from_perspective(fovY: Float,
                                              aspect: Float,
                                              nearZ: Float,
                                              farZ: Float) -> matrix_float4x4 {
        
        // Variant 1: Keeps correct aspect ratio independent of window size
        let yscale = 1.0 / tanf(fovY * 0.5) // 1 / tan == cot
        let xscale = yscale / aspect
        let q = farZ / (farZ - nearZ)
    
        // Alternative: Adjust to window size */
        // float yscale = 1.0f / tanf(fovY * 0.5f);
        // float xscale = 0.75 * yscale;
        // float q = farZ / (farZ - nearZ);
        
        var m = matrix_float4x4()
        m.columns.0 = float4(xscale, 0.0, 0.0, 0.0)
        m.columns.1 = float4(0.0, yscale, 0.0, 0.0)
        m.columns.2 = float4(0.0, 0.0, q, 1.0)
        m.columns.3 = float4(0.0, 0.0, q * -nearZ, 0.0)
    
        return m
    }
    
    @objc public func matrix_from_translation(x: Float,
                                              y: Float,
                                              z: Float) -> matrix_float4x4 {
    
        var m = matrix_identity_float4x4;
        m.columns.3 = float4(x, y, z, 1.0)
    
        return m
    }
    
    @objc public func matrix_from_rotation(radians: Float,
                                           x: Float,
                                           y: Float,
                                           z: Float) -> matrix_float4x4 {
    
        var v = vector_float3(x, y, z)
        v = normalize(v)
        let cos = cosf(radians)
        let cosp = 1.0 - cos
        let sin = sinf(radians)
    
        var m = matrix_float4x4()
        m.columns.0 = float4(cos + cosp * v.x * v.x,
                             cosp * v.x * v.y + v.z * sin,
                             cosp * v.x * v.z - v.y * sin,
                             0.0)
        m.columns.1 = float4(cosp * v.x * v.y - v.z * sin,
                             cos + cosp * v.y * v.y,
                             cosp * v.y * v.z + v.x * sin,
                             0.0)
        m.columns.2 = float4(cosp * v.x * v.z + v.y * sin,
                             cosp * v.y * v.z - v.x * sin,
                             cos + cosp * v.z * v.z,
                             0.0)
        m.columns.3 = float4(0.0,
                             0.0,
                             0.0,
                             1.0)
        return m
    }
    
}

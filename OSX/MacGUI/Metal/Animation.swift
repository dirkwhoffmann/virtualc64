//
// This file is part of VirtualC64 - A cycle accurate Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation
import simd

public extension MetalView {
 
    //! Returns true iff an animation is in progress
    func animates() -> Bool {

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
    
    func matrix_from_perspective(fovY: Float,
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
    
    func matrix_from_translation(x: Float,
                                 y: Float,
                                 z: Float) -> matrix_float4x4 {
        
        var m = matrix_identity_float4x4;
        m.columns.3 = float4(x, y, z, 1.0)
    
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

//
//  Animation.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 14.01.18.
//

import Foundation

public extension MyMetalView {
    
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

}

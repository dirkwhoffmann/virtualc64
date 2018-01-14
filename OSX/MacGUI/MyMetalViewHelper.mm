#import "C64GUI.h"
#import <CoreGraphics/CoreGraphics.h>

@implementation MyMetalView(Helper)

// --------------------------------------------------------------------------------
//                               Animation effects
// --------------------------------------------------------------------------------

- (bool)animates
{
    return (currentXAngle != targetXAngle ||
            currentYAngle != targetYAngle ||
            currentZAngle != targetZAngle ||
            currentEyeX != targetEyeX ||
            currentEyeY != targetEyeY ||
            currentEyeZ != targetEyeZ ||
            currentAlpha != targetAlpha);
}

- (float)eyeX
{
    return currentEyeX;
}

- (void)setEyeX:(float)newX
{
    currentEyeX = targetEyeX = newX;
    [self buildMatrices3D];
}

- (float)eyeY
{
    return currentEyeY;
}

- (void)setEyeY:(float)newY
{
    currentEyeY = targetEyeY = newY;
    [self buildMatrices3D];
}

- (float)eyeZ
{
    return currentEyeZ;
}

- (void)setEyeZ:(float)newZ
{
    currentEyeZ = targetEyeZ = newZ;
    [self buildMatrices3D];
}

- (void)updateAngles
{
    if (fabs(currentXAngle - targetXAngle) < fabs(deltaXAngle)) currentXAngle = targetXAngle;
    else														currentXAngle += deltaXAngle;
        
    if (fabs(currentYAngle - targetYAngle) < fabs(deltaYAngle)) currentYAngle = targetYAngle;
    else														currentYAngle += deltaYAngle;
        
    if (fabs(currentZAngle - targetZAngle) < fabs(deltaZAngle)) currentZAngle = targetZAngle;
    else														currentZAngle += deltaZAngle;
        
    if (fabs(currentEyeX - targetEyeX) < fabs(deltaEyeX))       currentEyeX   = targetEyeX;
    else														currentEyeX   += deltaEyeX;
        
    if (fabs(currentEyeY - targetEyeY) < fabs(deltaEyeY))       currentEyeY   = targetEyeY;
    else														currentEyeY   += deltaEyeY;
        
    if (fabs(currentEyeZ - targetEyeZ) < fabs(deltaEyeZ))       currentEyeZ   = targetEyeZ;
    else														currentEyeZ   += deltaEyeZ;

    if (fabs(currentAlpha - targetAlpha) < fabs(deltaAlpha))    currentAlpha  = targetAlpha;
    else														currentAlpha  += deltaAlpha;

    if (currentXAngle >= 360.0) currentXAngle -= 360.0;
    if (currentXAngle < 0.0) currentXAngle += 360.0;
    if (currentYAngle >= 360.0) currentYAngle -= 360.0;
    if (currentYAngle < 0.0) currentYAngle += 360.0;
    if (currentZAngle >= 360.0) currentZAngle -= 360.0;
    if (currentZAngle < 0.0) currentZAngle += 360.0;
}

- (void)computeAnimationDeltaSteps:(int)animationCycles
{
    deltaXAngle = (targetXAngle - currentXAngle) / animationCycles;
    deltaYAngle = (targetYAngle - currentYAngle) / animationCycles;
    deltaZAngle = (targetZAngle - currentZAngle) / animationCycles;
    deltaEyeX = (targetEyeX - currentEyeX) / animationCycles;
    deltaEyeY = (targetEyeY - currentEyeY) / animationCycles;
    deltaEyeZ = (targetEyeZ - currentEyeZ) / animationCycles;
    deltaAlpha = (targetAlpha - currentAlpha) / animationCycles;
}

- (void)zoom
{
    NSLog(@"Zooming in...\n\n");
    
    currentEyeZ   = 6;
    targetXAngle  = 0;
    targetYAngle  = 0;
    targetZAngle  = 0;
    
    [self computeAnimationDeltaSteps:120 /* 2 sec */];
}

- (void)rotateBack
{
    NSLog(@"Rotating back...\n\n");
    
    targetXAngle  = 0;
    targetZAngle  = 0;
    targetYAngle  += 90;
    
    [self computeAnimationDeltaSteps:60 /* 1 sec */];
    
    if (targetYAngle >= 360)
        targetYAngle -= 360;
}

- (void)rotate
{
    NSLog(@"Rotating...\n\n");
    
    targetXAngle  = 0;
    targetZAngle  = 0;
    targetYAngle  -= 90;
    
    [self computeAnimationDeltaSteps:60 /* 1 sec */];
    
    if (targetYAngle < 0)
        targetYAngle += 360;
}

- (void)scroll
{
    NSLog(@"Scrolling...\n\n");
    
    currentEyeY   = -1.5;
    targetXAngle  = 0;
    targetYAngle  = 0;
    targetZAngle  = 0;
    
    [self computeAnimationDeltaSteps:120];		
}

- (void)fadeIn
{
    NSLog(@"Fading in...\n\n");
    
    
    currentXAngle = -90;
    currentEyeZ   = 5.0;
    
    currentEyeY   = 4.5;
    targetXAngle  = 0;
    targetYAngle  = 0;
    targetZAngle  = 0;
    
    [self computeAnimationDeltaSteps:120];	
}

- (void)blendIn
{
    NSLog(@"Blending in...\n\n");
    
    targetXAngle = 0;
    targetYAngle = 0;
    targetZAngle = 0;
    currentAlpha = 0.0;
    targetAlpha  = 1.0;
    
    [self computeAnimationDeltaSteps:120];
}


@end


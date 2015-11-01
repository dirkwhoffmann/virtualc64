#import "C64GUI.h"
#import <CoreGraphics/CoreGraphics.h>

@implementation MyMetalView(Helper)

// --------------------------------------------------------------------------------
//                                    Graphics
// --------------------------------------------------------------------------------

- (NSImage *)screenshot
{
    NSImage *image = [MyMetalView imageFromTexture:textureFromEmulator
                                                x1:textureXStart
                                                y1:textureYStart
                                                x2:textureXEnd
                                                y2:textureYEnd];
    return image;
}

- (NSImage *)flipImage:(NSImage *)image
{
    if (!image)
        return nil;
    
    NSSize size = [image size];
    NSImage *newImage = [[NSImage alloc] initWithSize:size];
    
    if (image) {
        [NSGraphicsContext saveGraphicsState];
        [newImage lockFocus];
        
        NSAffineTransform* t = [NSAffineTransform transform];
        [t translateXBy:0 yBy:size.height];
        [t scaleXBy:1 yBy:-1];
        [t concat];
        
        [image drawInRect:NSMakeRect(0, 0, size.width,size.height)];
        
        [newImage unlockFocus];
        [NSGraphicsContext restoreGraphicsState];
    }
    
    return newImage;
}

- (NSImage *)expandImage:(NSImage *)image toSize:(NSSize)size
{
    if (!image)
        return nil;

    NSImage *newImage = [[NSImage alloc] initWithSize:size];
    
    if (image) {
        [NSGraphicsContext saveGraphicsState];
        [newImage lockFocus];
        
        NSAffineTransform* t = [NSAffineTransform transform];
        [t translateXBy:0 yBy:size.height];
        [t scaleXBy:1 yBy:-1];
        [t concat];
        
        [image drawInRect:NSMakeRect(0,0,size.width,size.height)
                 fromRect:NSMakeRect(0,0,[image size].width, [image size].height)
                operation:NSCompositeSourceOver fraction:1.0];
        
        [newImage unlockFocus];
        [NSGraphicsContext restoreGraphicsState];
    }
    
    return newImage;
}

- (id<MTLTexture>) textureFromImage:(NSImage *)image
{
    if (!image)
        return nil;
    
    NSRect imageRect = NSMakeRect(0, 0, image.size.width, image.size.height);
    CGImageRef imageRef = [image CGImageForProposedRect:&imageRect context:NULL hints:nil];
    
    // Create a suitable bitmap context for extracting the bits of the image
    NSUInteger width = CGImageGetWidth(imageRef);
    NSUInteger height = CGImageGetHeight(imageRef);
    
    if (width == 0 || height == 0)
        return nil;

    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    uint8_t *rawData = (uint8_t *)calloc(height * width * 4, sizeof(uint8_t));
    NSUInteger bytesPerPixel = 4;
    NSUInteger bytesPerRow = bytesPerPixel * width;
    NSUInteger bitsPerComponent = 8;
    CGContextRef bitmapContext = CGBitmapContextCreate(rawData, width, height,
                                                       bitsPerComponent, bytesPerRow, colorSpace,
                                                       kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
    CGColorSpaceRelease(colorSpace);
        
    // Flip the context so the positive Y axis points down
    CGContextTranslateCTM(bitmapContext, 0, height);
    CGContextScaleCTM(bitmapContext, 1, -1);
        
    CGContextDrawImage(bitmapContext, CGRectMake(0, 0, width, height), imageRef);
    CGContextRelease(bitmapContext);
    
    MTLTextureDescriptor *textureDescriptor =
    [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                       width:width
                                                      height:height
                                                   mipmapped:NO];
    id<MTLTexture> texture = [device newTextureWithDescriptor:textureDescriptor];
        
    MTLRegion region = MTLRegionMake2D(0, 0, width, height);
    [texture replaceRegion:region mipmapLevel:0 withBytes:rawData bytesPerRow:bytesPerRow];
        
    free(rawData);
        
    return texture;
}

- (id<MTLTexture>) defaultBackgroundTexture
{
    uint32_t data = 0xA0A0A0A0;
    
    MTLTextureDescriptor *textureDescriptor =
    [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                       width:1
                                                      height:1
                                                   mipmapped:NO];
    id<MTLTexture> texture = [device newTextureWithDescriptor:textureDescriptor];
    
    MTLRegion region = MTLRegionMake2D(0, 0, 1, 1);
    [texture replaceRegion:region mipmapLevel:0 withBytes:&data bytesPerRow:4];
    return texture;
}

static void releaseDataCallback(void *info, const void *data, size_t size)
{
    free((void *)data);
}

+ (NSImage *)imageFromTexture:(id<MTLTexture>)texture x1:(float)_x1 y1:(float)_y1 x2:(float)_x2 y2:(float)_y2
{
    NSAssert([texture pixelFormat] == MTLPixelFormatRGBA8Unorm,
             @"Pixel format of texture must be MTLPixelFormatBGRA8Unorm");
    
    CGSize imageSize = CGSizeMake([texture width] * (_x2 - _x1), [texture height] * (_y2 - _y1));
    size_t imageByteCount = imageSize.width * imageSize.height * 4;
    void *imageBytes = malloc(imageByteCount);
    NSUInteger bytesPerRow = imageSize.width * 4;
    MTLRegion region = MTLRegionMake2D([texture width] * _x1, [texture height] * _y1, imageSize.width, imageSize.height);
    [texture getBytes:imageBytes bytesPerRow:bytesPerRow fromRegion:region mipmapLevel:0];
    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, imageBytes, imageByteCount, releaseDataCallback);
    int bitsPerComponent = 8;
    int bitsPerPixel = 32;
    CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
    CGBitmapInfo bitmapInfo = kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big;
    CGColorRenderingIntent renderingIntent = kCGRenderingIntentDefault;
    CGImageRef imageRef = CGImageCreate(imageSize.width,
                                        imageSize.height,
                                        bitsPerComponent,
                                        bitsPerPixel,
                                        bytesPerRow,
                                        colorSpaceRef,
                                        bitmapInfo,
                                        provider,
                                        NULL,
                                        false,
                                        renderingIntent);
    
    NSImage *image = [[NSImage alloc] initWithCGImage:imageRef size:NSZeroSize];
    CFRelease(provider);
    CFRelease(colorSpaceRef);
    CFRelease(imageRef);
    
    return image;
}



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


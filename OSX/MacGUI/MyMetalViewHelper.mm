/*
 * Author: Dirk W. Hoffmann, 2016
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#import "C64GUI.h"
#import "ShaderTypes.h"
#import <CoreGraphics/CoreGraphics.h>

matrix_float4x4
vc64_matrix_identity()
{
    vector_float4 X = { 1, 0, 0, 0 };
    vector_float4 Y = { 0, 1, 0, 0 };
    vector_float4 Z = { 0, 0, 1, 0 };
    vector_float4 W = { 0, 0, 0, 1 };
    
    matrix_float4x4 identity = { X, Y, Z, W };
    
    return identity;
}

matrix_float4x4
vc64_matrix_from_perspective_fov_aspectLH(float fovY, float aspect, float nearZ, float farZ)
{
    // 1 / tan == cot
    float yscale = 1.0f / tanf(fovY * 0.5f);
    float xscale = yscale / aspect;
    float q = farZ / (farZ - nearZ);

    matrix_float4x4 m = {
        .columns[0] = { xscale, 0.0f, 0.0f, 0.0f },
        .columns[1] = { 0.0f, yscale, 0.0f, 0.0f },
        .columns[2] = { 0.0f, 0.0f, q, 1.0f },
        .columns[3] = { 0.0f, 0.0f, q * -nearZ, 0.0f }
    };
    
    return m;
}

matrix_float4x4
vc64_matrix_from_translation(float x, float y, float z)
{
    matrix_float4x4 m = matrix_identity_float4x4;
    m.columns[3] = (vector_float4) { x, y, z, 1.0 };
    return m;
}

matrix_float4x4
vc64_matrix_from_rotation(float radians, float x, float y, float z)
{
    vector_float3 v = vector_normalize(((vector_float3){x, y, z}));
    float cos = cosf(radians);
    float cosp = 1.0f - cos;
    float sin = sinf(radians);
    
    return (matrix_float4x4) {
        .columns[0] = {
            cos + cosp * v.x * v.x,
            cosp * v.x * v.y + v.z * sin,
            cosp * v.x * v.z - v.y * sin,
            0.0f,
        },
        
        .columns[1] = {
            cosp * v.x * v.y - v.z * sin,
            cos + cosp * v.y * v.y,
            cosp * v.y * v.z + v.x * sin,
            0.0f,
        },
        
        .columns[2] = {
            cosp * v.x * v.z + v.y * sin,
            cosp * v.y * v.z - v.x * sin,
            cos + cosp * v.z * v.z,
            0.0f,
        },
        
        .columns[3] = { 0.0f, 0.0f, 0.0f, 1.0f
        }
    };
}

@implementation MyMetalView(Helper)

- (id<MTLBuffer>)buildVertexBuffer:(id<MTLDevice>)device
{
    
    NSLog(@"MyMetalView::buildVertexBuffer (texture cut: %f %f %f %f)",
          textureXStart, textureXEnd, textureYStart, textureYEnd);
    const float dx = 0.64;
    const float dy = 0.48;
    const float dz = 0.64;
    const float bgx = 6.4;
    const float bgy = 4.8;
    const float bgz = -6.8;
    float positions[] =
    {
        // Background
        -bgx,  bgy, -bgz, 1,   0.0, 0.0,
        -bgx, -bgy, -bgz, 1,   0.0, 1.0,
         bgx, -bgy, -bgz, 1,   1.0, 1.0,
        
        -bgx,  bgy, -bgz, 1,   0.0, 0.0,
         bgx,  bgy, -bgz, 1,   1.0, 0.0,
         bgx, -bgy, -bgz, 1,   1.0, 1.0,

        // -Z
        -dx,  dy, -dz, 1,   textureXStart, textureYStart,
        -dx, -dy, -dz, 1,   textureXStart, textureYEnd,
         dx, -dy, -dz, 1,   textureXEnd, textureYEnd,
        
        -dx,  dy, -dz, 1,   textureXStart, textureYStart,
         dx,  dy, -dz, 1,   textureXEnd, textureYStart,
         dx, -dy, -dz, 1,   textureXEnd, textureYEnd,

        // +Z
        -dx,  dy,  dz, 1,   textureXEnd, textureYStart,
        -dx, -dy,  dz, 1,   textureXEnd, textureYEnd,
         dx, -dy,  dz, 1,   textureXStart, textureYEnd,
        
        -dx,  dy,  dz, 1,   textureXEnd, textureYStart,
         dx,  dy,  dz, 1,   textureXStart, textureYStart,
         dx, -dy,  dz, 1,   textureXStart, textureYEnd,

        // -X
        -dx,  dy, -dz, 1,   textureXEnd, textureYStart,
        -dx, -dy, -dz, 1,   textureXEnd, textureYEnd,
        -dx, -dy,  dz, 1,   textureXStart, textureYEnd,
        
        -dx,  dy, -dz, 1,   textureXEnd, textureYStart,
        -dx,  dy,  dz, 1,   textureXStart, textureYStart,
        -dx, -dy,  dz, 1,   textureXStart, textureYEnd,

        // +X
         dx,  dy, -dz, 1,   textureXStart, textureYStart,
         dx, -dy, -dz, 1,   textureXStart, textureYEnd,
         dx, -dy,  dz, 1,   textureXEnd, textureYEnd,
        
         dx,  dy, -dz, 1,   textureXStart, textureYStart,
         dx,  dy,  dz, 1,   textureXEnd, textureYStart,
         dx, -dy,  dz, 1,   textureXEnd, textureYEnd,

        // -Y
         dx, -dy, -dz, 1,   textureXStart, textureYStart,
        -dx, -dy, -dz, 1,   textureXStart, textureYEnd,
        -dx, -dy,  dz, 1,   textureXEnd, textureYEnd,
        
         dx, -dy, -dz, 1,   textureXStart, textureYStart,
         dx, -dy,  dz, 1,   textureXEnd, textureYStart,
        -dx, -dy,  dz, 1,   textureXEnd, textureYEnd,

        // +Y
        +dx, +dy, -dz, 1,   textureXStart, textureYStart,
        -dx, +dy, -dz, 1,   textureXStart, textureYEnd,
        -dx, +dy, +dz, 1,   textureXEnd, textureYEnd,

        +dx, +dy, -dz, 1,   textureXStart, textureYStart,
        -dx, +dy, +dz, 1,   textureXEnd, textureYEnd,
        +dx, +dy, +dz, 1,   textureXEnd, textureYStart,
        
        // 2D drawing quad
        -1,  1, 0, 1,  textureXStart, textureYStart,
        -1, -1, 0, 1,  textureXStart, textureYEnd,
         1, -1, 0, 1,  textureXEnd, textureYEnd,
        
        -1,  1, 0, 1,  textureXStart, textureYStart,
         1,  1, 0, 1,  textureXEnd, textureYStart,
         1, -1, 0, 1,  textureXEnd, textureYEnd,

    };
    
    return [device newBufferWithBytes:positions
                               length:sizeof(positions)
                              options:MTLResourceOptionCPUCacheModeDefault];
}

// --------------------------------------------------------------------------------
//                                    Graphics
// --------------------------------------------------------------------------------

- (NSImage *)screenshot
{
    [lock lock];
    
    NSImage *image = [MyMetalView imageWithTexture:_texture
                                                x1:textureXStart
                                                y1:textureYStart
                                                x2:textureXEnd
                                                y2:textureYEnd];
    
    [lock unlock];
    
    return image;
}

- (NSImage *)flipImage:(NSImage *)image
{
    assert(image != nil);
    
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

- (id<MTLTexture>) makeTexture:(NSImage *)image withDevice:(id <MTLDevice>)device
{
 
    // CGImageRef imageRef = [image CGImage];
    NSRect imageRect = NSMakeRect(0, 0, image.size.width, image.size.height);
    CGImageRef imageRef = [image CGImageForProposedRect:&imageRect context:NULL hints:nil];
    
    
    // Create a suitable bitmap context for extracting the bits of the image
    NSUInteger width = CGImageGetWidth(imageRef);
    NSUInteger height = CGImageGetHeight(imageRef);
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
        
    MTLTextureDescriptor *textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                                                                 width:width
                                                                                                height:height
                                                                                             mipmapped:NO];
    id<MTLTexture> texture = [device newTextureWithDescriptor:textureDescriptor];
        
    MTLRegion region = MTLRegionMake2D(0, 0, width, height);
    [texture replaceRegion:region mipmapLevel:0 withBytes:rawData bytesPerRow:bytesPerRow];
        
    free(rawData);
        
    return texture;
}

static void releaseDataCallback(void *info, const void *data, size_t size)
{
    free((void *)data);
}

+ (NSImage *)imageWithTexture:(id<MTLTexture>)texture x1:(float)_x1 y1:(float)_y1 x2:(float)_x2 y2:(float)_y2
{
    NSAssert([texture pixelFormat] == MTLPixelFormatRGBA8Unorm, @"Pixel format of texture must be MTLPixelFormatBGRA8Unorm");
    
    // CGSize imageSize = CGSizeMake([texture width] / 2, [texture height] / 2);
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
}

- (float)eyeY
{
    return currentEyeY;
}

- (void)setEyeY:(float)newY
{
    currentEyeY = targetEyeY = newY;
}

- (float)eyeZ
{
    return currentEyeZ;
}

- (void)setEyeZ:(float)newZ
{
    currentEyeZ = targetEyeZ = newZ;
}

- (void)updateAngles
{
    //if ([self animates]) {
        
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
        
    // }
    //else {
    //    drawEntireCube = false;
    //}
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
    
    currentEyeZ     = 6;
    targetXAngle    = 0;
    targetYAngle    = 0;
    targetZAngle    = 0;
    
    [self computeAnimationDeltaSteps:120 /* 2 sec */];
}

- (void)rotateBack
{
    NSLog(@"Rotating back...\n\n");
    
    targetXAngle   = 0;
    targetZAngle   = 0;
    targetYAngle   += 90;
    
    [self computeAnimationDeltaSteps:60 /* 1 sec */];
    
    if (targetYAngle >= 360)
        targetYAngle -= 360;
    
    drawEntireCube = true;
}

- (void)rotate
{
    NSLog(@"Rotating...\n\n");
    
    targetXAngle   = 0;
    targetZAngle   = 0;
    targetYAngle   -= 90;
    drawEntireCube = true;
    
    [self computeAnimationDeltaSteps:60 /* 1 sec */];
    
    if (targetYAngle < 0)
        targetYAngle += 360;
}

- (void)scroll
{
    NSLog(@"Scrolling...\n\n");
    
    currentEyeY    = -1.5;
    targetXAngle   = 0;
    targetYAngle   = 0;
    targetZAngle   = 0;
    
    [self computeAnimationDeltaSteps:120];		
}

- (void)fadeIn
{
    NSLog(@"Fading in...\n\n");
    
    
    currentXAngle  = -90;
    currentEyeZ    = 5.0;
    
    currentEyeY    = 4.5;
    targetXAngle   = 0;
    targetYAngle   = 0;
    targetZAngle   = 0;
    
    [self computeAnimationDeltaSteps:120];	
}

- (void)blendIn
{
    NSLog(@"Blending in...\n\n");
    
    /*
    currentXAngle  = -90;
    currentEyeZ    = 5.0;
    currentEyeY    = 4.5;
    */
    
    targetXAngle   = 0;
    targetYAngle   = 0;
    targetZAngle   = 0;
    currentAlpha = 0.0;
    targetAlpha = 1.0;
    
    [self computeAnimationDeltaSteps:120];
}


@end


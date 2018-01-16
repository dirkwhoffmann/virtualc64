/*
 * Author: Dirk W. Hoffmann, 2015
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
#import "MyMetalDefs.h"
#import "VirtualC64-Swift.h"

@implementation MyMetalView {
}

// -----------------------------------------------------------------------------------------------
//                                           Properties
// -----------------------------------------------------------------------------------------------

@synthesize library;
@synthesize queue;
@synthesize pipeline;
@synthesize depthState;
@synthesize _commandBuffer;
@synthesize _commandEncoder;
@synthesize _drawable;

@synthesize bgTexture;
@synthesize emulatorTexture;
@synthesize upscaledTexture;
@synthesize filteredTexture;
@synthesize depthTexture;

@synthesize positionBuffer;
@synthesize uniformBuffer2D;
@synthesize uniformBuffer3D;
@synthesize uniformBufferBg;

@synthesize metalLayer;
@synthesize layerWidth;
@synthesize layerHeight;
@synthesize layerIsDirty;

@synthesize currentXAngle;
@synthesize targetXAngle;
@synthesize deltaXAngle;
@synthesize currentYAngle;
@synthesize targetYAngle;
@synthesize deltaYAngle;
@synthesize currentZAngle;
@synthesize targetZAngle;
@synthesize deltaZAngle;
@synthesize currentEyeX;
@synthesize targetEyeX;
@synthesize deltaEyeX;
@synthesize currentEyeY;
@synthesize targetEyeY;
@synthesize deltaEyeY;
@synthesize currentEyeZ;
@synthesize targetEyeZ;
@synthesize deltaEyeZ;
@synthesize currentAlpha;
@synthesize targetAlpha;
@synthesize deltaAlpha;

@synthesize textureXStart;
@synthesize textureYStart;
@synthesize textureXEnd;
@synthesize textureYEnd;
@synthesize videoUpscaler;
@synthesize videoFilter;
@synthesize enableMetal;
@synthesize fullscreen;
@synthesize fullscreenKeepAspectRatio;
@synthesize drawC64texture;

// -----------------------------------------------------------------------------------------------
//                                         Initialization
// -----------------------------------------------------------------------------------------------

- (id)initWithCoder:(NSCoder *)c
{
    NSLog(@"MyMetalView::initWithCoder");
    
    if ((self = [super initWithCoder:c]) == nil) {
        NSLog(@"Error: Can't initiaize MetalView");
    }
    return self;
}

-(void)awakeFromNib
{
    NSLog(@"MyMetalView::awakeFromNib");
    
    // Create semaphore
    _inflightSemaphore = dispatch_semaphore_create(1);
    
    // Set initial scene position and drawing properties
    [self initAnimation];
    
    // Properties
    enableMetal = false;
    fullscreen = false;
    fullscreenKeepAspectRatio = true;
    drawC64texture = false;
    
    // Metal
    layerWidth = 0;
    layerHeight = 0;
    layerIsDirty = YES;
    
    positionBuffer = nil;
    uniformBuffer2D = nil;
    uniformBuffer3D = nil;
    uniformBufferBg = nil;
    
    bgTexture = nil;
    emulatorTexture = nil;
    upscaledTexture = nil;
    filteredTexture = nil;
    depthTexture = nil;
    
    // Check if machine is capable to run the Metal graphics interface
    if (!MTLCreateSystemDefaultDevice()) {
        NSAlert *alert = [[NSAlert alloc] init];
        [alert setIcon:[NSImage imageNamed:@"metal.png"]];
        [alert setMessageText:@"No suitable GPU hardware found"];
        [alert setInformativeText:@"VirtualC64 can only run on machines supporting the Metal graphics technology (2012 models and above)."];
        [alert addButtonWithTitle:@"Ok"];
        [alert runModal];
        [NSApp terminate:self];
    }
        
    // Register for drag and drop
    [self setupDragAndDrop];
}

- (void) dealloc
{
    [self cleanup];
}

-(void)cleanup
{
    NSLog(@"MyMetalView::cleanup");
}

// -----------------------------------------------------------------------------------------------
//                                           Drawing
// -----------------------------------------------------------------------------------------------

- (void)updateScreenGeometry
{
    if ([c64proxy isPAL]) {
        
        // PAL border will be 36 pixels wide and 34 pixels heigh
        textureXStart = (float)(PAL_LEFT_BORDER_WIDTH - 36.0) / (float)C64_TEXTURE_WIDTH;
        textureXEnd = (float)(PAL_LEFT_BORDER_WIDTH + PAL_CANVAS_WIDTH + 36.0) / (float)C64_TEXTURE_WIDTH;
        textureYStart = (float)(PAL_UPPER_BORDER_HEIGHT - 34.0) / (float)C64_TEXTURE_HEIGHT;
        textureYEnd = (float)(PAL_UPPER_BORDER_HEIGHT + PAL_CANVAS_HEIGHT + 34.0) / (float)C64_TEXTURE_HEIGHT;
        
    } else {
        
        // NTSC border will be 42 pixels wide and 9 pixels heigh
        textureXStart = (float)(NTSC_LEFT_BORDER_WIDTH - 42.0) / (float)C64_TEXTURE_WIDTH;
        textureXEnd = (float)(NTSC_LEFT_BORDER_WIDTH + NTSC_CANVAS_WIDTH + 42.0) / (float)C64_TEXTURE_WIDTH;
        textureYStart = (float)(NTSC_UPPER_BORDER_HEIGHT - 9) / (float)C64_TEXTURE_HEIGHT;
        textureYEnd = (float)(NTSC_UPPER_BORDER_HEIGHT + NTSC_CANVAS_HEIGHT + 9) / (float)C64_TEXTURE_HEIGHT;
    }
    
    // Enable this for debugging (will display the whole texture)
    /*
     textureXStart = 0.0;
     textureXEnd = 1.0;
     textureYStart = 0.0;
     textureYEnd = 1.0;
     */
    
    // Update texture coordinates in vertex buffer
    [self buildVertexBuffer];
}

- (void)updateTexture:(id<MTLCommandBuffer>) cmdBuffer
{
    if (!c64proxy) {
        NSLog(@"Can't access C64");
        return;
    }
    
    void *buf = [[c64proxy vic] screenBuffer];
    assert(buf != NULL);

    NSUInteger pixelSize = 4;
    NSUInteger width = NTSC_PIXELS;
    NSUInteger height = PAL_RASTERLINES;
    NSUInteger rowBytes = width * pixelSize;
    NSUInteger imageBytes = rowBytes * height;
    
    [emulatorTexture replaceRegion:MTLRegionMake2D(0,0,width,height)
                       mipmapLevel:0 slice:0 withBytes:buf
                       bytesPerRow:rowBytes bytesPerImage:imageBytes];
}

- (void)setFrame:(CGRect)frame
{
    // NSLog(@"MyMetalView::setFrame");

    [super setFrame:frame];
    layerIsDirty = YES;
}

- (void)reshapeWithFrame:(CGRect)frame
{
   //  NSLog(@"MetalLayer::reshapeWithFrame");
          
    CGFloat scale = [[NSScreen mainScreen] backingScaleFactor];
    CGSize drawableSize = self.bounds.size;
    
    drawableSize.width *= scale;
    drawableSize.height *= scale;
    
    metalLayer.drawableSize = drawableSize;
    
    [self reshape];
}

- (void)reshape
{
    CGSize drawableSize = [metalLayer drawableSize];

    if (layerWidth == drawableSize.width && layerHeight == drawableSize.height)
        return;

    layerWidth = drawableSize.width;
    layerHeight = drawableSize.height;

    // NSLog(@"MetalLayer::reshape (%f,%f)", drawableSize.width, drawableSize.height);
    
    // Rebuild matrices
    [self buildMatricesBg];
    [self buildMatrices2D];
    [self buildMatrices3D];
    
    // Rebuild depth buffer
    [self buildDepthBuffer];
}

- (void)drawScene2D
{
    if (![self startFrame])
        return;
    
    // Render quad
    [_commandEncoder setFragmentTexture:filteredTexture atIndex:0];
    [_commandEncoder setVertexBuffer:uniformBuffer2D offset:0 atIndex:1];
    [_commandEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:42 vertexCount:6 instanceCount:1];
    
    [self endFrame];
}

- (void)drawScene3D
{
    bool animates = [self animates];
    bool drawBackground = !fullscreen && (animates || !drawC64texture);
    
    if (animates) {
        [self updateAngles];
        [self buildMatrices3D];
    }
    
    if (![self startFrame])
        return;
    
    // Make texture transparent if emulator is halted
    Uniforms *frameData = (Uniforms *)[uniformBuffer3D contents];
    frameData->alpha = [c64proxy isHalted] ? 0.5 : currentAlpha;

    // Render background
    if (drawBackground) {
        [_commandEncoder setFragmentTexture:bgTexture atIndex:0];
        [_commandEncoder setVertexBuffer:uniformBufferBg offset:0 atIndex:1];
        [_commandEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:6 instanceCount:1];
    }
    
    // Render cube
    if (drawC64texture) {
        [_commandEncoder setFragmentTexture:filteredTexture atIndex:0];
        [_commandEncoder setVertexBuffer:uniformBuffer3D offset:0 atIndex:1];
        [_commandEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:6 vertexCount:(animates ? 24 : 6) instanceCount:1];
    }
    
    [self endFrame];
}

- (void)endFrame
{
    [_commandEncoder endEncoding];
    
    __block dispatch_semaphore_t block_sema = _inflightSemaphore;
    [_commandBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer) {
        dispatch_semaphore_signal(block_sema);
    }];

    if (_drawable) {
        [_commandBuffer presentDrawable:_drawable];
        [_commandBuffer commit];
    }
}

- (void)drawRect:(CGRect)rect
{    
    if (!c64proxy || !enableMetal)
        return;
    
    dispatch_semaphore_wait(_inflightSemaphore, DISPATCH_TIME_FOREVER);
    
    // Refresh size dependent items if needed
    if (layerIsDirty) {
        [self reshapeWithFrame:[self frame]];
        layerIsDirty = NO;
    }
    
    // Get drawable from layer
    if (!(_drawable = [metalLayer nextDrawable])) {
        NSLog(@"Metal drawable must not be nil");
        return;
    }
    
    // Draw scene
    [self updateTexture:_commandBuffer];
    if (fullscreen && !fullscreenKeepAspectRatio) {
        [self drawScene2D];
    } else {
        [self drawScene3D];
    }
}

@end

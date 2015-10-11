/*
    Copyright (C) 2015 Apple Inc. All Rights Reserved.
    See LICENSE.txt for this sample’s licensing information
    
    Abstract:
    MetalKit view controller that's setup as the MTKViewDelegate.
*/

#import <Cocoa/Cocoa.h>
#import <MetalKit/MetalKit.h>

@interface AAPLMetalKitEssentialsViewController : NSViewController <MTKViewDelegate>
{
#if 0
    IBOutlet MyController *controller;
    IBOutlet C64Proxy* c64proxy;
    
    C64 *c64; // DEPRECATED. GET RID OF THIS VARIABLE AND RENAME c64proxy to c64
#endif
    
    // Texture cut-out (fist and last visible texture coordinates)
    float textureXStart;
    float textureXEnd;
    float textureYStart;
    float textureYEnd;
}


@end
//
//  MetalViewController.h
//  V64
//
//  Created by Dirk Hoffmann on 09.10.15.
//
//

#import <Cocoa/Cocoa.h>
#import <MetalKit/MetalKit.h>

// Forward declaration
@class MyController;

// Graphics constants
const int C64_TEXTURE_WIDTH = 512;
const int C64_TEXTURE_HEIGHT= 512;
const int C64_TEXTURE_DEPTH = 4;


@interface MyMetalView : MTKView
{
    IBOutlet MyController *controller;
    IBOutlet C64Proxy* c64proxy;
    
    C64 *c64; // DEPRECATED. GET RID OF THIS VARIABLE AND RENAME c64proxy to c64
}

-(CVReturn)getFrameForTime:(const CVTimeStamp *)timeStamp flagsOut:(CVOptionFlags *)flagsOut;

@end
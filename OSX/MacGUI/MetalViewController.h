//
//  MetalViewController.h
//  V64
//
//  Created by Dirk Hoffmann on 09.10.15.
//
//

#ifndef MetalViewController_h
#define MetalViewController_h

// #import <Cocoa/Cocoa.h>
// #import <MetalKit/MetalKit.h>
#import "C64GUI.h"

// Forward declaration
@class MyController;
@class C64Proxy;
class c64;

@interface MetalViewController : NSViewController <MTKViewDelegate>
{
    IBOutlet MyController* controller;
    IBOutlet C64Proxy* c64proxy;
//     C64 *c64;
}

-(CVReturn)getFrameForTime:(const CVTimeStamp *)timeStamp flagsOut:(CVOptionFlags *)flagsOut;

@end

#endif
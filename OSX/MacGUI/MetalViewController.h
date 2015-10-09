//
//  MetalViewController.h
//  V64
//
//  Created by Dirk Hoffmann on 09.10.15.
//
//

#ifndef MetalViewController_h
#define MetalViewController_h

#import <Cocoa/Cocoa.h>
#import <MetalKit/MetalKit.h>


@interface MetalViewController : NSViewController <MTKViewDelegate>

-(CVReturn)getFrameForTime:(const CVTimeStamp *)timeStamp flagsOut:(CVOptionFlags *)flagsOut;

@end

#endif
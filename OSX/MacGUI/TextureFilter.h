//
//  TextureFilter.h
//  V64
//
//  Created by Dirk Hoffmann on 18.10.15.
//
//

#ifndef TextureFilter_h
#define TextureFilter_h

#import <Foundation/Foundation.h>
#import <MetalKit/MetalKit.h>

@interface TextureFilter : NSObject
{
    id <MTLComputePipelineState> kernel;
    MTLSize threadgroupSize;
    MTLSize threadgroupCount;
}

- (instancetype)initWithFunctionName:(NSString *)name device:(id <MTLDevice>)dev library:(id <MTLLibrary>)lib;
- (void)apply:(id <MTLCommandBuffer>)commandBuffer;

@end

#endif

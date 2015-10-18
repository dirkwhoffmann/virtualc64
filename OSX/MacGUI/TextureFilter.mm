//
//  TextureFilter.m
//  V64
//
//  Created by Dirk Hoffmann on 18.10.15.
//
//

#import <Foundation/Foundation.h>
#import "TextureFilter.h"

@implementation TextureFilter

- (instancetype)initWithFunctionName:(NSString *)name device:(id <MTLDevice>)dev library:(id <MTLLibrary>)lib
{
    NSError *error = nil;
    
    // Get kernel function from library
    id <MTLFunction> function = [lib newFunctionWithName:name];
    if (!function) {
        NSLog(@"ERROR: Cannot find kernel function %@ in library", name);
        exit(0);
    }
    
    // Create kernel
    kernel = [dev newComputePipelineStateWithFunction:function error:&error];
    if(!kernel) {
        NSLog(@"ERROR: Failed to create compute kernel %@: %@", name, error);
        exit(0);
    }

    // Set thread group size of 16x16
    threadgroupSize = MTLSizeMake(16 /* width */, 16 /* height */, 1 /* depth */);
    
    // Calculate the compute kernel's width and height
    NSUInteger threadCountX = (512 /* texture width */ + threadgroupSize.width -  1) / threadgroupSize.width;
    NSUInteger threadCountY = (512 /* texture height */ + threadgroupSize.height - 1) / threadgroupSize.height;
    
    // Set the kernel's thread count
    threadgroupCount = MTLSizeMake(threadCountX, threadCountY, 1);
    
    return self;
}

- (void)apply:(id <MTLCommandBuffer>)commandBuffer
{
    
}

@end

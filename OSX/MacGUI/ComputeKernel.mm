//
//  ComputeKernel.mm
//  V64
//
//  Created by Dirk Hoffmann on 18.10.15.
//
//

#import <Foundation/Foundation.h>
#import "ComputeKernel.h"

@implementation ComputeKernel

@synthesize sampler;

- (instancetype)initWithFunctionName:(NSString *)name
                              device:(id <MTLDevice>)dev
                             library:(id <MTLLibrary>)lib
{
    NSError *error = nil;
    
    // Lookup kernel function in library
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
    NSUInteger threadCountX = (1024 /* texture width */ + threadgroupSize.width -  1) / threadgroupSize.width;
    NSUInteger threadCountY = (1024 /* texture height */ + threadgroupSize.height - 1) / threadgroupSize.height;
    
    // Set the kernel's thread count
    threadgroupCount = MTLSizeMake(threadCountX, threadCountY, 1);
    
    // Build default texture sampler
    MTLSamplerDescriptor *samplerDescriptor = [MTLSamplerDescriptor new];
    {
        samplerDescriptor.minFilter = MTLSamplerMinMagFilterLinear;
        samplerDescriptor.magFilter = MTLSamplerMinMagFilterLinear;
        samplerDescriptor.sAddressMode = MTLSamplerAddressModeClampToEdge;
        samplerDescriptor.tAddressMode = MTLSamplerAddressModeClampToEdge;
        samplerDescriptor.mipFilter = MTLSamplerMipFilterNotMipmapped;
    }
    sampler = [dev newSamplerStateWithDescriptor:samplerDescriptor];

    return self;
}

- (void)configureComputeCommandEncoder:(id <MTLComputeCommandEncoder>)encoder
{
    // To be implemented by custom class
}

- (void)apply:(id <MTLCommandBuffer>)commandBuffer in:(id <MTLTexture>)i out:(id <MTLTexture>)o
{
    computeEncoder = [commandBuffer computeCommandEncoder];
    
    [computeEncoder setComputePipelineState:kernel];
    [computeEncoder setTexture:i atIndex:0];
    [computeEncoder setTexture:o atIndex:1];
    
    [self configureComputeCommandEncoder:computeEncoder];
    
    [computeEncoder dispatchThreadgroups:threadgroupCount threadsPerThreadgroup:threadgroupSize];
    [computeEncoder endEncoding];
    // sampling = TEX_SAMPLE_LINEAR;

}

@end

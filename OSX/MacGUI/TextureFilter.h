//
//  TextureFilter.h
//  V64
//
//  Created by Dirk Hoffmann on 18.10.15.
//
//

#import <Foundation/Foundation.h>
#import <MetalKit/MetalKit.h>

@interface TextureFilter : NSObject
{
    id <MTLComputeCommandEncoder> computeEncoder;
    id <MTLBuffer> uniformBuffer;
    id <MTLComputePipelineState> kernel;
    id <MTLSamplerState> sampler;
    
    MTLSize threadgroupSize;
    MTLSize threadgroupCount;
}

@property (readonly) id <MTLSamplerState> sampler;

- (instancetype)initWithFunctionName:(NSString *)name device:(id <MTLDevice>)dev library:(id <MTLLibrary>)lib;
- (void)configureComputeCommandEncoder:(id <MTLComputeCommandEncoder>)encoder;
- (void)apply:(id <MTLCommandBuffer>)commandBuffer in:(id <MTLTexture>)i out:(id <MTLTexture>)o;

@end


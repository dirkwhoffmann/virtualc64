//
//  BypassUpscaler.m
//  VirtualC64
//
//  Created by Dirk Hoffmann on 12.01.18.
//

#import "BypassUpscaler.h"

@implementation BypassUpscaler

+ (instancetype) forDevice:(id <MTLDevice>)dev fromLibrary:(id <MTLLibrary>)lib
{
    return [[self alloc] initWithDevice:dev library:lib];
}

- (instancetype)initWithDevice:(id <MTLDevice>)dev library:(id <MTLLibrary>)lib
{
    if (!(self = [super initWithFunctionName:@"bypassupscaler" device:dev library:lib]))
    return nil;
    
    // Replace default texture sampler
    MTLSamplerDescriptor *samplerDescriptor = [MTLSamplerDescriptor new];
    {
        samplerDescriptor.minFilter = MTLSamplerMinMagFilterNearest;
        samplerDescriptor.magFilter = MTLSamplerMinMagFilterNearest;
        samplerDescriptor.sAddressMode = MTLSamplerAddressModeClampToEdge;
        samplerDescriptor.tAddressMode = MTLSamplerAddressModeClampToEdge;
        samplerDescriptor.mipFilter = MTLSamplerMipFilterNotMipmapped;
    }
    sampler = [dev newSamplerStateWithDescriptor:samplerDescriptor];
    
    return self;
}

@end


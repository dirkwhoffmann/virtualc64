//
//  BypassFilter.m
//  V64
//
//  Created by Dirk Hoffmann on 18.10.15.
//
//

#import <BypassFilter.h>

@implementation BypassFilter

+ (instancetype)filterWithDevice:(id <MTLDevice>)dev library:(id <MTLLibrary>)lib
{
    return [[self alloc] initWithDevice:dev library:lib];
}

- (instancetype)initWithDevice:(id <MTLDevice>)dev library:(id <MTLLibrary>)lib
{
    if (!(self = [super initWithFunctionName:@"bypass" device:dev library:lib]))
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

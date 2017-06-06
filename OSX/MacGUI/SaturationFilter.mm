//
//  SaturationFilter.m
//  V64
//
//  Created by Dirk Hoffmann on 18.10.15.
//
//

#import "SaturationFilter.h"

@implementation SaturationFilter

+ (instancetype)filterWithFactor:(float)sat device:(id <MTLDevice>)dev library:(id <MTLLibrary>)lib
{
    return [[self alloc] initWithFactor:sat device:dev library:lib];
}

- (instancetype)initWithFactor:(float)sat device:(id <MTLDevice>)dev library:(id <MTLLibrary>)lib
{
    if (!(self = [super initWithFunctionName:@"saturation" device:dev library:lib]))
        return nil;
    
    saturation = sat;
    
    // Setup uniforms
    struct SaturationFilterUniforms uniforms;
    uniforms.saturationFactor = sat;
    uniformBuffer = [dev newBufferWithLength:sizeof(uniforms)
                                     options:MTLResourceOptionCPUCacheModeDefault];
    memcpy([uniformBuffer contents], &uniforms, sizeof(uniforms));
    return self;
}

- (void)configureComputeCommandEncoder:(id <MTLComputeCommandEncoder>)encoder
{
    // NSLog(@"SaturationFilter::configureCommandEncoder");
    [encoder setBuffer:uniformBuffer offset:0 atIndex:0];
}

@end

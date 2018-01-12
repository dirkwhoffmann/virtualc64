//
//  SaturationFilter.m
//  V64
//
//  Created by Dirk Hoffmann on 18.10.15.
//
//

#import "SaturationFilter.h"

@implementation SaturationFilter

+ (instancetype) withFactor:(float)sat forDevice:(id <MTLDevice>)dev fromLibrary:(id <MTLLibrary>)lib
{
    return [[self alloc] initWithFactor:sat forDevice:dev fromLibrary:lib];
}

- (instancetype)initWithFactor:(float)sat forDevice:(id <MTLDevice>)dev fromLibrary:(id <MTLLibrary>)lib
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

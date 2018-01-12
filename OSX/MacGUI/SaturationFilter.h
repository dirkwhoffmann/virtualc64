//
//  SaturationFilter.h
//  V64
//
//  Created by Dirk Hoffmann on 18.10.15.
//
//

#import "ComputeKernel.h"

struct SaturationFilterUniforms
{
    float saturationFactor;
};

@interface SaturationFilter : ComputeKernel
{
    float saturation;
}

+ (instancetype) withFactor:(float)sat forDevice:(id <MTLDevice>)dev fromLibrary:(id <MTLLibrary>)lib;

@end

//
//  SaturationFilter.h
//  V64
//
//  Created by Dirk Hoffmann on 18.10.15.
//
//

#import "TextureFilter.h"

struct SaturationFilterUniforms
{
    float saturationFactor;
};

@interface SaturationFilter : TextureFilter
{
    float saturation;
}

+ (instancetype)filterWithFactor:(float)sat device:(id <MTLDevice>)dev library:(id <MTLLibrary>)lib;

@end

//
//  BlurFilter.h
//  V64
//
//  Created by Dirk Hoffmann on 18.10.15.
//
//

#import "ComputeKernel.h"


@interface BlurFilter : ComputeKernel
{
    id<MTLTexture> blurWeightTexture;
}

+ (instancetype) withRadius:(float)radius forDevice:(id <MTLDevice>)dev fromLibrary:(id <MTLLibrary>)lib;

@end


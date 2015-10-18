//
//  BlurFilter.h
//  V64
//
//  Created by Dirk Hoffmann on 18.10.15.
//
//

#import "TextureFilter.h"


@interface BlurFilter : TextureFilter
{
    id<MTLTexture> blurWeightTexture;
}

+ (instancetype)filterWithRadius:(float)radius device:(id <MTLDevice>)dev library:(id <MTLLibrary>)lib;

@end


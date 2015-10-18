//
//  CrtFilter.h
//  V64
//
//  Created by Dirk Hoffmann on 18.10.15.
//
//

#import "TextureFilter.h"

@interface CrtFilter : TextureFilter
{
}

+ (instancetype)filterWithDevice:(id <MTLDevice>)dev library:(id <MTLLibrary>)lib;

@end
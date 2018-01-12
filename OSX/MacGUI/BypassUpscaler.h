//
//  BypassUpscaler.h
//  V64
//
//  Created by Dirk Hoffmann on 12.01.18.
//

#import "ComputeKernel.h"

@interface BypassUpscaler : ComputeKernel
{
}

+ (instancetype) forDevice:(id <MTLDevice>)dev fromLibrary:(id <MTLLibrary>)lib;

@end

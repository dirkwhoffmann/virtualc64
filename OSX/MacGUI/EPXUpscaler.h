//
//  EPXUpscaler.h
//  V64
//
//  Created by Dirk Hoffmann on 12.1.18.
//
//

#import "ComputeKernel.h"

@interface EPXUpscaler : ComputeKernel
{
}

+ (instancetype) forDevice:(id <MTLDevice>)dev fromLibrary:(id <MTLLibrary>)lib;

@end


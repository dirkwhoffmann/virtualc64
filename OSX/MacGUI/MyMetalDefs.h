//
//  MyMetalDefs.h
//  V64
//
//  Created by Dirk Hoffmann on 06.06.17.
//
//

#ifndef MYMETALDEFS_H
#define MYMETALDEFS_H

#import <simd/simd.h>
using namespace simd;

struct Uniforms {
    float4x4 modelViewProjection;
    float alpha;
};

#endif

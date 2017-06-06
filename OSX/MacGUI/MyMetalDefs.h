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
    float4x4 model;
    float4x4 view;
    float4x4 projection;
    float4x4 projectionView;
    float alpha;
};

#endif

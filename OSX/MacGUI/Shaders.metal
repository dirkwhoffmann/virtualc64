//
//  Shaders.metal
//  MetalTriangles
//
//  Created by Warren Moore on 8/26/14.
//  Copyright (c) 2014 Metal By Example. All rights reserved.
//

#include <metal_stdlib>
#include "ShaderTypes.h"

using namespace metal;

struct InVertex
{
    packed_float4 position [[attribute(0)]];
    packed_float2 texCoords [[attribute(1)]];
};

struct ProjectedVertex
{
    float4 position [[position]];
    float2 texCoords [[user(tex_coords)]];
};


vertex ProjectedVertex vertex_main(constant InVertex *vertices [[buffer(0)]],
                                   constant Uniforms &uniforms [[buffer(1)]],
                                   ushort vid [[vertex_id]])
{
#if 0
    vector_float4 X = { 1, 0, 0, 0 };
    vector_float4 Y = { 0, 1, 0, 0 };
    vector_float4 Z = { 0, 0, 1, 0 };
    vector_float4 W = { 0, 0, 0, 1 };
    matrix_float4x4 identity = { X, Y, Z, W };
#endif
    
    ProjectedVertex out;

    out.position = uniforms.projectionView * float4(vertices[vid].position);
    out.texCoords = vertices[vid].texCoords;
    return out;
}

fragment half4 fragment_main(ProjectedVertex vert [[stage_in]],
                                texture2d<float, access::sample> texture [[texture(0)]],
                                sampler texSampler [[sampler(0)]])
{
    float4 diffuseColor = texture.sample(texSampler, vert.texCoords);
    float4 color = diffuseColor;
    return half4(color.r, color.g, color.b, 1);
}

fragment half4 fragment_saturation(ProjectedVertex vert [[stage_in]],
                             texture2d<float, access::sample> texture [[texture(0)]],
                             sampler texSampler [[sampler(0)]])
{
    float4 inColor = texture.sample(texSampler, vert.texCoords);
    
    float value = dot(inColor.rgb, float3(0.299, 0.587, 0.114));
    float4 grayColor(value, value, value, 1.0);
    float4 outColor = mix(grayColor, inColor, 0.5);
    return half4(outColor.r, outColor.g, outColor.b, 1);
}

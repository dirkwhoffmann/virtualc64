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
    vector_float4 X = { 1, 0, 0, 0 };
    vector_float4 Y = { 0, 1, 0, 0 };
    vector_float4 Z = { 0, 0, 1, 0 };
    vector_float4 W = { 0, 0, 0, 1 };
    matrix_float4x4 identity = { X, Y, Z, W };

    
    ProjectedVertex out;

    // float4 in_position = float4(in.position, 1.0);
    // out.position = frameUniforms.projectionView * in_position;
    out.position = uniforms.projectionView * float4(vertices[vid].position);
    // out.position = identity * float4(vertices[vid].position);

    // out.normal = uniforms.normalMatrix * float4(vertices[vid].normal).xyz;
    // out.position = vertices[vid].position;
    out.texCoords = vertices[vid].texCoords;
    return out;
}

fragment half4 fragment_main(ProjectedVertex vert [[stage_in]],
                                texture2d<float, access::sample> texture [[texture(0)]],
                                sampler texSampler [[sampler(0)]])
{
    // float diffuseIntensity = max(0.33, dot(normalize(vert.normal), -kLightDirection));
    float4 diffuseColor = texture.sample(texSampler, vert.texCoords);
    // float4 color = diffuseColor * diffuseIntensity;
    float4 color = diffuseColor;
    return half4(color.r, color.g, color.b, 1);
    // return half4(0.7, 0.3, 0.6, 1);
}

#if 0
// will be run on each vertex ...
vertex ColoredVertex vertex_main(constant float4 *position [[buffer(0)]],
                                 constant float4 *color [[buffer(1)]],
                                 uint vid [[vertex_id]])
{
    ColoredVertex vert;
    vert.position = position[vid];
    vert.color = color[vid];
    return vert;
}

// will be run on each pixel ...
fragment float4 fragment_main(ColoredVertex vert [[stage_in]])
{
    return vert.color;
}
#endif

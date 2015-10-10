//
//  Shaders.metal
//  MetalTriangles
//
//  Created by Warren Moore on 8/26/14.
//  Copyright (c) 2014 Metal By Example. All rights reserved.
//

#include <metal_stdlib>
using namespace metal;

struct InVertex
{
    packed_float4 position [[attribute(0)]];
    packed_float2 texCoords [[attribute(2)]];
};

struct ProjectedVertex
{
    float4 position [[position]];
    float2 texCoords [[user(tex_coords)]];
};

#if 0
struct ColoredVertex
{
    float4 position [[position]];
    float4 color;
};
#endif

vertex ProjectedVertex vertex_main(constant InVertex *vertices [[buffer(0)]],
                                      // constant Uniforms &uniforms [[buffer(1)]],
                                      ushort vid [[vertex_id]])
{
    ProjectedVertex outVert;
    // outVert.position = uniforms.modelViewProjectionMatrix * float4(vertices[vid].position);
    // outVert.normal = uniforms.normalMatrix * float4(vertices[vid].normal).xyz;
    outVert.position = vertices[vid].position;
    outVert.texCoords = vertices[vid].texCoords;
    return outVert;
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

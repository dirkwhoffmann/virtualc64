//
//  Shaders.metal
//  MetalTriangles
//
//  Created by Warren Moore on 8/26/14.
//  Copyright (c) 2014 Metal By Example. All rights reserved.
//

using namespace metal;

struct ColoredVertex
{
    float4 position [[position]];
    float4 color;
};

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

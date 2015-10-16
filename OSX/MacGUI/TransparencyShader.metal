//
//  TransparencyShader.metal
//  V64
//
//  Created by Dirk Hoffmann on 16.10.15.
//
//

#include <metal_stdlib>

using namespace metal;

// Grayscale compute shader
kernel void transparency(texture2d<half, access::read>  inTexture   [[ texture(0) ]],
                         texture2d<half, access::write> outTexture  [[ texture(1) ]],
                         uint2                          gid         [[ thread_position_in_grid ]])
{
    if((gid.x < outTexture.get_width()) && (gid.y < outTexture.get_height()))
    {
        half4 inColor  = inTexture.read(gid);

//        half4 outColor = half4(inColor.x, inColor.x, inColor.z, 1.0);
        half4 outColor = half4(inColor.r, inColor.b, inColor.g, 0.5);
        
        outTexture.write(outColor, gid);
    }
}



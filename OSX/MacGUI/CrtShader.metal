//
//  CrtShader.metal
//  V64
//
//  Created by Dirk Hoffmann on 18.10.15.
//
//

#include <metal_stdlib>
using namespace metal;

kernel void crt(texture2d<half, access::read>  inTexture   [[ texture(0) ]],
                texture2d<half, access::write> outTexture  [[ texture(1) ]],
                uint2                          gid         [[ thread_position_in_grid ]])
{
    if((gid.x < outTexture.get_width()) && (gid.y < outTexture.get_height()))
    {
        half4 inColor = inTexture.read(gid);
        half line = (gid.y % 3) / 2.0;
        half4 grayColor(line,line,line,1.0);
        half4 result = mix(inColor, grayColor, half(0.15));
        
        outTexture.write(result, gid);
    }
}


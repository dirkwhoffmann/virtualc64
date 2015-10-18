//
//  BlurShader.metal
//  V64
//
//  Created by Dirk Hoffmann on 17.10.15.
//
//

#include <metal_stdlib>
using namespace metal;

kernel void blur(texture2d<float, access::read> inTexture [[texture(0)]],
                 texture2d<float, access::write> outTexture [[texture(1)]],
                 texture2d<float, access::read> weights [[texture(2)]],
                 uint2 gid [[thread_position_in_grid]])
{
    int size = weights.get_width();
    int radius = size / 2;
    
    float4 accumColor(0, 0, 0, 0);
    
    if((gid.x < outTexture.get_width()) && (gid.y < outTexture.get_height()))
    {
        for (int j = 0; j < size; ++j)
        {
            for (int i = 0; i < size; ++i)
            {
                uint2 kernelIndex(i, j);
                uint2 textureIndex(gid.x + (i - radius), gid.y + (j - radius));
                float4 color = inTexture.read(textureIndex).rgba;
                float4 weight = weights.read(kernelIndex).rrrr;
                accumColor += weight * color;
            }
        }
        
        outTexture.write(float4(accumColor.rgb, 1), gid);
    }
}

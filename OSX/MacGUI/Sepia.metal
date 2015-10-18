//
//  Sepia.metal
//  V64
//
//  Created by Dirk Hoffmann on 18.10.15.
//
//

#include <metal_stdlib>
using namespace metal;

constant half4 darkColor = half4(0.2, 0.05, 0.0, 1.0);
constant half4 lightColor = half4(1.0, 0.9, 0.5, 1.0);

kernel void sepia(texture2d<half, access::read>  inTexture   [[ texture(0) ]],
                  texture2d<half, access::write> outTexture  [[ texture(1) ]],
                  uint2                          gid         [[ thread_position_in_grid ]])
{
    if((gid.x < outTexture.get_width()) && (gid.y < outTexture.get_height()))
    {
        half4 inColor = inTexture.read(gid);
        half  brightness = dot(inColor.rgb, half3(0.299, 0.587, 0.114));
        half4 muted = mix(inColor, half4(brightness,brightness,brightness,1.0), half(0.5));
        half4 sepia = mix(darkColor, lightColor, brightness);
        half4 result = mix(muted, sepia, half(1.0));
        
        outTexture.write(result, gid);
    }
}
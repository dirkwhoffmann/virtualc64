#include <metal_stdlib>
using namespace metal;

struct SaturationFilterUniforms
{
    float saturationFactor;
};

// LUMA values for grayscale image conversion
constant half3 luma = half3(0.2126, 0.7152, 0.0722);
// constant half3 luma = half3(0.299, 0.587, 0.114);


// Grayscale compute shader
kernel void saturation(texture2d<half, access::read>  inTexture   [[ texture(0) ]],
                       texture2d<half, access::write> outTexture  [[ texture(1) ]],
                       constant SaturationFilterUniforms &uniforms [[buffer(0)]],
                       uint2                          gid         [[ thread_position_in_grid ]])
{
    if((gid.x < outTexture.get_width()) && (gid.y < outTexture.get_height()))
    {
        half  factor   = uniforms.saturationFactor;
        half4 inColor  = inTexture.read(gid);
        half  gray     = dot(inColor.rgb, luma);
        half4 grayColor(gray, gray, gray, 1.0);
        half4 outColor = mix(grayColor, inColor, factor);
        
        outTexture.write(outColor, gid);
    }
}

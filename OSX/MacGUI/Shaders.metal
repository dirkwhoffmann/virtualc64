//
//  Shaders.metal
//
//  Dirk W. Hoffmann, 18.10.2015
//

#include <metal_stdlib>

using namespace metal;

// -----------------------------------------------------------------------------------------------
//                        Main vertex shader (for drawing the quad)
// -----------------------------------------------------------------------------------------------

struct Uniforms {
    float4x4 model;
    float4x4 view;
    float4x4 projection;
    float4x4 projectionView;
    float alpha;
};

struct InVertex
{
    packed_float4 position [[attribute(0)]];
    packed_float2 texCoords [[attribute(1)]];
};

struct ProjectedVertex
{
    float4 position [[position]];
    float2 texCoords [[user(tex_coords)]];
    float  alpha;
};


vertex ProjectedVertex vertex_main(constant InVertex *vertices [[buffer(0)]],
                                   constant Uniforms &uniforms [[buffer(1)]],
                                   ushort vid [[vertex_id]])
{
    ProjectedVertex out;

    out.position = uniforms.projectionView * float4(vertices[vid].position);
    out.texCoords = vertices[vid].texCoords;
    out.alpha = uniforms.alpha;
    return out;
}

fragment half4 fragment_main(ProjectedVertex vert [[stage_in]],
                                texture2d<float, access::sample> texture [[texture(0)]],
                                sampler texSampler [[sampler(0)]])
{
    float4 diffuseColor = texture.sample(texSampler, vert.texCoords);
    float4 color = diffuseColor;
    return half4(color.r, color.g, color.b, vert.alpha);
}

// -----------------------------------------------------------------------------------------------
//                                      Bypass shader
// -----------------------------------------------------------------------------------------------

kernel void bypass(texture2d<half, access::read>  inTexture   [[ texture(0) ]],
                   texture2d<half, access::write> outTexture  [[ texture(1) ]],
                   uint2                          gid         [[ thread_position_in_grid ]])
{
#if 0
    if((gid.x % 2 == 0) && (gid.y % 2 == 0)) {
        if((gid.x < outTexture.get_width() - 1) && (gid.y < outTexture.get_height() - 1) &&
           (gid.x > 0) && (gid.y > 0)) {
            
            //   A    --\ 1 2
            // C P B  --/ 3 4
            //   D
            // 1=P; 2=P; 3=P; 4=P;
            // IF C==A AND C!=D AND A!=B => 1=A
            // IF A==B AND A!=C AND B!=D => 2=B
            // IF B==D AND B!=A AND D!=C => 4=D
            // IF D==C AND D!=B AND C!=A => 3=C
            half xx = gid.x / 2;
            half yy = gid.y / 2;
            half4 A = inTexture.read(uint2(xx, yy - 1));
            half4 C = inTexture.read(uint2(xx - 1, yy));
            half4 P = inTexture.read(uint2(xx, yy));
            half4 B = inTexture.read(uint2(xx + 1, yy));
            half4 D = inTexture.read(uint2(xx, yy + 1));
        
            bool b1 = true; // all(A == C);
            half4 r1 = mix(A,C,half(0.5));
            half4 r2 = mix(A,B,half(0.5));
            half4 r3 = mix(C,D,half(0.5));
            half4 r4 = mix(B,D,half(0.5));
            
            outTexture.write(r1, uint2(gid.x, gid.y));
            outTexture.write(r2, uint2(gid.x + 1, gid.y));
            outTexture.write(r3, uint2(gid.x, gid.y + 1));
            outTexture.write(r4, uint2(gid.x + 1, gid.y + 1));
        }

#endif
    if((gid.x < outTexture.get_width()) && (gid.y < outTexture.get_height()))
    {
        // uint2 newgid = uint2(gid.x / 2, gid.y / 2);
        // gid.x = gid.x / 2; gid.y = gid.y / 2;
        half4 result = inTexture.read(uint2(gid.x / 2, gid.y / 2));
        outTexture.write(result, gid);
    }
}


// -----------------------------------------------------------------------------------------------
//                                    Saturation shader
// -----------------------------------------------------------------------------------------------

struct SaturationFilterUniforms
{
    float saturationFactor;
};

// LUMA values for grayscale image conversion
constant half3 luma = half3(0.2126, 0.7152, 0.0722);
// constant half3 luma = half3(0.299, 0.587, 0.114);


kernel void saturation(texture2d<half, access::read>  inTexture    [[ texture(0) ]],
                       texture2d<half, access::write> outTexture   [[ texture(1) ]],
                       constant SaturationFilterUniforms &uniforms [[buffer(0)]],
                       uint2                          gid          [[ thread_position_in_grid ]])
{
    if((gid.x < outTexture.get_width()) && (gid.y < outTexture.get_height()))
    {
        half  factor   = uniforms.saturationFactor;
        half4 inColor  = inTexture.read(uint2(gid.x / 2, gid.y / 2)); // gid);
        half  gray     = dot(inColor.rgb, luma);
        half4 grayColor(gray, gray, gray, 1.0);
        half4 outColor = mix(grayColor, inColor, factor);
        
        outTexture.write(outColor, gid);
    }
}


// -----------------------------------------------------------------------------------------------
//                                    Gaussian blur shader 
// -----------------------------------------------------------------------------------------------

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
                // uint2 textureIndex(gid.x + (i - radius), gid.y + (j - radius));
                uint2 textureIndex((gid.x + (i - radius)) / 2, (gid.y + (j - radius)) / 2);
                float4 color = inTexture.read(textureIndex).rgba;
                float4 weight = weights.read(kernelIndex).rrrr;
                accumColor += weight * color;
            }
        }
        
        outTexture.write(float4(accumColor.rgb, 1), gid);
    }
}


// -----------------------------------------------------------------------------------------------
//                                     Sepia shader
// -----------------------------------------------------------------------------------------------

constant half4 darkColor = half4(0.2, 0.05, 0.0, 1.0);
constant half4 lightColor = half4(1.0, 0.9, 0.5, 1.0);

kernel void sepia(texture2d<half, access::read>  inTexture   [[ texture(0) ]],
                  texture2d<half, access::write> outTexture  [[ texture(1) ]],
                  uint2                          gid         [[ thread_position_in_grid ]])
{
    if((gid.x < outTexture.get_width()) && (gid.y < outTexture.get_height()))
    {
        half4 inColor = inTexture.read(uint2(gid.x / 2, gid.y / 2)); // gid);
        half  brightness = dot(inColor.rgb, half3(0.299, 0.587, 0.114));
        half4 muted = mix(inColor, half4(brightness,brightness,brightness,1.0), half(0.5));
        half4 sepia = mix(darkColor, lightColor, brightness);
        half4 result = mix(muted, sepia, half(1.0));
        
        outTexture.write(result, gid);
    }
}


// -----------------------------------------------------------------------------------------------
//                                     Simple CRT shader
// -----------------------------------------------------------------------------------------------

kernel void crt(texture2d<half, access::read>  inTexture   [[ texture(0) ]],
                texture2d<half, access::write> outTexture  [[ texture(1) ]],
                uint2                          gid         [[ thread_position_in_grid ]])
{
    if((gid.x < outTexture.get_width()) && (gid.y < outTexture.get_height()))
    {
        half4 inColor = inTexture.read(uint2(gid.x / 2, gid.y / 2)); // gid);
        half line = (gid.y % 2) / 1.0;
        half4 grayColor(line,line,line,1.0);
        half4 result = mix(inColor, grayColor, half(0.11));
        
        outTexture.write(result, gid);
    }
}



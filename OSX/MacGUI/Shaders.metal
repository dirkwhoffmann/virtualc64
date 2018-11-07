//
// This file is part of VirtualC64 - A user-friendly Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

#include <metal_stdlib>

using namespace metal;

#define SCALE_FACTOR 4

//
// Main vertex shader (for drawing the quad)
// 

struct Uniforms {
    float4x4 modelViewProjection;
    float alpha;
};

struct InVertex {
    float4 position [[attribute(0)]];
    float2 texCoords [[attribute(1)]];
};

struct ProjectedVertex {
    float4 position [[position]];
    float2 texCoords [[user(tex_coords)]];
    float  alpha;
};

struct FragmentUniforms {
    uint scanline;
    float scanlineBrightness;
    float scanlineWeight;
    float bloomFactor;
    uint mask;
    float maskBrightness;
};

vertex ProjectedVertex vertex_main(device InVertex *vertices [[buffer(0)]],
                                   constant Uniforms &uniforms [[buffer(1)]],
                                   ushort vid [[vertex_id]])
{
    ProjectedVertex out;

    out.position = uniforms.modelViewProjection * float4(vertices[vid].position);
    out.texCoords = vertices[vid].texCoords;
    out.alpha = uniforms.alpha;
    return out;
}

float4 scanlineWeight(uint2 pixel, float weight, float brightness, float bloom) {
    
    // Calculate distance to nearest scanline
    float dy = ((float(pixel.y % 6) - 2.5) / 2.5) / 4;
    
    // Calculate scanline weight
    float scanlineWeight = max(1.0 - dy * dy * weight, brightness);
    
    // Apply bloom effect an return
    return scanlineWeight * bloom;
}

float4 dotMaskWeight(int dotmaskType, uint2 pixel, float brightness) {
    
    float shadow = brightness * brightness;
    
    switch (dotmaskType) {
            
        case 1:
            switch(pixel.x % 3) {
                case 0: return float4(brightness, 1.0, brightness, 0.0);
                case 1: return float4(1.0, brightness, 1.0, 0.0);
                default: return float4(shadow, shadow, shadow, 0.0);
            }
        case 2:
            switch(pixel.x % 4) {
                case 0: return float4(1.0, brightness, brightness, 0.0);
                case 1: return float4(brightness, 1.0, brightness, 0.0);
                case 2: return float4(brightness, brightness, 1.0, 0.0);
                default: return float4(shadow, shadow, shadow, 0.0);
            }
        case 3:
            switch((pixel.x + ((pixel.y / 6) % 2)) % 3) {
                case 0: return float4(brightness, 1.0, brightness, 0.0);
                case 1: return float4(1.0, brightness, 1.0, 0.0);
                default: return float4(shadow, shadow, shadow, 0.0);
            }
        case 4:
            switch((pixel.x + ((pixel.y / 6) % 2)) % 4) {
                case 0: return float4(1.0, brightness, brightness, 0.0);
                case 1: return float4(brightness, 1.0, brightness, 0.0);
                case 2: return float4(brightness, brightness, 1.0, 0.0);
                default: return float4(shadow, shadow, shadow, 0.0);
            }
    }
    return float4(1.0, 1.0, 1.0, 0.0);
}

fragment half4 fragment_main(ProjectedVertex vert [[stage_in]],
                             texture2d<float, access::sample> texture [[texture(0)]],
                             constant FragmentUniforms &uniforms [[buffer(0)]],
                             sampler texSampler [[sampler(0)]])
{
    uint2 pixel = uint2(uint(vert.position.x), uint(vert.position.y));
    
    // Read fragment from texture
    float2 tc = float2(vert.texCoords.x, vert.texCoords.y);
    float4 color = texture.sample(texSampler, tc);
    
    // Apply scanline effect
    if (uniforms.scanline == 1) {
        color *= scanlineWeight(pixel,
                                uniforms.scanlineWeight,
                                uniforms.scanlineBrightness,
                                uniforms.bloomFactor);
    }
    
    // Apply dot mask effect
    color *= dotMaskWeight(uniforms.mask, pixel, uniforms.maskBrightness);

    return half4(color.r, color.g, color.b, vert.alpha);
}


//
// Texture upscalers (first post-processing stage)
//

kernel void bypassupscaler(texture2d<half, access::read>  inTexture   [[ texture(0) ]],
                           texture2d<half, access::write> outTexture  [[ texture(1) ]],
                           uint2                          gid         [[ thread_position_in_grid ]])
{
    half4 result = inTexture.read(uint2(gid.x / SCALE_FACTOR, gid.y / SCALE_FACTOR));
    outTexture.write(result, gid);
}


//
// EPX upscaler (Eric's Pixel Expansion)
//

void writePixelBlock(texture2d<half, access::write> outTexture, uint2 gid, half4 value)
{
    outTexture.write(value, gid + uint2(0,0));
    outTexture.write(value, gid + uint2(0,1));
    outTexture.write(value, gid + uint2(1,0));
    outTexture.write(value, gid + uint2(1,1));
}

kernel void epxupscaler(texture2d<half, access::read>  inTexture   [[ texture(0) ]],
                        texture2d<half, access::write> outTexture  [[ texture(1) ]],
                        uint2                          gid         [[ thread_position_in_grid ]])
{
    if((gid.x % 4 != 0) || (gid.y % 4 != 0))
        return;
    
    //   A    --\ 1 2
    // C P B  --/ 3 4
    //   D
    // 1=P; 2=P; 3=P; 4=P;
    // IF C==A AND C!=D AND A!=B => 1=A
    // IF A==B AND A!=C AND B!=D => 2=B
    // IF D==C AND D!=B AND C!=A => 3=C
    // IF B==D AND B!=A AND D!=C => 4=D

    half xx = gid.x / SCALE_FACTOR;
    half yy = gid.y / SCALE_FACTOR;
    half4 A = inTexture.read(uint2(xx, yy - 1));
    half4 C = inTexture.read(uint2(xx - 1, yy));
    half4 P = inTexture.read(uint2(xx, yy));
    half4 B = inTexture.read(uint2(xx + 1, yy));
    half4 D = inTexture.read(uint2(xx, yy + 1));
        
    half4 r1 = (all(C == A) && any(C != D) && any(A != B)) ? A : P;
    half4 r2 = (all(A == B) && any(A != C) && any(B != D)) ? B : P;
    half4 r3 = (all(A == B) && any(A != C) && any(B != D)) ? C : P;
    half4 r4 = (all(B == D) && any(B != A) && any(D != C)) ? D : P;

    writePixelBlock(outTexture, gid + uint2(0,0), r1);
    writePixelBlock(outTexture, gid + uint2(2,0), r2);
    writePixelBlock(outTexture, gid + uint2(0,2), r3);
    writePixelBlock(outTexture, gid + uint2(2,2), r4);
}

//
// xBR upscaler (4x)
//
// Code is based on what I've found at:
// https://gamedev.stackexchange.com/questions/87275/how-do-i-perform-an-xbr-or-hqx-filter-in-xna

constant half coef = 2.0;
constant half3 yuv_weighted = half3(14.352, 28.176, 5.472);

half4 df(half4 A, half4 B)
{
    return abs(A - B);
}

half4 weighted_distance(half4 a, half4 b, half4 c, half4 d,
                        half4 e, half4 f, half4 g, half4 h)
{
    return (df(a, b) + df(a, c) + df(d, e) + df(d, f) + 4.0 * df(g, h));
}

kernel void xbrupscaler(texture2d<half, access::read>  inTexture   [[ texture(0) ]],
                        texture2d<half, access::write> outTexture  [[ texture(1) ]],
                        uint2                          gid         [[ thread_position_in_grid ]])
{
    bool4 edr, edr_left, edr_up, px;        // px = pixel, edr = edge detection rule
    bool4 ir_lv1, ir_lv2_left, ir_lv2_up;
    bool4 nc;                               // new color
    bool4 fx, fx_left, fx_up;               // inequations of straight lines
        
    half2 fp = fract(half2(gid) / SCALE_FACTOR);
    uint2 ggid = gid / SCALE_FACTOR;
    
    half3 A  = inTexture.read(ggid + uint2(-1,-1)).xyz;
    half3 B  = inTexture.read(ggid + uint2( 0,-1)).xyz;
    half3 C  = inTexture.read(ggid + uint2( 1,-1)).xyz;
    half3 D  = inTexture.read(ggid + uint2(-1, 0)).xyz;
    half3 E  = inTexture.read(ggid + uint2( 0, 0)).xyz;
    half3 F  = inTexture.read(ggid + uint2( 1, 0)).xyz;
    half3 G  = inTexture.read(ggid + uint2(-1, 1)).xyz;
    half3 H  = inTexture.read(ggid + uint2( 0, 1)).xyz;
    half3 I  = inTexture.read(ggid + uint2( 1, 1)).xyz;
    half3 A1 = inTexture.read(ggid + uint2(-1,-2)).xyz;
    half3 C1 = inTexture.read(ggid + uint2( 1,-2)).xyz;
    half3 A0 = inTexture.read(ggid + uint2(-2,-1)).xyz;
    half3 G0 = inTexture.read(ggid + uint2(-2, 1)).xyz;
    half3 C4 = inTexture.read(ggid + uint2( 2,-1)).xyz;
    half3 I4 = inTexture.read(ggid + uint2( 2, 1)).xyz;
    half3 G5 = inTexture.read(ggid + uint2(-1, 2)).xyz;
    half3 I5 = inTexture.read(ggid + uint2( 1, 2)).xyz;
    half3 B1 = inTexture.read(ggid + uint2( 0,-2)).xyz;
    half3 D0 = inTexture.read(ggid + uint2(-2, 0)).xyz;
    half3 H5 = inTexture.read(ggid + uint2( 0, 2)).xyz;
    half3 F4 = inTexture.read(ggid + uint2( 2, 0)).xyz;
        
    half4 b = yuv_weighted * half4x3(B, D, H, F);
    half4 c = yuv_weighted * half4x3(C, A, G, I);
    half4 e = yuv_weighted * half4x3(E, E, E, E);
    half4 d = b.yzwx;
    half4 f = b.wxyz;
    half4 g = c.zwxy;
    half4 h = b.zwxy;
    half4 i = c.wxyz;
        
    half4 i4 = yuv_weighted * half4x3(I4, C1, A0, G5);
    half4 i5 = yuv_weighted * half4x3(I5, C4, A1, G0);
    half4 h5 = yuv_weighted * half4x3(H5, F4, B1, D0);
    half4 f4 = h5.yzwx;
        
    half4 Ao = half4(1.0, -1.0, -1.0, 1.0);
    half4 Bo = half4(1.0, 1.0, -1.0, -1.0);
    half4 Co = half4(1.5, 0.5, -0.5, 0.5);
    half4 Ax = half4(1.0, -1.0, -1.0, 1.0);
    half4 Bx = half4(0.5, 2.0, -0.5, -2.0);
    half4 Cx = half4(1.0, 1.0, -0.5, 0.0);
    half4 Ay = half4(1.0, -1.0, -1.0, 1.0);
    half4 By = half4(2.0, 0.5, -2.0, -0.5);
    half4 Cy = half4(2.0, 0.0, -1.0, 0.5);
        
    // These inequations define the line below which interpolation occurs.
    fx.x = (Ao.x * fp.y + Bo.x * fp.x > Co.x);
    fx.y = (Ao.y * fp.y + Bo.y * fp.x > Co.y);
    fx.z = (Ao.z * fp.y + Bo.z * fp.x > Co.z);
    fx.w = (Ao.w * fp.y + Bo.w * fp.x > Co.w);
        
    fx_left.x = (Ax.x * fp.y + Bx.x * fp.x > Cx.x);
    fx_left.y = (Ax.y * fp.y + Bx.y * fp.x > Cx.y);
    fx_left.z = (Ax.z * fp.y + Bx.z * fp.x > Cx.z);
    fx_left.w = (Ax.w * fp.y + Bx.w * fp.x > Cx.w);
        
    fx_up.x = (Ay.x * fp.y + By.x * fp.x > Cy.x);
    fx_up.y = (Ay.y * fp.y + By.y * fp.x > Cy.y);
    fx_up.z = (Ay.z * fp.y + By.z * fp.x > Cy.z);
    fx_up.w = (Ay.w * fp.y + By.w * fp.x > Cy.w);
        
    ir_lv1      = ((e != f) && (e != h));
    ir_lv2_left = ((e != g) && (d != g));
    ir_lv2_up   = ((e != c) && (b != c));
        
    half4 w1 = weighted_distance(e, c, g, i, h5, f4, h, f);
    half4 w2 = weighted_distance(h, d, i5, f, i4, b, e, i);
    half4 df_fg = df(f, g);
    half4 df_hc = df(h, c);
    half4 t1 = (coef * df_fg);
    half4 t2 = df_hc;
    half4 t3 = df_fg;
    half4 t4 = (coef * df_hc);
        
    edr      = (w1 < w2)  && ir_lv1;
    edr_left = (t1 <= t2) && ir_lv2_left;
    edr_up   = (t4 <= t3) && ir_lv2_up;
        
    nc = (edr && (fx || (edr_left && fx_left) || (edr_up && fx_up)));
        
    t1 = df(e, f);
    t2 = df(e, h);
    px = t1 <= t2;
        
    half3 res =
        nc.x ? px.x ? F : H :
        nc.y ? px.y ? B : F :
        nc.z ? px.z ? D : B :
        nc.w ? px.w ? H : D : E;
        
    outTexture.write(half4(res,1.0), gid);
}


//
// Texture filter (second post-processing stage)
//

//
// Bypass filter
//

kernel void bypass(texture2d<half, access::read>  inTexture   [[ texture(0) ]],
                   texture2d<half, access::write> outTexture  [[ texture(1) ]],
                   uint2                          gid         [[ thread_position_in_grid ]])
{
    half4 result = inTexture.read(uint2(gid.x, gid.y));
    outTexture.write(result, gid);
}

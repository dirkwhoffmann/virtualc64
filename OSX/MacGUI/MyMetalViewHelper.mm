/*
 * Author: Dirk W. Hoffmann, 2016
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#import "C64GUI.h"

matrix_float4x4
vc64_matrix_identity()
{
    vector_float4 X = { 1, 0, 0, 0 };
    vector_float4 Y = { 0, 1, 0, 0 };
    vector_float4 Z = { 0, 0, 1, 0 };
    vector_float4 W = { 0, 0, 0, 1 };
    
    matrix_float4x4 identity = { X, Y, Z, W };
    
    return identity;
}

matrix_float4x4
vc64_matrix_from_perspective_fov_aspectLH(float fovY, float aspect, float nearZ, float farZ)
{
    // 1 / tan == cot
    float yscale = 1.0f / tanf(fovY * 0.5f);
    float xscale = yscale / aspect;
    float q = farZ / (farZ - nearZ);
    
    matrix_float4x4 m = {
        .columns[0] = { xscale, 0.0f, 0.0f, 0.0f },
        .columns[1] = { 0.0f, yscale, 0.0f, 0.0f },
        .columns[2] = { 0.0f, 0.0f, q, 1.0f },
        .columns[3] = { 0.0f, 0.0f, q * -nearZ, 0.0f }
    };
    
    return m;
}

matrix_float4x4
vc64_matrix_from_translation(float x, float y, float z)
{
    matrix_float4x4 m = matrix_identity_float4x4;
    m.columns[3] = (vector_float4) { x, y, z, 1.0 };
    return m;
}

matrix_float4x4
vc64_matrix_from_rotation(float radians, float x, float y, float z)
{
    vector_float3 v = vector_normalize(((vector_float3){x, y, z}));
    float cos = cosf(radians);
    float cosp = 1.0f - cos;
    float sin = sinf(radians);
    
    return (matrix_float4x4) {
        .columns[0] = {
            cos + cosp * v.x * v.x,
            cosp * v.x * v.y + v.z * sin,
            cosp * v.x * v.z - v.y * sin,
            0.0f,
        },
        
        .columns[1] = {
            cosp * v.x * v.y - v.z * sin,
            cos + cosp * v.y * v.y,
            cosp * v.y * v.z + v.x * sin,
            0.0f,
        },
        
        .columns[2] = {
            cosp * v.x * v.z + v.y * sin,
            cosp * v.y * v.z - v.x * sin,
            cos + cosp * v.z * v.z,
            0.0f,
        },
        
        .columns[3] = { 0.0f, 0.0f, 0.0f, 1.0f
        }
    };
}

@implementation MyMetalView(Helper)

- (id<MTLBuffer>)buildVertexBuffer:(id<MTLDevice>)device
{
    
    NSLog(@"MyMetalView::buildVertexBuffer (texture cut: %f %f %f %f)",
          textureXStart, textureXEnd, textureYStart, textureYEnd);
    
    const float dx = 0.5;
    const float dy = 0.5;
    const float dz = 0.5;
    
    float positions[] =
    {
        // -Z
        -dx,  dy, -dz, 1,   textureXStart, textureYStart,
        -dx, -dy, -dz, 1,   textureXStart, textureYEnd,
         dx, -dy, -dz, 1,   textureXEnd, textureYEnd,
        
        -dx,  dy, -dz, 1,   textureXStart, textureYStart,
         dx,  dy, -dz, 1,   textureXEnd, textureYStart,
         dx, -dy, -dz, 1,   textureXEnd, textureYEnd,

        // +Z
        -dx,  dy,  dz, 1,   textureXStart, textureYStart,
        -dx, -dy,  dz, 1,   textureXStart, textureYEnd,
         dx, -dy,  dz, 1,   textureXEnd, textureYEnd,
        
        -dx,  dy,  dz, 1,   textureXStart, textureYStart,
         dx,  dy,  dz, 1,   textureXEnd, textureYStart,
         dx, -dy,  dz, 1,   textureXEnd, textureYEnd,

        // -X
        -dx,  dy, -dz, 1,   textureXStart, textureYStart,
        -dx, -dy, -dz, 1,   textureXStart, textureYEnd,
        -dx, -dy,  dz, 1,   textureXEnd, textureYEnd,
        
        -dx,  dy, -dz, 1,   textureXStart, textureYStart,
        -dx,  dy,  dz, 1,   textureXEnd, textureYStart,
        -dx, -dy,  dz, 1,   textureXEnd, textureYEnd,

        // +X
         dx,  dy, -dz, 1,   textureXStart, textureYStart,
         dx, -dy, -dz, 1,   textureXStart, textureYEnd,
         dx, -dy,  dz, 1,   textureXEnd, textureYEnd,
        
         dx,  dy, -dz, 1,   textureXStart, textureYStart,
         dx,  dy,  dz, 1,   textureXEnd, textureYStart,
         dx, -dy,  dz, 1,   textureXEnd, textureYEnd,

        // -Y
         dx, -dy, -dz, 1,   textureXStart, textureYStart,
        -dx, -dy, -dz, 1,   textureXStart, textureYEnd,
        -dx, -dy,  dz, 1,   textureXEnd, textureYEnd,
        
         dx, -dy, -dz, 1,   textureXStart, textureYStart,
         dx, -dy,  dz, 1,   textureXEnd, textureYStart,
        -dx, -dy,  dz, 1,   textureXEnd, textureYEnd,

        // +Y
        +dx, +dy, -dz, 1,   textureXStart, textureYStart,
        -dx, +dy, -dz, 1,   textureXStart, textureYEnd,
        -dx, +dy, +dz, 1,   textureXEnd, textureYEnd,

        +dx, +dy, -dz, 1,   textureXStart, textureYStart,
        -dx, +dy, +dz, 1,   textureXEnd, textureYEnd,
        +dx, +dy, +dz, 1,   textureXEnd, textureYStart,
    };
    
    return [device newBufferWithBytes:positions
                               length:sizeof(positions)
                              options:MTLResourceOptionCPUCacheModeDefault];
}

@end


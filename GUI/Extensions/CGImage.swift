// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

public extension CGImage {

    static func defaultBitmapInfo() -> CGBitmapInfo {

        let alpha = CGImageAlphaInfo.premultipliedLast.rawValue
        let bigEn32 = CGBitmapInfo.byteOrder32Big.rawValue

        return CGBitmapInfo(rawValue: alpha | bigEn32)
    }

    static func dataProvider(data: UnsafeMutableRawPointer, size: CGSize) -> CGDataProvider? {

        let dealloc: CGDataProviderReleaseDataCallback = {

            (info: UnsafeMutableRawPointer?, data: UnsafeRawPointer, size: Int) -> Void in

            // Core Foundation objects are memory managed, aren't they?
            return
        }

        return CGDataProvider(dataInfo: nil,
                              data: data,
                              size: 4 * Int(size.width) * Int(size.height),
                              releaseData: dealloc)
    }

    // Creates a CGImage from a raw data stream
    static func make(data: UnsafeMutableRawPointer, size: CGSize, bitmapInfo: CGBitmapInfo? = nil) -> CGImage? {


        let w = Int(size.width)
        let h = Int(size.height)

        return CGImage(width: w, height: h,
                       bitsPerComponent: 8,
                       bitsPerPixel: 32,
                       bytesPerRow: 4 * w,
                       space: CGColorSpaceCreateDeviceRGB(),
                       bitmapInfo: bitmapInfo ?? defaultBitmapInfo(),
                       provider: dataProvider(data: data, size: size)!,
                       decode: nil,
                       shouldInterpolate: false,
                       intent: CGColorRenderingIntent.defaultIntent)
    }

    static func make(texture: MTLTexture, region: MTLRegion, bitmapInfo: CGBitmapInfo? = nil) -> CGImage? {

        let w = region.size.width
        let h = region.size.height

        // Get texture data as a byte stream
        guard let data = malloc(4 * w * h) else { return nil; }
        texture.getBytes(data,
                         bytesPerRow: 4 * region.size.width,
                         from: region, // MTLRegionMake2D(x, y, w, h),
                         mipmapLevel: 0)

        return make(data: data, size: CGSize(width: w, height: h), bitmapInfo: bitmapInfo)
    }

    // Creates a CGImage from a MTLTexture
    static func make(texture: MTLTexture, rect: CGRect, bitmapInfo: CGBitmapInfo? = nil) -> CGImage? {

        // Compute texture cutout
        let x = Int(CGFloat(texture.width) * rect.minX)
        let y = Int(CGFloat(texture.height) * rect.minY)
        let w = Int(CGFloat(texture.width) * rect.width)
        let h = Int(CGFloat(texture.height) * rect.height)

        return make(texture: texture, region: MTLRegionMake2D(x, y, w, h), bitmapInfo: bitmapInfo)
    }
}

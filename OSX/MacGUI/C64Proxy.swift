/*!
 * @header      C64Proxy.swift
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @brief       Implements some Swift extension to Objective-C C64Proxy 
 * @copyright   2017 Dirk W. Hoffmann
 */
/*
 * (C) 2017 Dirk W. Hoffmann. All rights reserved.
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

// TODO: Move ProxyColors.mm stuff in here

import Foundation

public extension C64Proxy {

    func timetravelSnapshotImage(_ item: Int) -> NSImage {
        
        // print("\(#function)")
        
        var data = historicSnapshotImageData(item)
        let width = historicSnapshotImageWidth(item)
        let height = historicSnapshotImageHeight(item)
        let imageRep = NSBitmapImageRep(bitmapDataPlanes: &data,
                                        pixelsWide: width,
                                        pixelsHigh: height,
                                        bitsPerSample: 8,
                                        samplesPerPixel: 4,
                                        hasAlpha: true,
                                        isPlanar: false,
                                        colorSpaceName: NSCalibratedRGBColorSpace,
                                        bytesPerRow: 4*width,
                                        bitsPerPixel: 32)
        let image = NSImage(size: (imageRep?.size)!)
        image.addRepresentation(imageRep!)
        
        return makeGlossy(image: image)
    }
    
    // TODO: Add to NSImage instead
    func makeGlossy(image: NSImage) -> NSImage {
        
        let width  = image.size.width;
        let height = image.size.height;
        let glossy = NSImage(named: "glossy.png")
        let result = NSImage(size: image.size)
        let rect   = NSRect(x: 0, y: 0, width: width, height: height)
        
        result.lockFocus()
        image.draw(in: rect, from: NSZeroRect, operation: NSCompositeSourceOver, fraction: 1.0)
        glossy!.draw(in: rect, from: NSZeroRect, operation: NSCompositeSourceOver, fraction: 1.0)
        result.unlockFocus()

        return result
    }
}

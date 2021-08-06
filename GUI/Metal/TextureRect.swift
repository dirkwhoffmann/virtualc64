// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension Canvas {
 
    var texW: CGFloat { return CGFloat(TextureSize.original.width) }
    var texH: CGFloat { return CGFloat(TextureSize.original.height) }

    fileprivate func normalize(_ rect: CGRect) -> CGRect {
        
        return CGRect(x: rect.origin.x / texW,
                      y: rect.origin.y / texH,
                      width: rect.width / texW,
                      height: rect.height / texH)
    }
    
    // Returns the used texture area (including HBLANK and VBLANK)
    var entire: CGRect {
        
        let w = c64.vic.hPixels
        let h = c64.vic.vPixels
        
        return CGRect(x: 0, y: 0, width: w, height: h)
    }
    
    var entireNormalized: CGRect {
        
        return normalize(entire)
    }
    
    // Returns the largest visibile texture area (excluding HBLANK and VBLANK)
    var largestVisible: CGRect {
        
        if c64.vic.isPAL() {
            return CGRect(x: 104, y: 16, width: 487 - 104, height: 299 - 16)
        } else {
            return CGRect(x: 104, y: 16, width: 487 - 104, height: 249 - 16)
        }
    }

    var largestVisibleNormalized: CGRect {
        
        return normalize(largestVisible)
    }

    // Returns the visible texture area based on the zoom and center parameters
    var visible: CGRect {
        
        /*
         *       aw <--------- maxWidth --------> dw
         *    ah |-----|---------------------|-----|
         *     ^ |     bw                   cw     |
         *     | -  bh *<----- width  ------>*     -
         *     | |     ^                     ^     |
         *     | |     |                     |     |
         *     | |   height                height  |
         *     | |     |                     |     |
         *     | |     v                     v     |
         *     | -  ch *<----- width  ------>*     -
         *     v |                                 |
         *    dh |-----|---------------------|-----|
         *
         *      aw/ah - dw/dh = largest posible texture cutout
         *      bw/bh - cw/ch = currently used texture cutout
         */
        
        let max = largestVisible
        
        let width = (1 - CGFloat(renderer.config.hZoom)) * max.width
        let bw = max.minX + CGFloat(renderer.config.hCenter) * (max.width - width)
        let height = (1 - CGFloat(renderer.config.vZoom)) * max.height
        let bh = max.minY + CGFloat(renderer.config.vCenter) * (max.height - height)
        
        return CGRect(x: bw, y: bh, width: width, height: height)
    }
    
    var visibleNormalized: CGRect {
        
        return normalize(visible)
    }

    func updateTextureRect() {
        
        textureRect = visibleNormalized
    }
}

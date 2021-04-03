// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

extension Renderer {
 
    var texW: CGFloat { return CGFloat(TextureSize.original.width) }
    var texH: CGFloat { return CGFloat(TextureSize.original.height) }

    fileprivate func normalize(_ rect: CGRect) -> CGRect {
        
        return CGRect.init(x: rect.origin.x / texW,
                           y: rect.origin.y / texH,
                           width: rect.width / texW,
                           height: rect.height / texH)
    }
    
    var maxTextureRect: CGRect {
        
        if parent.c64.vic.isPAL() {
            return CGRect.init(x: 104, y: 16, width: 487 - 104, height: 299 - 16)
        } else {
            return CGRect.init(x: 104, y: 16, width: 487 - 104, height: 249 - 16)
        }
    }

    var maxTextureRectScaled: CGRect {
        
        return normalize(maxTextureRect)
    }

    func computeTextureRect() -> CGRect {
                
        // Display the whole texture if zoom is set
        if zoom { return CGRect.init(x: 0.0, y: 0.0, width: 1.0, height: 1.0) }
        
        let rect = maxTextureRect
        let aw = rect.minX
        let dw = rect.maxX
        let ah = rect.minY
        let dh = rect.maxY
        
        /*
        var aw: CGFloat, dw: CGFloat, ah: CGFloat, dh: CGFloat
        if parent.c64.vic.isPAL() {
            
            aw = CGFloat(104)
            dw = CGFloat(487)
            ah = CGFloat(16)
            dh = CGFloat(299)
            
        } else {
            
            aw = CGFloat(104)
            dw = CGFloat(487)
            ah = CGFloat(16)
            dh = CGFloat(249)
        }
        */
        
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
        let maxWidth = dw - aw
        let maxHeight = dh - ah
        
        let hZoom = CGFloat(config.hZoom)
        let vZoom = CGFloat(config.vZoom)
        let hCenter = CGFloat(config.hCenter)
        let vCenter = CGFloat(config.vCenter)

        /*
        let hZoom = CGFloat(0)
        let vZoom = CGFloat(0)
        let hCenter = CGFloat(0)
        let vCenter = CGFloat(0)
        */
        
        let width = (1 - hZoom) * maxWidth
        let bw = aw + hCenter * (maxWidth - width)
        let height = (1 - vZoom) * maxHeight
        let bh = ah + vCenter * (maxHeight - height)
        
        let texW = CGFloat(TextureSize.original.width)
        let texH = CGFloat(TextureSize.original.height)
        
        return CGRect.init(x: bw / texW,
                           y: bh / texH,
                           width: width / texW,
                           height: height / texH)
    }
     
    func updateTextureRect() {
        
        textureRect = computeTextureRect()
        buildVertexBuffer()
    }
}

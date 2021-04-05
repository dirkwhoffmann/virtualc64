// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

struct Vertex {
    
    // Coordinate
    var x, y, z: Float
    
    // Texture coordinate
    var tx, ty: Float
    
    init(x: Float, y: Float, z: Float, t: NSPoint) {
        
        self.x = x
        self.y = y
        self.z = z
        tx = Float(t.x)
        ty = Float(t.y)
    }
    
    func floatBuffer() -> [Float] {

        return [x, y, z, 1.0, tx, ty, 0.0, 0.0]
    }
}

class Node {
    
    let device: MTLDevice
    var vertexCount: Int
    var vertexBuffer: MTLBuffer
    
    init(device: MTLDevice, vertices: [Vertex]) {

        var vertexData = [Float]()
        for vertex in vertices {
            vertexData += vertex.floatBuffer()
        }
        
        let dataSize = vertexData.count * MemoryLayout.size(ofValue: vertexData[0])
        vertexBuffer = device.makeBuffer(bytes: vertexData,
                                         length: dataSize,
                                         options: [])!
        self.device = device
        vertexCount = vertices.count
    }
    
    convenience init(device: MTLDevice,
                     x: Float, y: Float, z: Float, w: Float, h: Float,
                     t: NSRect = NSRect.init(x: 0, y: 0, width: 1.0, height: 1.0)) {
        
        let upperLeft = NSPoint(x: t.minX, y: t.minY)
        let lowerLeft = NSPoint(x: t.minX, y: t.maxY)
        let upperRight = NSPoint(x: t.maxX, y: t.minY)
        let lowerRight = NSPoint(x: t.maxX, y: t.maxY)
        
        let v0 = Vertex(x: x, y: y, z: z, t: lowerLeft)
        let v1 = Vertex(x: x, y: y + h, z: z, t: upperLeft)
        let v2 = Vertex(x: x + w, y: y + h, z: z, t: upperRight)
        
        let v3 = Vertex(x: x, y: y, z: z, t: lowerLeft)
        let v4 = Vertex(x: x + w, y: y + h, z: z, t: upperRight)
        let v5 = Vertex(x: x + w, y: y, z: z, t: lowerRight)
        
        self.init(device: device, vertices: [v0, v1, v2, v3, v4, v5])
    }
 
    convenience init(device: MTLDevice,
                     x: Float, y: Float, z: Float, w: Float, d: Float, t: NSRect) {
        
        let upperLeft = NSPoint(x: t.minX, y: t.minY)
        let lowerLeft = NSPoint(x: t.minX, y: t.maxY)
        let upperRight = NSPoint(x: t.maxX, y: t.minY)
        let lowerRight = NSPoint(x: t.maxX, y: t.maxY)
        
        let v0 = Vertex(x: x, y: y, z: z, t: lowerLeft)
        let v1 = Vertex(x: x, y: y, z: z + d, t: upperLeft)
        let v2 = Vertex(x: x + w, y: y, z: z + d, t: upperRight)
        
        let v3 = Vertex(x: x, y: y, z: z, t: lowerLeft)
        let v4 = Vertex(x: x + w, y: y, z: z + d, t: upperRight)
        let v5 = Vertex(x: x + w, y: y, z: z, t: lowerRight)
        
        self.init(device: device, vertices: [v0, v1, v2, v3, v4, v5])
    }
 
    convenience init(device: MTLDevice,
                     x: Float, y: Float, z: Float, h: Float, d: Float, t: NSRect) {
        
        let upperLeft = NSPoint(x: t.minX, y: t.minY)
        let lowerLeft = NSPoint(x: t.minX, y: t.maxY)
        let upperRight = NSPoint(x: t.maxX, y: t.minY)
        let lowerRight = NSPoint(x: t.maxX, y: t.maxY)
        
        let v0 = Vertex(x: x, y: y, z: z, t: lowerLeft)
        let v1 = Vertex(x: x, y: y + h, z: z, t: upperLeft)
        let v2 = Vertex(x: x, y: y + h, z: z + d, t: upperRight)
        
        let v3 = Vertex(x: x, y: y, z: z, t: lowerLeft)
        let v4 = Vertex(x: x, y: y + h, z: z + d, t: upperRight)
        let v5 = Vertex(x: x, y: y, z: z + d, t: lowerRight)
        
        self.init(device: device, vertices: [v0, v1, v2, v3, v4, v5])
    }
    
    func drawPrimitives(_ commandEncoder: MTLRenderCommandEncoder, count: Int) {
        
        commandEncoder.setVertexBuffer(vertexBuffer, offset: 0, index: 0)
        commandEncoder.drawPrimitives(type: .triangle,
                                      vertexStart: 0,
                                      vertexCount: count,
                                      instanceCount: 1)
    }

    func drawPrimitives(_ commandEncoder: MTLRenderCommandEncoder) {
        
        drawPrimitives(commandEncoder, count: vertexCount)
    }
}

class Quad {
    
    var front: Node
    var back: Node
    var left: Node
    var right: Node
    var top: Node
    var bottom: Node
    
    init(device: MTLDevice,
         x1: Float, y1: Float, z1: Float,
         x2: Float, y2: Float, z2: Float, t: NSRect) {
        
        front  = Node.init(device: device,
                           x: x1, y: y1, z: z1, w: (x2 - x1), h: (y2 - y1), t: t)
        back   = Node.init(device: device,
                           x: x2, y: y1, z: z2, w: (x1 - x2), h: (y2 - y1), t: t)
        left   = Node.init(device: device,
                           x: x1, y: y1, z: z2, h: (y2 - y1), d: (z1 - z2), t: t)
        right  = Node.init(device: device,
                           x: x2, y: y1, z: z1, h: (y2 - y1), d: (z2 - z1), t: t)
        top    = Node.init(device: device,
                           x: x1, y: y2, z: z1, w: (x2 - x1), d: (y2 - y1), t: t)
        bottom = Node.init(device: device,
                           x: x1, y: y1, z: z2, w: (x2 - x1), d: (y1 - y2), t: t)
    }

    func draw(_ commandEncoder: MTLRenderCommandEncoder, allSides: Bool) {
        
        front.drawPrimitives(commandEncoder)
        
        if allSides {
            
            back.drawPrimitives(commandEncoder)
            left.drawPrimitives(commandEncoder)
            right.drawPrimitives(commandEncoder)
            top.drawPrimitives(commandEncoder)
            bottom.drawPrimitives(commandEncoder)
        }
    }
}

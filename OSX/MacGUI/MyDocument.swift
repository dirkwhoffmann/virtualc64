//
//  MyDocument.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 31.01.18.
//

import Foundation

extension MyDocument {

//     override init() {
    @objc public func initSwift() {

        NSLog("MyDocument::\(#function)")
        
        // super.init()

        // Create emulator instance
        c64 = C64Proxy()
        
        attachedSnapshot = nil
        attachedArchive = nil
        attachedTape = nil
        attachedCartridge = nil
    }
    
    /*
    convenience init(type typeName: String) throws {
        self.init()
        // Rest of initialization code here
    }
    */
    
    override open func makeWindowControllers() {
        
        NSLog("\(#function)")
        
        let nibName = NSNib.Name(rawValue: "MyDocument")
        let controller = MyController.init(windowNibName: nibName)
        controller.c64 = c64
        self.addWindowController(controller)
    }
    

    //
    // Loading and saving
    //
    
/*
    override open func data(ofType typeName: String) throws -> Data {
        
        NSLog("data(ofType:\(typeName))")
        
        if typeName == "VC64" {

            NSLog("Type is VC64")
            
            // Better: We should be able to write:
            // let snapshot = SnapshotProxy(c64)

            let snapshot = SnapshotProxy()!
            c64.save(toSnapshot: snapshot)

            let data = NSMutableData.init(length: snapshot.sizeOnDisk())
            let ptr = data!.mutableBytes.assumingMemoryBound(to: UInt8.self)
            snapshot.write(toBuffer: ptr)
            return data! as Data
        }
        
        throw NSError(domain: NSOSStatusErrorDomain, code: unimpErr, userInfo: nil)
    }
*/
    
/*
    override open func read(from data: Data, ofType typeName: String) throws {
        
        NSLog("read(from data: ofType:\(typeName))")
        
        NSLog("\(#function)")
        
        if typeName == "VC64" {
            
            NSLog("Type is VC64")
            
            let size = data.count
            let nsData = data as NSData
            let ptr = nsData.bytes
            
            NSLog("size = \(size)")
            
            let snapshot = SnapshotProxy.snapshot(fromBuffer: ptr, length: UInt32(size))
            c64.load(fromSnapshot: snapshot)
            return
        
        }
        
        throw NSError(domain: NSOSStatusErrorDomain, code: unimpErr, userInfo: nil)
    }
*/
    
    open override func removeWindowController(_ windowController: NSWindowController) {
        
        NSLog("MyDocument:\(#function)")

        super.removeWindowController(windowController)
        c64.kill()
    }
}


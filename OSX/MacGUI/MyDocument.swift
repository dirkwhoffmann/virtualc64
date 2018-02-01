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

        // Create emulator instance and try to load ROMs
        c64 = C64Proxy()
        let defaults = UserDefaults.standard
        loadRom(defaults.string(forKey: VC64BasicRomFileKey))
        loadRom(defaults.string(forKey: VC64CharRomFileKey))
        loadRom(defaults.string(forKey: VC64KernelRomFileKey))
        loadRom(defaults.string(forKey: VC64VC1541RomFileKey))

        // Try to get rid of this!!!
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
    
    @discardableResult
    @objc func loadRom(_ filename: String?) -> Bool {
        
        if (filename == nil) {
            return false
        }
        
        let defaults = UserDefaults.standard
        
        if c64.loadBasicRom(filename!) {
            NSLog("Basic ROM:  \(filename!)")
            defaults.set(filename, forKey: VC64BasicRomFileKey)
            return true
        }
        if c64.loadCharRom(filename!) {
            NSLog("Char ROM:   \(filename!)")
            defaults.set(filename, forKey: VC64CharRomFileKey)
            return true
        }
        if c64.loadKernelRom(filename!) {
            NSLog("Kernel ROM: \(filename!)")
            defaults.set(filename, forKey: VC64KernelRomFileKey)
            return true
        }
        if c64.loadVC1541Rom(filename!) {
            NSLog("VC1541 ROM: \(filename!)")
            defaults.set(filename, forKey: VC64VC1541RomFileKey)
            return true
        }
    
    return false
    }
    
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


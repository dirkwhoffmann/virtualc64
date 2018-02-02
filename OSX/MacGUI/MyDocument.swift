//
//  MyDocument.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 31.01.18.
//

import Foundation

class MyDocument : NSDocument {

    // ObjC/C++ bridge
    // C64Proxy *__strong c64;
    var c64: C64Proxy!
    
    //! Reference to an attached VC64 snapshot
    /*! When a new documents opens and this variable is not NULL, the snapshot is automatically flashed */
    @objc var attachedSnapshot: SnapshotProxy? = nil
    
    //! Reference to an attached D64, G64, or NIB archive
    /*! When a new documents opens and this variable is not NULL, the archive is automatically inserted into the virtual floopy drive */
    @objc var attachedArchive: ArchiveProxy? = nil
    
    //! Reference to an attached TAP container
    /*! When a new documents opens and this variable is not NULL, the tape is automatically inserted into the virtual datasette */
    @objc var attachedTape: TAPContainerProxy? = nil
    
    //! Reference to an attached CRT container
    /*! When a new documents opens and this variable is not NULL, the cartridge is automatically plugged into the virtual expansion port */
    @objc var attachedCartridge: CRTContainerProxy? = nil
    
    override init() {

        NSLog("MyDocument::\(#function)")
        
        super.init()
        
        // Create emulator instance and try to load ROMs
        c64 = C64Proxy()
        let defaults = UserDefaults.standard
        loadRom(defaults.string(forKey: VC64BasicRomFileKey))
        loadRom(defaults.string(forKey: VC64CharRomFileKey))
        loadRom(defaults.string(forKey: VC64KernelRomFileKey))
        loadRom(defaults.string(forKey: VC64VC1541RomFileKey))


        attachedSnapshot = nil
        attachedArchive = nil
        attachedTape = nil
        attachedCartridge = nil
 
    }
 
    /*
    @objc func getAttachedArchive() -> String {
        return attachedArchive
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
    
    override open func read(from url: URL, ofType typeName: String) throws {
        
        let filename = url.path
        NSLog("MyDocument::\(#function):\(filename)")
    
        // Is it a snapshot from a different version?
        if SnapshotProxy.isUsupportedSnapshot(filename) {
            showSnapshotVersionAlert()
            throw NSError(domain: NSOSStatusErrorDomain, code: unimpErr, userInfo: nil)
        }
        
        // Is it a snapshop with a matching version number?
        attachedSnapshot = SnapshotProxy.snapshot(fromFile: filename)
        if attachedSnapshot != nil {
            NSLog("Successfully read snapshot.")
            return
        }
        
        // Is it an archive?
        attachedArchive = ArchiveProxy.makeArchive(fromFile: filename)
        if attachedArchive != nil {
            NSLog("Successfully read archive.")
            fileURL = nil // Make the document 'Untitled'
            return
        }
 
        // Is it a magnetic tape?
        attachedTape = TAPContainerProxy.container(fromTAPFile: filename)
        if attachedTape != nil {
            NSLog("Successfully read tape.")
            fileURL = nil
            return
        }
        
        // Is it a cartridge?
        attachedCartridge = CRTContainerProxy.container(fromCRTFile: filename)
        if attachedCartridge != nil {
            NSLog("Successfully read cartridge.")
            fileURL = nil
            return
        }
    
    NSLog("Unable to read file\n")
    }
    
    override open func revert(toContentsOf url: URL, ofType typeName: String) throws {
       
        let filename = url.path
        NSLog("MyDocument::\(#function):\(filename)")
        
        if typeName != "VC64" {
            NSLog("Document type is \(typeName), expected VC64")
            throw NSError(domain: NSOSStatusErrorDomain, code: unimpErr, userInfo: nil)
        }
        
        guard let snapshot = SnapshotProxy.snapshot(fromFile: filename) else {
            NSLog("Error while trying to revert to older snapshopt")
            throw NSError(domain: NSOSStatusErrorDomain, code: unimpErr, userInfo: nil)
        }
        
        c64.load(fromSnapshot: snapshot)
    }
    
    override open func write(to url: URL, ofType typeName: String) throws {
   
        let filename = url.path
        NSLog("MyDocument::\(#function):\(filename)")
    
        if typeName != "VC64" {
            NSLog("Document type is \(typeName), expected VC64")
            throw NSError(domain: NSOSStatusErrorDomain, code: unimpErr, userInfo: nil)
        }

        let snapshot = SnapshotProxy()
        c64.save(toSnapshot: snapshot)
        snapshot?.writeData(toFile: filename)
    }
 
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


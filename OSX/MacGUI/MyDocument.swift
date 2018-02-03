/*
 * (C) 2018 Dirk W. Hoffmann. All rights reserved.
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

import Foundation

class MyDocument : NSDocument {
    
    //! @brief   Emulator proxy object
    /*! @details This object is an objC object bridging between the GUI (written in Swift)
     *           an the core emulator (written in C++).
     */
    var c64: C64Proxy!
    
    // TODO: Merge the following four variables into one:
    // var Container attachment: ContainerProxy? = nil
    
    //! @brief   Reference to an attached snapshot image
    /*! @details When the GUI launches, it checks for this object. If set, the attached
     *           snapshot is loaded into the emulator.
     */
    @objc var attachedSnapshot: SnapshotProxy? = nil
    
    //! Reference to an attached D64, G64, or NIB archive
    @objc var attachedArchive: ArchiveProxy? = nil
    
    //! Reference to an attached TAP container
    @objc var attachedTape: TAPContainerProxy? = nil
    
    //! Reference to an attached CRT container
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
    }
 
    override open func makeWindowControllers() {
        
        NSLog("\(#function)")
        
        let nibName = NSNib.Name(rawValue: "MyDocument")
        let controller = MyController.init(windowNibName: nibName)
        controller.c64 = c64
        self.addWindowController(controller)
    }
    
    //
    // Loading
    //
    
    /*
    override open func read(from url: URL, ofType typeName: String) throws {
        
        let filename = url.path
        NSLog("MyDocument::\(#function):\(filename)")
    
        // Is it a snapshot from a different version?
        if SnapshotProxy.isUsupportedSnapshotFile(filename) {
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
    */
    
    override open func read(from data: Data, ofType typeName: String) throws {
        
        let size = data.count
        let ptr = (data as NSData).bytes
        
        track("typeName: \(typeName) (\(size) bytes)")
        
        switch (typeName) {
        
        case "VC64":
            
            NSLog("Type is VC64")
            let snapshot = SnapshotProxy.makeSnapshot(withBuffer: ptr, length: size)
            c64.load(fromSnapshot: snapshot)
            return
        
        case "T64":
            NSLog("Type is T64")
            // let archive = T64ArchiveProxy.makeArchive(fromBuffer: ptr, length: size)
            return
            
        default:
            break
        }
        
        throw NSError(domain: NSOSStatusErrorDomain, code: unimpErr, userInfo: nil)
    }

    override open func revert(toContentsOf url: URL, ofType typeName: String) throws {
       
        let path = url.path
        NSLog("MyDocument::\(#function):\(path)")
        
        if typeName != "VC64" {
            NSLog("Document type is \(typeName), expected VC64")
            throw NSError(domain: NSOSStatusErrorDomain, code: unimpErr, userInfo: nil)
        }
        
        guard let snapshot = SnapshotProxy.makeSnapshot(withFile: path) else {
            NSLog("Error while trying to revert to older snapshopt")
            throw NSError(domain: NSOSStatusErrorDomain, code: unimpErr, userInfo: nil)
        }
        
        c64.load(fromSnapshot: snapshot)
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

    //
    // Saving
    //

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

*/
    
    open override func removeWindowController(_ windowController: NSWindowController) {
        
        NSLog("MyDocument:\(#function)")

        super.removeWindowController(windowController)
        
        // Shut down the emulator.
        // Note that all GUI elements need to be inactive when we set the proxy to nil.
        // Hence, the emulator should be shut down as late as possible.
        c64.kill()
    }
    
}


// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class ImportDialog: DialogController {
        
    enum MediaType {
        case archive
        case disk
        case tape
        case cartridge
        case directory
    }
    
    @IBOutlet weak var icon: NSImageView!
    @IBOutlet weak var checkbox: NSButton!
    @IBOutlet weak var title: NSTextField!
    @IBOutlet weak var subtitle1: NSTextField!
    @IBOutlet weak var subtitle2: NSTextField!
    @IBOutlet weak var subtitle3: NSTextField!
    @IBOutlet weak var drive8: NSButton!
    @IBOutlet weak var drive9: NSButton!
    @IBOutlet weak var flash: NSPopUpButton!
    @IBOutlet weak var flashLabel: NSTextField!
    @IBOutlet weak var carousel: iCarousel!

    var type: FileType!
    var media: MediaType!
    
    var writeProtect: Bool { return checkbox.state == .on }
    var autoRun: Bool { return checkbox.state == .on }
    
    var screenshots: [Screenshot] = []
    
    // Custom font
    let cbmfont = NSFont.init(name: "C64ProMono", size: 10)
    let cbmfontsmall = NSFont.init(name: "C64ProMono", size: 8)
    var monofont: NSFont {
        if #available(OSX 10.15, *) {
            return NSFont.monospacedSystemFont(ofSize: 13.0, weight: .semibold)
        } else {
            return NSFont.monospacedDigitSystemFont(ofSize: 13.0, weight: .semibold)
        }
    }
    
    var titleString = "???"
    var subtitle1String = "???"
    var subtitle2String = ""
    var subtitle3String = ""

    // Shortcuts
    var myDocument: MyDocument { return parent.mydocument! }
    var numItems: Int { return carousel.numberOfItems }
    var currentItem: Int { return carousel.currentItemIndex }
    var centerItem: Int { return numItems / 2 }
    var lastItem: Int { return numItems - 1 }
    var empty: Bool { return numItems == 0 }
    
    override func showSheet(completionHandler handler:(() -> Void)? = nil) {
    
        type = myDocument.attachment!.type()

        switch myDocument.attachment {
             
        case _ as CRTFileProxy:
            
            media = .cartridge
            let crt = myDocument.attachment as! CRTFileProxy
            let type = crt.cartridgeType().rawValue
            let typeName = crt.cartridgeTypeName()!
            
            track("CRTFileProxy")
            titleString = "CRT File"
            subtitle1String = "A cartridge for the C64 expansion port"
            subtitle2String = "CRT type \(type): \(typeName)"
            
        case _ as TAPFileProxy:
            
            media = .tape
            let tap = myDocument.attachment as! TAPFileProxy
            let version = tap.tapVersion()
            let versionName = version == 0 ? "Original" : "Extended"
            track("TAPFileProxy")
            titleString = "TAP File"
            subtitle1String = "A representation of a C64 cassette"
            subtitle2String = "TAP type \(version): \(versionName) layout"
            
        case _ as T64FileProxy:
            
            media = .archive
            track("T64FileProxy")
            titleString = "T64 File"
            subtitle1String = "A collection of multiple C64 programs"

            /* REMOVE ASAP
            var err: FSError = .OK
            let d64 = D64FileProxy.make(withAnyArchive: myDocument.attachment as? AnyArchiveProxy)
            let vol = FSDeviceProxy.make(withArchive: myDocument.attachment as? AnyArchiveProxy)
            let d64_2 = D64FileProxy.make(withVolume: vol, error: &err)
            d64?.write(toFile: "/tmp/d64_1.d64")
            d64_2?.write(toFile: "/tmp/d64_2.d64")
            */
        
        case _ as P00FileProxy:
            
            media = .archive
            track("P00FileProxy")
            titleString = "P00 File"
            subtitle1String = "Binary representation of a single C64 program"
            
        case _ as PRGFileProxy:
            
            media = .archive
            track("PRGFileProxy")
            titleString = "PRG File"
            subtitle1String = "Binary representation of a single C64 program"
            
        case _ as PRGFolderProxy:
            media = .directory
            track("PRGFolderProxy")
            titleString = "Disk from a file system folder"
            subtitle1String = "Comprises all PRG files found in this directory"

        case _ as D64FileProxy:
            
            media = .archive
            track("D64FileProxy")
            titleString = "D64 File"
            subtitle1String = "A byte-accurate image of a C64 diskette"
            
            // REMOVE ASAP
            // let device = FSDeviceProxy.make(withD64: myDocument.attachment as? D64FileProxy)
            // let device = FSDeviceProxy.make(withArchive: myDocument.attachment as? AnyArchiveProxy)
            
        case _ as G64FileProxy:
            
            media = .disk
            track("G64FileProxy")
            titleString = "G64 File"
            subtitle1String = "A bit-accurate image of a C64 diskette"
            
        default:
            fatalError()
        }
        
        // Load screenshots (if any)
        let fnv = myDocument.attachment!.fnv()
        if fnv != 0 {
            for url in Screenshot.collectFiles(forDisk: fnv) {
                if let screenshot = Screenshot.init(fromUrl: url) {
                    screenshots.append(screenshot)
                }
            }
        }
        
        super.showSheet(completionHandler: handler)
    }
    
    override public func awakeFromNib() {
        
        super.awakeFromNib()
        window?.makeFirstResponder(carousel)
        refresh()
        updateCarousel(goto: centerItem, animated: false)
    }
    
    override func windowDidLoad() {
                
        let connected8 = parent.config.drive8Connected
        let connected9 = parent.config.drive9Connected

        title.stringValue = titleString
        subtitle1.stringValue = subtitle1String
        subtitle2.stringValue = subtitle2String
        subtitle3.stringValue = subtitle3String

        let numberOfItems = setUpFlashItems()
        flash.isHidden = numberOfItems == 0
        flashLabel.isHidden = numberOfItems == 0
        
        // Configure controls
        switch media {
                        
        case .archive, .disk, .directory:
            checkbox.title = "Write protect"
            drive8.isEnabled = connected8
            drive9.isEnabled = connected9

        case .tape:
            checkbox.title = "Auto load"
            drive8.title = "Insert"
            drive9.isHidden = true

        case .cartridge:
            checkbox.isHidden = true
            drive8.title = "Attach"
            drive9.isHidden = true

        default:
            fatalError()
        }

        if empty {

            setHeight(212)
            
        } else {
            
            DispatchQueue.main.asyncAfter(deadline: .now() + 0.5) {
                self.carousel.type = iCarouselType.coverFlow
                self.carousel.isHidden = false
                self.carousel.scrollToItem(at: self.centerItem, animated: false)
            }
        }
        
        track("(3)")
    }
    
    func setUpFlashItems() -> Int {
                
        if media != .archive && media != .directory { return 0 }
        let d64 = myDocument.attachment as! AnyArchiveProxy
        
        flash.removeAllItems()
        flash.font = monofont
        
        let items = d64.numberOfItems()
        var seen: [String] = []
        var item = 0
        for i in  0 ..< items {
                        
            d64.selectItem(i)
            let name = d64.nameOfItem()!
            let size = d64.sizeOfItemInBlocks()
            let type = d64.typeOfItem()!
                        
            var title = "\(size)"
            title = title.padding(toLength: 5, withPad: " ", startingAt: 0)
            title += "\"\(name)\""
            title = title.padding(toLength: 24, withPad: " ", startingAt: 0)
            title += "\(type)"
            title = title.padding(toLength: 28, withPad: " ", startingAt: 0)

            if !seen.contains(title) {
                seen.append(title)
                flash.addItem(withTitle: title)
                flash.item(at: item)!.tag = i
                flash.item(at: item)!.isEnabled = type == "PRG"
                item += 1
            }
        }
        flash.autoenablesItems = false
        return flash.numberOfItems
    }
    
    func setHeight(_ newHeight: CGFloat) {
        
        var rect = window!.frame
        rect.origin.y += rect.size.height - newHeight
        rect.size.height = newHeight
        
        window?.setFrame(rect, display: true)
        refresh()
    }

    func refresh() {
        
        switch media {
        case .archive, .disk:
            icon.image = NSImage.init(named: writeProtect ? "disk_protected" : "disk")
        case .directory:
            icon.image = NSImage.init(named: "NSFolder")
        case .cartridge:
            icon.image = NSImage.init(named: "cartridge")
        case .tape:
            icon.image = NSImage.init(named: "tape")
        case .none:
            fatalError()
        }
    }
    
    func updateCarousel(goto item: Int = -1, animated: Bool = false) {
        
        carousel.reloadData()
        if item != -1 {
            self.carousel.scrollToItem(at: item, animated: animated)
        }
        carousel.layOutItemViews()
    }

    //
    // Action methods
    //
    
    @IBAction func checkboxAction(_ sender: NSButton!) {
        
        refresh()
    }

    @IBAction func insertAction(_ sender: NSButton!) {
        
        track("insertAction: \(sender.tag)")

        switch media {
        case .archive, .disk, .directory:
            
            let disk = myDocument.attachment
            let id = sender.tag == 0 ? DriveID.DRIVE8 : DriveID.DRIVE9
            c64.drive(id)?.insertDisk(disk as? AnyArchiveProxy)
            c64.drive(id).setWriteProtection(writeProtect)
            
        case .cartridge:

            let cartridge = myDocument.attachment as! CRTFileProxy
            c64.expansionport.attachCartridgeAndReset(cartridge)
                        
        case .tape:
            
            let tape = myDocument.attachment as! TAPFileProxy
            c64.datasette.insertTape(tape)
            
            if autoRun {
                parent.keyboard.type(string: "LOAD\n",
                                     completion: c64.datasette.pressPlay)
            }
            
        case .none:
            fatalError()
        }
        
        parent.renderer.rotateLeft()
        hideSheet()
    }

    @IBAction func flashAction(_ sender: NSButton!) {
        
        track("flashAction: \(sender.selectedTag())")
        
        let archive = myDocument.attachment as! AnyArchiveProxy
        parent.c64.flash(archive, item: sender.selectedTag())

        parent.keyboard.type("RUN\n")
        parent.renderer.rotateLeft()
        hideSheet()
    }

    @IBAction override func cancelAction(_ sender: Any!) {
                                  
         hideSheet()
     }
}

extension ImportDialog: NSWindowDelegate {
    
    func windowDidResize(_ notification: Notification) {
        
        track()
    }
    
    func windowWillStartLiveResize(_ notification: Notification) {
         
         track()
     }
     
     func windowDidEndLiveResize(_ notification: Notification) {
        
        track()
     }
}

//
// iCarousel data source and delegate
//

extension ImportDialog: iCarouselDataSource, iCarouselDelegate {
    
    func numberOfItems(in carousel: iCarousel) -> Int {
        
        return screenshots.count
    }
    
    func carousel(_ carousel: iCarousel, viewForItemAt index: Int, reusing view: NSView?) -> NSView {
        
        let h = carousel.frame.height - 10
        let w = h * 4 / 3
        let itemView = NSImageView(frame: CGRect(x: 0, y: 0, width: w, height: h))
        
        itemView.image = screenshots[index].screen?.roundCorners()
        return itemView
    }
}

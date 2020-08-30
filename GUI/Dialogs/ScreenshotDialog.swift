// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class ScreenshotDialog: DialogController {
    
    var now: Date!

    @IBOutlet weak var carousel: iCarousel!
    @IBOutlet weak var leftButton: NSButton!
    @IBOutlet weak var rightButton: NSButton!
    @IBOutlet weak var actionButton: NSButton!
    @IBOutlet weak var finderLabel: NSTextField!
    @IBOutlet weak var finderButton: NSButton!
    @IBOutlet weak var itemLabel: NSTextField!
    @IBOutlet weak var text1: NSTextField!
    @IBOutlet weak var text2: NSTextField!

    // Fingerprint of linked media file
    var checksum = UInt64(0)
    
    var latestIndex = 0
    var favoriteIndex = 0
    
    // Computed variables
    var myDocument: MyDocument { return parent.mydocument! }
    var numItems: Int { return carousel.numberOfItems }
    var currentItem: Int { return carousel.currentItemIndex }
    var centerItem: Int { return numItems / 2 }
    var lastItem: Int { return numItems - 1 }
    var empty: Bool { return numItems == 0 }

    override func windowWillLoad() {
   
        track()
    }
    
    override func sheetDidShow() {
  
        track()
        
        now = Date()
        
        updateLabels()
        
        self.carousel.type = iCarouselType.coverFlow
        self.carousel.isHidden = false
        self.updateCarousel(goto: self.centerItem, animated: false)
    }
    
    func updateLabels() {
        
        track("numItems = \(numItems)")
        
        carousel.isHidden = false
        itemLabel.isHidden = empty
        text1.isHidden = false
        text2.isHidden = false

        leftButton.isEnabled = currentItem > 0
        rightButton.isEnabled = currentItem >= 0 && currentItem < lastItem
        itemLabel.stringValue = "\(currentItem + 1) / \(numItems)"

        actionButton.image = NSImage.init(named: "trashTemplate")
        actionButton.toolTip = "Delete screenshot from disk"
        actionButton.isHidden = empty
        leftButton.isHidden = empty
        rightButton.isHidden = empty
        finderLabel.isHidden = empty
        finderButton.isHidden = empty
        
        if let screenshot = myDocument.screenshots.element(at: currentItem) {
            text1.stringValue = screenshot.description
            text2.stringValue = screenshot.sizeString + " pixels"
        } else {
            text1.stringValue = "No screenshots available"
            text2.stringValue = ""
        }
    }
   
    func updateCarousel(goto item: Int, animated: Bool) {
        
        carousel.reloadData()
        
        let index = min(item, lastItem)
        if index >= 0 { carousel.scrollToItem(at: index, animated: animated) }
        
        carousel.layOutItemViews()
        updateLabels()
    }

    func updateCarousel(animated: Bool = false) {
        
        updateCarousel(goto: -1, animated: animated)
    }

    @IBAction func selectorAction(_ sender: NSSegmentedControl!) {
                
        updateCarousel(goto: Int.max, animated: false)
    }
    
    @IBAction func leftAction(_ sender: NSButton!) {
        
        if currentItem > 0 {
            myDocument.screenshots.swapAt(currentItem, currentItem - 1)
            updateCarousel(goto: currentItem - 1, animated: true)
        }
    }

    @IBAction func rightAction(_ sender: NSButton!) {
        
        if currentItem < lastItem {
            myDocument.screenshots.swapAt(currentItem, currentItem + 1)
            updateCarousel(goto: currentItem + 1, animated: true)
        }
    }

    @IBAction func actionAction(_ sender: NSButton!) {
                    
        myDocument.screenshots.remove(at: currentItem)
        updateCarousel(goto: currentItem - 1, animated: true)
    }

    @IBAction func finderAction(_ sender: NSButton!) {
        
        if let url = Screenshot.folder(forDisk: checksum) {
            
            try? myDocument.persistScreenshots()
            NSWorkspace.shared.open(url)
        }
    }
    
    @IBAction override func cancelAction(_ sender: Any!) {
        
        track()
                        
        hideSheet()
                
        carousel.isHidden = true
        leftButton.isHidden = true
        rightButton.isHidden = true
        actionButton.isHidden = true
        text1.stringValue = ""
        text2.stringValue = ""
        
        try? myDocument.persistScreenshots()
    }
}

//
// iCarousel data source and delegate
//

extension ScreenshotDialog: iCarouselDataSource, iCarouselDelegate {
    
    func numberOfItems(in carousel: iCarousel) -> Int {
                
        return myDocument.screenshots.count
    }
    
    func carousel(_ carousel: iCarousel, viewForItemAt index: Int, reusing view: NSView?) -> NSView {
        
        let h = carousel.frame.height - 10
        let w = h * 4 / 3
        let itemView = NSImageView(frame: CGRect(x: 0, y: 0, width: w, height: h))
        
        itemView.image =
            myDocument.screenshots.element(at: index)?.screen?.roundCorners()
        
        return itemView
    }
    
    func carouselCurrentItemIndexDidChange(_ carousel: iCarousel) {
        
        track()
        updateLabels()
    }
}

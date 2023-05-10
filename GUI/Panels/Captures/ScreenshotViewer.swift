// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class ScreenshotViewer: DialogController {
    
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

    // Screenshot storage
    var screenshots: [Screenshot] = []
    
    // Indicates if the screenshot storage has been edited
    var needsSaving = false
    
    // Computed variables
    var myDocument: MyDocument { return parent.mydocument! }
    var currentItem: Int { return carousel.currentItemIndex }
    var centerItem: Int { return screenshots.count / 2 }
    var lastItem: Int { return screenshots.count - 1 }
    var empty: Bool { return screenshots.count == 0 }

    func loadScreenshots() {

        debug(.media)

        for url in Screenshot.allFiles {
            if let screenshot = Screenshot(fromUrl: url) {
                screenshots.append(screenshot)
            }
        }

        debug(.media, "\(screenshots.count) screenshots loaded")
    }
    
    func saveScreenshots() throws {
        
        debug(.media)

        Screenshot.deleteFolder()
        for n in 0 ..< screenshots.count {
            try? screenshots[n].save()
        }

        debug(.media, "All screenshots saved")
    }
    
    override func sheetWillShow() {
        
        loadScreenshots()
    }
    
    override func sheetDidShow() {

        now = Date()
        
        updateLabels()
        
        self.carousel.type = iCarouselType.coverFlow
        self.carousel.isHidden = false
        self.updateCarousel(goto: self.centerItem, animated: false)
    }
    
    func updateLabels() {

        carousel.isHidden = false
        itemLabel.isHidden = empty
        text1.isHidden = false
        text2.isHidden = false

        leftButton.isEnabled = currentItem > 0
        rightButton.isEnabled = currentItem >= 0 && currentItem < lastItem
        itemLabel.stringValue = "\(currentItem + 1) / \(screenshots.count)"

        actionButton.image = NSImage(named: "trashTemplate")
        actionButton.toolTip = "Delete screenshot"
        actionButton.isHidden = empty
        leftButton.isHidden = empty
        rightButton.isHidden = empty
        finderLabel.isHidden = empty
        finderButton.isHidden = empty

        var label1 = "No screenshots available"
        var label2 = ""
        
        if currentItem >= 0 && currentItem < screenshots.count {
            let screenshot = screenshots[currentItem]
            label1 = screenshot.description
            label2 = screenshot.sizeString + " pixels"
        }
        
        text1.stringValue = label1
        text2.stringValue = label2
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
            screenshots.swapAt(currentItem, currentItem - 1)
            updateCarousel(goto: currentItem - 1, animated: true)
            needsSaving = true
        }
    }

    @IBAction func rightAction(_ sender: NSButton!) {
        
        if currentItem < lastItem {
            screenshots.swapAt(currentItem, currentItem + 1)
            updateCarousel(goto: currentItem + 1, animated: true)
            needsSaving = true
        }
    }

    @IBAction func deleteAction(_ sender: NSButton!) {
        
        screenshots.remove(at: currentItem)
        updateCarousel(goto: currentItem - 1, animated: true)
        needsSaving = true
    }

    @IBAction func finderAction(_ sender: NSButton!) {
        
        if let url = Screenshot.folder {

            NSWorkspace.shared.open(url)
        }
    }
    
    @IBAction override func cancelAction(_ sender: Any!) {
                                
        hideSheet()
                
        carousel.isHidden = true
        leftButton.isHidden = true
        rightButton.isHidden = true
        actionButton.isHidden = true
        text1.stringValue = ""
        text2.stringValue = ""
        
        if needsSaving { try? saveScreenshots() }
        screenshots = []
    }
}

//
// iCarousel data source and delegate
//

extension ScreenshotViewer: iCarouselDataSource, iCarouselDelegate {
    
    func numberOfItems(in carousel: iCarousel) -> Int {
                
        return screenshots.count
    }
    
    func carousel(_ carousel: iCarousel, viewForItemAt index: Int, reusing view: NSView?) -> NSView {
        
        let h = carousel.frame.height - 10
        let w = h * 4 / 3
        let itemView = NSImageView(frame: CGRect(x: 0, y: 0, width: w, height: h))
        
        if index < screenshots.count {
            itemView.image = screenshots[index].screen?.roundCorners()
        }
        return itemView
    }
    
    func carouselCurrentItemIndexDidChange(_ carousel: iCarousel) {
        
        updateLabels()
    }
}

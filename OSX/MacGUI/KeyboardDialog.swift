//
//  KeyboardDialogController.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 19.02.18.
//

import Foundation

func textToImage(text: NSString, inImage: NSImage) -> NSImage {
    
    let width = 48.0
    let height = 48.0
    let font = NSFont.boldSystemFont(ofSize: 12)
    let imageRect = CGRect(x: 0, y: 0, width: width, height: height)
    let textRect = CGRect(x: 5, y: 5, width: width - 5, height: height - 5)
    let textStyle = NSMutableParagraphStyle.default.mutableCopy() as! NSMutableParagraphStyle
    let textFontAttributes = [
        NSAttributedStringKey.font: font,
        NSAttributedStringKey.foregroundColor: NSColor.gray,
        NSAttributedStringKey.paragraphStyle: textStyle
    ]
    
    let outImage = NSImage(size: NSSize.init(width: width, height: height))
    let rep:NSBitmapImageRep = NSBitmapImageRep(bitmapDataPlanes: nil,
                                                pixelsWide: Int(width),
                                                pixelsHigh: Int(height),
                                                bitsPerSample: 8,
                                                samplesPerPixel: 4,
                                                hasAlpha: true,
                                                isPlanar: false,
                                                colorSpaceName: NSColorSpaceName.calibratedRGB,
                                                bytesPerRow: 0,
                                                bitsPerPixel: 0)!
    outImage.addRepresentation(rep)
    outImage.lockFocus()
    inImage.draw(in: imageRect)
    text.draw(in: textRect, withAttributes: textFontAttributes)
    outImage.unlockFocus()
    
    track()
    return outImage
}

class KeyboardDialog : UserDialogController {

    // Double array of key images, indexed by their row and column number
    // var keyImage: [[NSImage]] = []
    var keyImage = Array(repeating: Array(repeating: nil as NSImage?, count: 8), count: 8)
    
    // Custom font
    let cbmfont = NSFont.init(name: "C64ProMono", size: 10)
    let cbmfontsmall = NSFont.init(name: "C64ProMono", size: 8)
    
    // Outlets
    @IBOutlet weak var keyCode: NSTextField!
    @IBOutlet weak var keyChar: NSTextField!
    @IBOutlet weak var keyMatrix: NSCollectionView!
    
    /*
    override func showSheet(withParent controller: MyController,
                            completionHandler:(() -> Void)? = nil) {
        
        track()
        let document = controller.document as! MyDocument
        archive = document.attachment as! ArchiveProxy
        super.showSheet(withParent: controller, completionHandler: completionHandler)
    }
    */
    
    override public func awakeFromNib() {
        
        track()
        
        // Create all images
        let background = NSImage(named: NSImage.Name(rawValue: "key.png"))!
        
        for row in 0...7 {
            for col in 0...7 {
                keyImage[row][col] = textToImage(text: "\(row),\(col)" as NSString,
                                                 inImage: background)
            }
        }
        
        if (keyImage[0][0] == keyImage[0][1]) {
            track("WE HAVE A PROBLEM")
        }
    }
    
    func updateImages(with keymap: [MacKey:C64Key]) {
        
    }
    //
    // Action methods
    //
    
    @IBAction func okAction(_ sender: Any!) {
        
        track()
        hideSheet()
    }
    
    @IBAction func performDoubleClick(_ sender: Any!) {
        
        track()
        hideSheet()
    }
}

//
// NSCollectionView delegate and data source
//

extension KeyboardDialog : NSCollectionViewDataSource {
    
    func numberOfSections(in collectionView: NSCollectionView) -> Int {
        
        return 8
    }
    
    func collectionView(_ collectionView: NSCollectionView, numberOfItemsInSection section: Int) -> Int {
        
        return 8
    }
    
    func collectionView(_ itemForRepresentedObjectAtcollectionView: NSCollectionView, itemForRepresentedObjectAt indexPath: IndexPath) -> NSCollectionViewItem {
        
        let id = NSUserInterfaceItemIdentifier(rawValue: "KeyViewItem")
        let item = keyMatrix.makeItem(withIdentifier: id, for: indexPath)
        guard let keyViewItem = item as? KeyViewItem else {
            track("ERROR")
            return item
        }
        
        let row = indexPath.section
        let col = indexPath.item
        
        track("row = \(row) col = \(col)")
        keyViewItem.imageView?.image = keyImage[row][col]
        return keyViewItem
    }

}



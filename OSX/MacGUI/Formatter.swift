//
//  Formatter.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 21.05.18.
//

import Foundation

class MyFormatter : Formatter {
    
    var hex : Bool = true
    var inFormat: String = ""
    var outFormat: String = ""
    
    init(inFormat: String, outFormat: String, hex: Bool) {
        
        super.init()
        self.hex = hex
        self.inFormat = inFormat
        self.outFormat = outFormat
    }
    
    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    override func isPartialStringValid(_ partialString: String, newEditingString newString: AutoreleasingUnsafeMutablePointer<NSString?>?, errorDescription error: AutoreleasingUnsafeMutablePointer<NSString?>?) -> Bool {
        
        let range = partialString.range(of: inFormat, options: .regularExpression)
        if range != partialString.startIndex..<partialString.endIndex {
            // newString = nil
            NSSound.beep()
            return false
        }
        return true
    }
    
    override func getObjectValue(_ obj: AutoreleasingUnsafeMutablePointer<AnyObject?>?, for string: String, errorDescription error: AutoreleasingUnsafeMutablePointer<NSString?>?) -> Bool {
        
        let scanner = Scanner.init(string: string)
        
        if hex {
            var result : UInt32 = 0
            if scanner.scanHexInt32(&result) {
                obj?.pointee = result as AnyObject?
                return true
            }
        } else {
            var result : Int32 = 0
            if scanner.scanInt32(&result) {
                obj?.pointee = result as AnyObject?
                return true
            }
        }
        obj?.pointee = 0 as AnyObject?
        return false
    }
        
    override func string(for obj: Any?) -> String? {
    
        if let number = obj as? Int {
            return String.init(format: outFormat, number)
        } else {
            return nil
        }
    }
}

//
//  Formatter.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 21.05.18.
//

import Foundation

class MyFormatter : Formatter {
    
    var radix : Int
    var minValue: Int
    var maxValue : Int
    var format: String
    
    init(radix: Int, min: Int, max: Int) {

        self.radix = radix
        self.minValue = min
        self.maxValue = max
        
        if radix == 10 {

            format =
                (maxValue < 10) ? "%01d" :
                (maxValue < 100) ? "%02d" :
                (maxValue < 1000) ? "%03d" :
                (maxValue < 10000) ? "%04d" :
                (maxValue < 100000) ? "%05d" : "???"
    
        } else {
            
            format =
                (maxValue < 0x10) ? "%01X" :
                (maxValue < 0x100) ? "%02X" :
                (maxValue < 0x1000) ? "%03X" :
                (maxValue < 0x10000) ? "%04X" :
                (maxValue < 0x100000) ? "%05X" : "???"
        }
  
        super.init()
    }
    
    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    override func isPartialStringValid(_ partialString: String, newEditingString newString: AutoreleasingUnsafeMutablePointer<NSString?>?, errorDescription error: AutoreleasingUnsafeMutablePointer<NSString?>?) -> Bool {
        
        /*
        let range = partialString.range(of: inFormat, options: .regularExpression)
        if range != partialString.startIndex..<partialString.endIndex {
            // newString = nil
            NSSound.beep()
            return false
        }
        */
        let number = (partialString == "") ? 0 : Int(partialString, radix: radix)
        if (number == nil || number! < minValue || number! > maxValue) {
            NSSound.beep()
            return false
        }
        return true
    }
    
    override func getObjectValue(_ obj: AutoreleasingUnsafeMutablePointer<AnyObject?>?, for string: String, errorDescription error: AutoreleasingUnsafeMutablePointer<NSString?>?) -> Bool {
        
        let result = (string == "") ? 0 : Int(string, radix: radix)
        obj?.pointee = result as AnyObject?
        return true
    }
        
    override func string(for obj: Any?) -> String? {
    
        guard let number = obj as? Int else {
            return nil
        }
        
        switch radix {
        case 2:
            let bits: [Character] = [number & 0x80 != 0 ? "1" : "0",
                                     number & 0x40 != 0 ? "1" : "0",
                                     number & 0x20 != 0 ? "1" : "0",
                                     number & 0x10 != 0 ? "1" : "0",
                                     number & 0x08 != 0 ? "1" : "0",
                                     number & 0x04 != 0 ? "1" : "0",
                                     number & 0x02 != 0 ? "1" : "0",
                                     number & 0x01 != 0 ? "1" : "0"]
            return String.init(bits)
            
        case 10, 16:
            return String.init(format: format, number)

        default:
            assert(false)
            return "?"
        }
    }
}

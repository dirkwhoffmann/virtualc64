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
        
        switch maxValue {
        case 7:
            format = (radix == 10) ? "%01d" : (radix == 16) ? "%01X" : ""
            break
        case 15:
            format = (radix == 10) ? "%02d" : (radix == 16) ? "%01X" : ""
            break
        case 255:
            format = (radix == 10) ? "%03d" : (radix == 16) ? "%02X" : ""
            break
        case 511:
            format = (radix == 10) ? "%03d" : (radix == 16) ? "%03X" : ""
            break
        case 4095:
            format = (radix == 10) ? "%04d" : (radix == 16) ? "%03X" : ""
            break
        case 65535:
            format = (radix == 10) ? "%04d" : (radix == 16) ? "%03X" : ""
            break
        default:
            format = ""
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

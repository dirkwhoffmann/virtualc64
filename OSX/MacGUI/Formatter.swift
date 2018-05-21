//
//  Formatter.swift
//  VirtualC64
//
//  Created by Dirk Hoffmann on 21.05.18.
//

import Foundation

class MyFormatter : Formatter {
    
    var radix : Int
    var inFormat: String
    var outFormat: String
    
    init(inFormat: String, outFormat: String, radix: Int) {

        self.radix = radix
        self.inFormat = inFormat
        self.outFormat = outFormat
        super.init()
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
        
        let result = Int(string, radix: radix)
        obj?.pointee = result as AnyObject?
        return true
        
        /*
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
        */
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
            return String.init(format: outFormat, number)
        
        default:
            assert(false)
            return "?"
        }
    }
}

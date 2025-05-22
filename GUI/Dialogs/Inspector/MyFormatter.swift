// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class MyFormatter: Formatter {
    
    var radix = 0 { didSet { updateFormatString() } }
    var padding = false { didSet { updateFormatString() } }
    var minValue = 0
    var maxValue = 0
    var format = ""
    
    func toBinary(value: Int, digits: Int) -> String {
        
        var result = ""
        
        for i in (0 ..< digits).reversed() {
        
            result += (value & (1 << i)) != 0 ? "1" : "0"
        }
        
        return result
    }
    
    init(radix: Int, min: Int, max: Int) {

        super.init()

        self.radix = radix
        self.minValue = min
        self.maxValue = max
        
        updateFormatString()
    }
    
    required init?(coder aDecoder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    func updateFormatString() {
    
        if radix == 10 && padding == false {
            
            format =
            (maxValue <= 9) ? "%1u" :
            (maxValue <= 99) ? "%2u" :
            (maxValue <= 999) ? "%3u" :
            (maxValue <= 9999) ? "%4u" :
            (maxValue <= 99999) ? "%5u" :
            (maxValue <= 999999) ? "%6u" :
            (maxValue <= 9999999) ? "%7u" :
            (maxValue <= 99999999) ? "%8u" :
            (maxValue <= 999999999) ? "%9u" :
            (maxValue <= 9999999999) ? "%10u" : "???"

        } else if radix == 10 && padding == true {
            
            format =
            (maxValue <= 9) ? "%01u" :
            (maxValue <= 99) ? "%02u" :
            (maxValue <= 999) ? "%03u" :
            (maxValue <= 9999) ? "%04u" :
            (maxValue <= 99999) ? "%05u" :
            (maxValue <= 999999) ? "%06u" :
            (maxValue <= 9999999) ? "%07u" :
            (maxValue <= 99999999) ? "%08u" :
            (maxValue <= 999999999) ? "%09u" :
            (maxValue <= 9999999999) ? "%010u" : "???"

        } else if radix == 16 && padding == false {
            
            format =
            (maxValue <= 0xF) ? "%1X" :
            (maxValue <= 0xFF) ? "%2X" :
            (maxValue <= 0xFFF) ? "%3X" :
            (maxValue <= 0xFFFF) ? "%4X" :
            (maxValue <= 0xFFFFF) ? "%5X" :
            (maxValue <= 0xFFFFFF) ? "%6X" :
            (maxValue <= 0xFFFFFFF) ? "%7X" :
            (maxValue <= 0xFFFFFFFF) ? "%8X" :
            (maxValue <= 0xFFFFFFFFF) ? "%9X" : "???"

        } else if radix == 16 && padding == true {
            
            format =
            (maxValue <= 0xF) ? "%01X" :
            (maxValue <= 0xFF) ? "%02X" :
            (maxValue <= 0xFFF) ? "%03X" :
            (maxValue <= 0xFFFF) ? "%04X" :
            (maxValue <= 0xFFFFF) ? "%05X" :
            (maxValue <= 0xFFFFFF) ? "%06X" :
            (maxValue <= 0xFFFFFFF) ? "%07X" :
            (maxValue <= 0xFFFFFFFF) ? "%08X" :
            (maxValue <= 0xFFFFFFFFF) ? "%09X" : "???"

        } else {
            
            format = "Invalid"
        }
    }
    
    override func isPartialStringValid(_ partialString: String, newEditingString newString: AutoreleasingUnsafeMutablePointer<NSString?>?, errorDescription error: AutoreleasingUnsafeMutablePointer<NSString?>?) -> Bool {
        
        let number = (partialString == "") ? 0 : Int(partialString, radix: radix)
        if number == nil || number! < minValue || number! > maxValue {
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
        case 2 where maxValue == 0xFF:
            return toBinary(value: number, digits: 8)
                        
        case 2 where maxValue == 0xFFFF:
            return toBinary(value: number, digits: 16)
            
        case 10, 16:
            return String(format: format, number)

        default:
            fatalError()
        }
    }
}

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
            (maxValue < 0x10) ? "%2u" :
            (maxValue < 0x100) ? "%3u" :
            (maxValue < 0x1000) ? "%4u" :
            (maxValue < 0x10000) ? "%5u" :
            (maxValue < 0x100000) ? "%7u" :
            (maxValue < 0x1000000) ? "%8u" :
            (maxValue < 0x10000000) ? "%9u" :
            (maxValue < 0x100000000) ? "%10u" : "???"
            
        } else if radix == 10 && padding == true {
            
            format =
            (maxValue < 0x10) ? "%02u" :
            (maxValue < 0x100) ? "%03u" :
            (maxValue < 0x1000) ? "%04u" :
            (maxValue < 0x10000) ? "%05u" :
            (maxValue < 0x100000) ? "%07u" :
            (maxValue < 0x1000000) ? "%08u" :
            (maxValue < 0x10000000) ? "%09u" :
            (maxValue < 0x100000000) ? "%010u" : "???"

        } else if radix == 16 && padding == false {
            
            format =
            (maxValue < 0x10) ? "%1X" :
            (maxValue < 0x100) ? "%2X" :
            (maxValue < 0x1000) ? "%3X" :
            (maxValue < 0x10000) ? "%4X" :
            (maxValue < 0x100000) ? "%5X" :
            (maxValue < 0x1000000) ? "%6X" :
            (maxValue < 0x10000000) ? "%7X" :
            (maxValue < 0x100000000) ? "%8X" : "???"

        } else if radix == 16 && padding == true {
            
            format =
            (maxValue < 0x10) ? "%01X" :
            (maxValue < 0x100) ? "%02X" :
            (maxValue < 0x1000) ? "%03X" :
            (maxValue < 0x10000) ? "%04X" :
            (maxValue < 0x100000) ? "%05X" :
            (maxValue < 0x1000000) ? "%06X" :
            (maxValue < 0x10000000) ? "%07X" :
            (maxValue < 0x100000000) ? "%08X" : "???"
            
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

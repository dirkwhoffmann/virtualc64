// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

final class RangeField: NSTextField {

    var minValue: Double?
    var maxValue: Double?

    func setRange<T: BinaryInteger>(_ range: ClosedRange<T>) {
        setFormatter(min: Double(range.lowerBound), max: Double(range.upperBound), allowsFloats: false)
    }

    func setRange<T: BinaryFloatingPoint>(_ range: ClosedRange<T>) {
        setFormatter(min: Double(range.lowerBound), max: Double(range.upperBound), allowsFloats: true)
    }

    func setRange<T: BinaryFloatingPoint>(min: T, max: T) {
        setFormatter(min: Double(min), max: Double(max), allowsFloats: true)
    }

    private func setFormatter(min: Double, max: Double, allowsFloats: Bool) {

        self.delegate = self

        self.minValue = min
        self.maxValue = max

        let formatter = (self.formatter as? NumberFormatter) ?? {
            let nf = NumberFormatter()
            self.formatter = nf
            return nf
        }()

        formatter.allowsFloats = allowsFloats
        // formatter.numberStyle = .none
        formatter.usesGroupingSeparator = false
        formatter.isLenient = true
    }
}

extension RangeField: NSTextFieldDelegate {

    func control(_ control: NSControl, textShouldEndEditing fieldEditor: NSText) -> Bool {

        if let min = minValue, doubleValue < min {

            NSSound.beep();
            doubleValue = min
            return false
        }

        if let max = maxValue, doubleValue > max {

            NSSound.beep();
            doubleValue = max
            return false
        }

        return true
    }
}

    /*
    override func textDidEndEditing(_ notification: Notification) {

        super.textDidEndEditing(notification)

        let number = doubleValue

        // Clamp to range
        if let minValue = minValue, number < minValue {
            self.doubleValue = minValue
        }
        if let maxValue = maxValue, number > maxValue {
            self.doubleValue = maxValue
        }
        print("textDidEndEditing \(self.doubleValue)")
    }
     */

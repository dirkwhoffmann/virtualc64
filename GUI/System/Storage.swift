// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class ManagedArray<Element> {

    // Element storage (each element is associated with a size)
    var elements: [(Element, Int)] = []

    // Maximum number of stored items
    var maxItems: Int

    // Maximum accumulated size
    var maxSize: Int

    init(maxCount: Int = Int.max, maxSize: Int = Int.max) {

        self.maxItems = maxCount
        self.maxSize = maxSize
    }

    func clear() {

        elements = []
    }

    var count: Int { return elements.count }
    var used: Int { return elements.reduce(0) { $0 + $1.1 } }
    var fill: Double { return 100 * Double(used) / Double(maxSize) }

    func element(at index: Int) -> Element? {

        return (index >= 0 && index < elements.count) ? elements[index].0 : nil
    }

    var lastElement: Element? {

        return elements.count > 0 ? elements[elements.count - 1].0 : nil
    }

    func append(_ newElement: Element, size: Int) {

        // Append the elements
        elements.append((newElement, size))

        // Remove older elements until the capacity contraints are met,
        // but do not delete the new element.
        while count > 1 && (count > maxItems || used > maxSize) {

            elements.remove(at: 0)
        }
    }

    func remove(at index: Int) {

        elements.remove(at: index)
    }

    func swapAt(_ i: Int, _ j: Int) {

        elements.swapAt(i, j)
    }
}

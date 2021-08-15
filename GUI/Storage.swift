// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

class ManagedArray<Element> {
    
    var capacity: Int
    var elements: [Element] = []
    var counter = 0
    var modified = false
    
    init(capacity: Int) {
        self.capacity = capacity
    }
    
    func clear() {
        elements = []
        counter = 0
        modified = false
    }
    
    var count: Int { return elements.count }
    
    func element(at index: Int) -> Element? {
        
        return (index >= 0 && index < elements.count) ? elements[index] : nil
    }

    var lastElement: Element? {
        
        return elements.count > 0 ? elements[elements.count - 1] : nil
    }

    func append(_ newElement: Element) {
        
        // Thin out the array if capacity has been reached
        if elements.count >= capacity {
            
            var itemToDelete = 0
            
            /*
            if counter % 2 == 0 {
                itemToDelete = 24
            } else if (counter >> 1) % 2 == 0 {
                itemToDelete = 16
            } else if (counter >> 2) % 2 == 0 {
                itemToDelete = 8
            }
            */
            counter += 1
            
            // WE ALWAYS DELETE THE OLDEST ELEMENT FOR NOW
            itemToDelete = elements.count - 1
            
            elements.remove(at: itemToDelete)
        }
        
        elements.append(newElement)
        modified = true
    }
    
    func remove(at index: Int) {
        
        track("elements = \(elements.count) index = \(index)")
        elements.remove(at: index)
        modified = true
    }

    func swapAt(_ i: Int, _ j: Int) {
        
        elements.swapAt(i, j)
        modified = true
    }
}

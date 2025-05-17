// -----------------------------------------------------------------------------
// This file is part of vAmiga
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

import SwiftUI

struct DataPoint: Identifiable {
    
    var id = UUID()
    var series: Int
    var timestamp: Date
    var value: Double
}

@MainActor
class DashboardDataProvider: ObservableObject {
    
    @Published var update = false
    
    static let maxTimeSpan: Double = 5
    
    // Visualized data
    var data: [DataPoint] = []
    
    // Valid value range
    var range: ClosedRange = 0.0...1.0 { didSet { updateGridLines() } }
    
    // Unit
    var unit = ""
        
    // Grid lines
    var gridLines: [Double] = []
        
    // Scaling
    var logScale = false { didSet { updateGridLines() } }
 
    func updateGridLines() {
        
        if (logScale) {
            
            gridLines = Array(repeating: 0.0, count: 7)
            for i in 0..<gridLines.count {
                var y = Double(i) / (Double(gridLines.count) - 1)
                if logScale { y = log(1.0 + 19.0 * y) / log(20) }
                gridLines[i] = y * range.upperBound
            }
            
        } else {
            gridLines = [0.0, 0.25, 0.5, 0.75, 1.0].map { $0 * range.upperBound }
        }
    }
    
    var timeSpan: TimeInterval {
        
        guard let first = data.first else { return 0.0 }
        guard let last = data.last else { return 0.0 }
        
        return last.timestamp.timeIntervalSince(first.timestamp)
    }
    
    func add(series: Int, value: Double) {
        
        data = []
        data.append(DataPoint(series: 0, timestamp: Date.now, value: value))
                
        // Update the view
        update.toggle()
    }
    
    func add(_ value1: Double?, _ value2: Double? = nil) {
        
        add(value1, value2, timestamp: Date.now)
    }
    
    private func add(_ value1: Double?, _ value2: Double?, timestamp: Date) {
        
        // Clamp
        var v1 = value1 == nil ? 0 : min(value1!, max(value1!, range.lowerBound), range.upperBound)
        var v2 = value2 == nil ? 0 : min(value2!, max(value2!, range.lowerBound), range.upperBound)

        // Scale
        if logScale {
            v1 =  log(1.0 + (19 * v1 / range.upperBound)) / log(20) * range.upperBound
            v2 =  log(1.0 + (19 * v2 / range.upperBound)) / log(20) * range.upperBound
        }
        
        // Record values
        if value2 != nil {
            data.append(DataPoint(series: 2, timestamp: timestamp, value: v2))
        }
        if value1 != nil {
            data.append(DataPoint(series: 1, timestamp: timestamp, value: v1))
        }
                
        // Record the sum (used for drawing the line)
        data.append(DataPoint(series: 3, timestamp: timestamp, value: v1 + v2))
        
        // Delete outdated data
        while timeSpan > DashboardDataProvider.maxTimeSpan { data.removeFirst() }
        
        // Update the view
        update.toggle()
    }
    
    func latest(series: Int = 3) -> Double {
        
        if let index = data.lastIndex(where: { $0.series == series }) {
            return data[index].value
        } else {
            return range.lowerBound
        }
    }
}

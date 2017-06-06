//
//  MyControllerTouchBar.swift
//  V64
//
//  Created by Dirk Hoffmann on 06.06.17.
//
//

import Foundation

#if false
public extension MyController {
    public func PrettyPrint () {
        print("EXTENDING MyController")
    }
}
#endif

public let CARDS = ["card1", "card2"]

@objc class SwiftTest : NSObject {
    // private init() {}
    class func cards() -> [String] { return CARDS }

    func hello_swift(_ name: String) {
        print("Hello \(name) in Swift");
    }

    func test_swift(c64: C64Proxy) {
        c64.dump();
    }
}

//
//  C64ProxyColors.h
//  V64
//
//  Created by Dirk Hoffmann on 17.08.15.
//
//

#import <Cocoa/Cocoa.h>

#import "C64Proxy.h"

//! Predefined color schemes
enum ColorScheme {
    CCS64           = 0x00,
    VICE            = 0x01,
    FRODO           = 0x02,
    PC64            = 0x03,
    C64S            = 0x04,
    ALEC64          = 0x05,
    WIN64           = 0x06,
    C64ALIVE_0_9    = 0x07,
    GODOT           = 0x08,
    C64SALLY        = 0x09,
    PEPTO           = 0x0A,
    GRAYSCALE       = 0x0B
};

@interface C64Proxy(Colors)

- (int) colorScheme;
- (void) setColorScheme:(unsigned)scheme;

@end

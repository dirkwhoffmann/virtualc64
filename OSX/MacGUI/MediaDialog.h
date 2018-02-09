/*
 * (C) 2015 Dirk W. Hoffmann. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#import <Cocoa/Cocoa.h>

@interface MediaDialog : NSWindow
{
    C64Proxy *c64;
    MyController *controller;
    
    /* VC 1541 */
    NSString *archiveName;
    unsigned noOfFiles;
    IBOutlet NSImageView *diskIcon;
    IBOutlet NSTextField *diskText;
    IBOutlet NSButton *diskEject;
    IBOutlet NSTextField *diskEjectText;
    IBOutlet NSButton *diskWriteProtected;
    
    /* VC 1530 */
    IBOutlet NSImageView *tapeIcon;
    IBOutlet NSTextField *tapeText;
    IBOutlet NSButton *tapeEject;
    IBOutlet NSTextField *tapeEjectText;
    IBOutlet NSSlider *tapeSlider;
    IBOutlet NSTextField *tapeHead;
    IBOutlet NSTextField *tapeEnd;

    /* Expansion port */
    IBOutlet NSImageView *cartridgeIcon;
    IBOutlet NSTextField *cartridgeText;
    IBOutlet NSButton *cartridgeEject;
    IBOutlet NSTextField *cartridgeEjectText;
}

- (void) initialize:(MyController *)mycontroller;
- (void) update;

// VC 1541
- (IBAction)diskEjectAction:(id)sender;
- (IBAction)diskWriteProtectAction:(id)sender;

// VC 1530
- (IBAction)tapeHeadAction:(id)sender;

// Expansion port
- (IBAction)cartridgeEjectAction:(id)sender;

@end

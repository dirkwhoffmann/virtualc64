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

/* System */
extern NSString *VC64PALorNTSCKey;

/* VC1541 */
extern NSString *VC64WarpLoadKey;

/* Audio */
extern NSString *VC64SIDFilterKey;
extern NSString *VC64SIDReSIDKey;
extern NSString *VC64SIDChipModelKey;
extern NSString *VC64SIDSamplingMethodKey;

@interface HardwareDialog : NSWindow
{
    C64Proxy *c64;
    MyController *controller;
    NSString *archiveName; 
    unsigned noOfFiles;
    
    /* System */
    IBOutlet NSPopUpButton *machineType;
    IBOutlet NSImageView *flag;
    IBOutlet NSTextField *systemText;
    IBOutlet NSTextField *systemText2;

    /* VC1541 */
    IBOutlet NSButton *warpLoad;
    IBOutlet NSTextField *warpText;
    IBOutlet NSImageView *diskIcon;
    IBOutlet NSButton *diskEject;
    IBOutlet NSTextField *diskEjectText;
    IBOutlet NSTextField *diskText;
    IBOutlet NSTextField *diskText2;
    IBOutlet NSButton *writeProtected;

    /* Expansion port */
    IBOutlet NSImageView *cartridgeIcon;
    IBOutlet NSButton *cartridgeEject;
    IBOutlet NSTextField *cartridgeEjectText;
    IBOutlet NSTextField *cartridgeText;
    IBOutlet NSTextField *cartridgeText2;
    
    /* Audio */
    IBOutlet NSButton *SIDFilter;
    IBOutlet NSButton *SIDUseReSID;
    IBOutlet NSPopUpButton *SIDChipModel;
    IBOutlet NSPopUpButton *SIDSamplingMethod;
}

- (void) initialize:(MyController *)mycontroller archiveName:(NSString *)name noOfFiles:(unsigned)files;
- (void) update;

// System
- (IBAction)setPalAction:(id)sender;
- (IBAction)setNtscAction:(id)sender;

// VC 1541
- (IBAction)warpLoadAction:(id)sender;
- (IBAction)ejectDiskAction:(id)sender;
- (IBAction)writeProtectionAction:(id)sender;

// Expansion port
- (IBAction)ejectCartridgeAction:(id)sender;

// SID
- (IBAction)SIDFilterAction:(id)sender;
- (IBAction)SIDReSIDAction:(id)sender;
- (IBAction)SIDSamplingMethodAction:(id)sender;
- (IBAction)SIDChipModelAction:(id)sender;

// Action buttons
- (IBAction)useAsDefaultAction:(id)sender;
- (IBAction)factorySettingsAction:(id)sender;

@end

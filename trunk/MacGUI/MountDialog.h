/*
 * (C) 2006 Dirk W. Hoffmann. All rights reserved.
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

class Archive;

@interface MountDialog : NSWindow <NSTableViewDelegate, NSTableViewDataSource>
{
	IBOutlet NSTableView *directory;
	IBOutlet NSButton *OKButton;
    IBOutlet NSButton *CancelButton;
    IBOutlet NSPopUpButton *loadOptions;
    IBOutlet NSTextField *headerText;
    IBOutlet NSTextField *warningText;
    IBOutlet NSButton *writeProtect;
    IBOutlet NSImageView *diskIcon;
    IBOutlet NSBox *diskIconFrame;
    
    // Internal state
    Archive *archive;
    C64Proxy *c64;
    int loadOption;
    int selectedRow;
    
    // Configuration
    //bool showEjectButton;
    //bool showCancelButton;

    // Todo items (when window closes)
    bool doMount;
    bool doFlash;
    bool doType;
    bool doEjectOnCancel;
}

// @property(readonly) int loadOption;
@property(readonly) Archive *archive;
@property(readonly) bool doMount;
@property(readonly) bool doFlash;
@property(readonly) bool doType;
@property(readonly) bool doEjectOnCancel;


// Initialization
// - (void) initialize:(Archive *)a c64proxy:(C64Proxy *)proxy mountBeforeLoading:(bool)mount;
- (void) _initialize:(Archive *)a c64proxy:(C64Proxy *)proxy;
- (void) initializeAsMountDialog:(Archive *)a c64proxy:(C64Proxy *)proxy;
- (void) initializeAsDriveDialog:(Archive *)a c64proxy:(C64Proxy *)proxy;

// Action methods
- (IBAction)writeProtectAction:(id)sender;
- (IBAction)loadOptionsAction:(id)sender;
- (void)singleClickAction:(id)sender;
- (void)doubleClickAction:(id)sender;

// Table
- (int)numberOfRowsInTableView:(NSTableView *)aTableView;
- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(int)row;
- (BOOL)tableView:(NSTableView *)aTableView shouldSelectRow:(NSInteger)row;

// Misc
- (int)selection;
- (NSString *)selectedFilename;
- (NSString *)loadCommand;
- (void)update;

@end

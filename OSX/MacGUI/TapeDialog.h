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

class TAPArchive;

@interface TapeDialog : NSWindow <NSTableViewDelegate, NSTableViewDataSource>
{
    IBOutlet NSButton *OKButton;
    IBOutlet NSButton *CancelButton;
    IBOutlet NSButton *typeButton;
    IBOutlet NSButton *pressPlayButton;
    IBOutlet NSTextField *headerText;
    IBOutlet NSBox *diskIconFrame;
    
    // Internal state
    TAPArchive *archive;
    C64Proxy *c64;
    
    // Todos after pressing "Insert tape"
    bool doAutoType;
    bool doPressPlay;
}

@property(readonly) TAPArchive *archive;
@property(readonly) bool doAutoType;
@property(readonly) bool doPressPlay;

// Initialization
- (void) initialize:(TAPContainerProxy *)cproxy c64proxy:(C64Proxy *)proxy;

// Action methods
- (IBAction)autoTypeAction:(id)sender;
- (IBAction)pressPlayAction:(id)sender;

// Misc
- (void)update;

@end

/*
 * (C) 2008 Dirk W. Hoffmann. All rights reserved.
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

#if 0

#import <Cocoa/Cocoa.h>

@interface RomDialog : NSWindow
{
	IBOutlet NSTextField *learnMore;
	IBOutlet NSImageView *kernelRom;
	IBOutlet NSImageView *basicRom;
	IBOutlet NSImageView *characterRom;
	IBOutlet NSImageView *VC1541Rom;
	IBOutlet NSBox *dragBox;
	IBOutlet NSButton *cancelButton;
}

- (IBAction)helpAction:(id)sender;

- (void) initialize:(int)missingRoms;
- (void) update:(int)missingRoms;

@end

#endif


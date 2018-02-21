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

#import "C64GUI.h"

@implementation RomDialog


- (IBAction)helpAction:(id)sender
{
	[[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@"http://www.dirkwhoffmann.de/virtualc64/ROMs.html"]];	
}

- (void)initialize:(int)missingRoms 
{
	[self update:missingRoms];
}

- (void) update:(int)missingRoms
{
	if (missingRoms & KERNEL_ROM)
		[kernelRom setImage:[NSImage imageNamed:@"romchipMissing"]];
	else
		[kernelRom setImage:[NSImage imageNamed:@"romchip"]];
	if (missingRoms & BASIC_ROM)
		[basicRom setImage:[NSImage imageNamed:@"romchipMissing"]];
	else
		[basicRom setImage:[NSImage imageNamed:@"romchip"]];
	if (missingRoms & CHAR_ROM)
		[characterRom setImage:[NSImage imageNamed:@"romchipMissing"]];
	else
		[characterRom setImage:[NSImage imageNamed:@"romchip"]];
	if (missingRoms & VC1541_ROM)
		[VC1541Rom setImage:[NSImage imageNamed:@"romchipMissing"]];
	else
		[VC1541Rom setImage:[NSImage imageNamed:@"romchip"]];
}

@end

#endif

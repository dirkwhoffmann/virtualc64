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

#import "C64GUI.h"

@implementation RomDialog

#if 0
-(void)awakeFromNib
{
	NSLog(@"RomDialog::awakeFromNib");

	// Create clickable URL string
	[learnMore setAllowsEditingTextAttributes: YES];
	[learnMore setSelectable: YES];
 	
	NSURL* url = [NSURL URLWithString:@"http://www.dirkwhoffmann.de"];
 	
	
	NSMutableAttributedString* attrString = [[NSMutableAttributedString alloc] initWithString:@"www.dirkwhoffmann.de/virtualC64/ROMs.html"];
	NSRange range = NSMakeRange(0, [attrString length]);
 	
	[attrString beginEditing];
	[attrString addAttribute:NSLinkAttributeName value:[url absoluteString] range:range];
 	
	// make the text appear in blue
	[attrString addAttribute:NSForegroundColorAttributeName value:[NSColor blueColor] range:range];
 	
	// next make the text appear with an underline
	[attrString addAttribute:
 	 NSUnderlineStyleAttributeName value:[NSNumber numberWithInt:NSSingleUnderlineStyle] range:range];
 	
	[attrString endEditing];
 	
	NSMutableAttributedString* string = [[NSMutableAttributedString alloc] initWithString:@"Learn more about compatible ROMs at "];
	[string appendAttributedString:attrString];
	
	[learnMore setAttributedStringValue:string];	
}
#endif

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
		[kernelRom setImage:[NSImage imageNamed:@"romMissing"]];
	else
		[kernelRom setImage:[NSImage imageNamed:@"rom"]];
	if (missingRoms & BASIC_ROM)
		[basicRom setImage:[NSImage imageNamed:@"romMissing"]];
	else
		[basicRom setImage:[NSImage imageNamed:@"rom"]];
	if (missingRoms & CHAR_ROM)
		[characterRom setImage:[NSImage imageNamed:@"romMissing"]];
	else
		[characterRom setImage:[NSImage imageNamed:@"rom"]];
	if (missingRoms & VC1541_ROM)
		[VC1541Rom setImage:[NSImage imageNamed:@"romMissing"]];
	else
		[VC1541Rom setImage:[NSImage imageNamed:@"rom"]];
}

@end

//
//  RomDialog.m
//  V64
//
//  Created by Dirk Hoffmann on 04.08.08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "MyDocument.h"

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

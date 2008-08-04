//
//  RomDialog.m
//  V64
//
//  Created by Dirk Hoffmann on 04.08.08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "RomDialog.h"


@implementation RomDialog

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

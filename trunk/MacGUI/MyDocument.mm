/*
 * (C) 2006 - 2009 Dirk W. Hoffmann. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published byc64
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

@implementation MyDocument

@synthesize c64;
@synthesize archive;
@synthesize cartridge;
@synthesize snapshot;

- (void)makeWindowControllers
{
	NSLog(@"MyDocument::makeWindowControllers");
	
	MyController *myController;
	
	myController = [[[MyController alloc] initWithWindowNibName:@"MyDocument"] autorelease];
	[self addWindowController:myController];
	
	[[self windowForSheet] setContentAspectRatio:NSMakeSize(652,432)];
}

- (id)init
{
	NSLog(@"MyDocument::init");
	
    self = [super init];

	// Initialize variables
	// archive = NULL;
	cartridge = NULL;
	snapshot = NULL;
	
	// Create virtual C64
	c64 = [[C64Proxy alloc] init];
	
	return self;
}

- (BOOL)setArchiveWithName:(NSString *)path
{
	
	if ((archive = D64Archive::archiveFromArbitraryFile([path UTF8String])) != NULL)
		return YES;
	
	return NO;
}

- (BOOL)setCartridgeWithName:(NSString *)path
{
	if (!(cartridge = Cartridge::cartridgeFromFile([path UTF8String])))
		return NO;
	
	// Try to mount archive
	[c64 attachCartridge:cartridge];
	
	// reset
	[c64 reset];
	
	return YES;
}

- (BOOL)detachCartridge
{
	[c64 detachCartridge];	
	delete cartridge;
	cartridge = NULL;
	
	return YES;
}

- (BOOL)loadRom:(NSString *)filename
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	bool success = NO;
	
	if ([c64 loadBasicRom:filename]) {
		[defaults setObject:filename forKey:VC64BasicRomFileKey];
		success = YES;
	}
	else if ([c64 loadCharRom:filename]) {
		[defaults setObject:filename forKey:VC64CharRomFileKey];
		success = YES;
	}
	else if ([c64 loadKernelRom:filename]) {
		[defaults setObject:filename forKey:VC64KernelRomFileKey];
		success = YES;
	}
	else if ([c64 loadVC1541Rom:filename]) {
		[defaults setObject:filename forKey:VC64VC1541RomFileKey];
		success = YES;
	}
	
	return success;
}

-(BOOL)writeToFile:(NSString *)filename ofType:(NSString *)type
{
	NSLog(@"MyDocument:writeToFile:%@ ofType:%@", filename, type);
	
	if (![type isEqualToString:@"VC64"]) {
		NSLog(@"File is not of type VC64\n");
		return NO;
	}
	
	V64Snapshot *s = [V64Snapshot snapshotFromC64:c64];
	[s writeDataToFile:filename];
	[s release];
	
	return YES;
}

-(BOOL)readFromFile:(NSString *)filename ofType:(NSString *)type
{
	NSLog(@"MyDocument:readFromFile:%@ ofType:%@", filename, type);
	
	if (![type isEqualToString:@"VC64"]) {
		NSLog(@"File is not of type VC64\n");
		return NO;
	}
	
	snapshot = Snapshot::snapshotFromFile([filename UTF8String]);
		
	if (!snapshot) {
		NSLog(@"Error while reading snapshot\n");
		return NO;
	}

	return YES;
}
	
- (BOOL)revertToSavedFromFile:(NSString *)filename ofType:(NSString *)type
{
	NSLog(@"MyDocument:revertToSavedFile:%@ ofType:%@", filename, type);
	
	if (![type isEqualToString:@"VC64"]) {
		NSLog(@"File is not of type VC64\n");
		return NO;
	}
		
	snapshot = Snapshot::snapshotFromFile([filename UTF8String]);

	if (!snapshot) {
		NSLog(@"Error while reading snapshot\n");
		return NO;
	}
	
	[c64 initWithContentsOfSnapshot:snapshot];
	delete snapshot;
	snapshot = NULL;

	return YES;
}


@end

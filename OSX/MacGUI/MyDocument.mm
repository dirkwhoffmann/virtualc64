/*
 * (C) 2006 - 2016 Dirk W. Hoffmann. All rights reserved.
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
@synthesize snapshot;
@synthesize attachedArchive;
@synthesize attachedTape;
@synthesize cartridge;

- (void)makeWindowControllers
{
	NSLog(@"MyDocument::makeWindowControllers");
    
	MyController *myController;
	
	myController = [[MyController alloc] initWithWindowNibName:@"MyDocument"];
	[self addWindowController:myController];
	
    // Kepp aspect ratio during window resizing
	[[self windowForSheet] setContentAspectRatio:NSMakeSize(804,621 + 20 /* control bar below C64 canvas */)];

    // Restore previous position and size
    [[self windowForSheet] setFrameUsingName:@"dirkwhoffmann.de.virtualC64.window"];
}

- (id)init
{
	NSLog(@"MyDocument::init");
	
    self = [super init];

    attachedArchive = nil;
	cartridge = NULL;
		
	return self;
}

- (void)awakeFromNib
{
	NSLog(@"MyDocument::awakeFromNib");
}

- (BOOL)setSnapshotWithName:(NSString *)path
{
    if (!(snapshot = Snapshot::snapshotFromFile([path UTF8String])))
        return NO;
    
    return YES;
}

- (BOOL)setG64ArchiveWithName:(NSString *)path
{
    if (!(attachedArchive = [G64ArchiveProxy archiveFromG64File:path]))
        return NO;
    
    return YES;
}

- (BOOL)setNIBArchiveWithName:(NSString *)path
{
    if (!(attachedArchive = [NIBArchiveProxy archiveFromNIBFile:path]))
    return NO;
    
    return YES;
}

- (BOOL)setTAPArchiveWithName:(NSString *)path
{
    if (!(attachedTape = [TAPContainerProxy containerFromTAPFile:path]))
        return NO;
    
    return YES;
}

- (BOOL)setArchiveWithName:(NSString *)path
{
    if (!(attachedArchive = [D64ArchiveProxy archiveFromArbitraryFile:path]))
		return NO;
	
	return YES;
}

- (BOOL)setCartridgeWithName:(NSString *)path
{	
	if (!(cartridge = Cartridge::cartridgeFromFile([path UTF8String])))
		return NO;
	
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

- (void)showVersionNumberAlert
{
    NSAlert *alert = [[NSAlert alloc] init];
    [alert setMessageText:@"Snapshot from other VirtualC64 release"];
    [alert setInformativeText:@"The snapshot was created with a different version of VirtualC64 and cannot be opened."];
    [alert addButtonWithTitle:@"Ok"];
    [alert runModal];
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
	
	return YES;
}

-(BOOL)readFromFile:(NSString *)filename ofType:(NSString *)type
{
    const char *name = [filename UTF8String];
    
	NSLog(@"MyDocument:readFromFile:%@ ofType:%@", filename, type);
	
    // Is it a snapshot?
    if (Snapshot::isSnapshot(name)) {
        
        // Do the version numbers match?
        if (Snapshot::isSnapshot(name, V_MAJOR, V_MINOR, V_SUBMINOR)) {
            
            if (![self setSnapshotWithName:filename]) {
                NSLog(@"Error while reading snapshot\n");
                return NO;
            }
            return YES;
        }
        
        [self showVersionNumberAlert];
        return NO;
    }
    
    // New code style. Use ProxyClasses
    // Is it a T64 file?
#if 0
    if ([T64ArchiveProxy isT64File:filename]) {
        if ([self setArchiveWithName:filename]) return YES; else goto failure;
    }
#endif
    
    // Old code style. TODO: Change to new style (?!)...
    switch (Container::typeOf([type UTF8String])) {
            
        case D64_CONTAINER:
        case T64_CONTAINER:
        case PRG_CONTAINER:
        case P00_CONTAINER:
		
            if ([self setArchiveWithName:filename])
                return YES;
            else break;
            
        case G64_CONTAINER:
            
            if ([self setG64ArchiveWithName:filename])
                return YES;
            else break;

        case NIB_CONTAINER:
            
            if ([self setNIBArchiveWithName:filename])
                return YES;
            else break;

        case TAP_CONTAINER:
            
            if ([self setTAPArchiveWithName:filename])
                return YES;
            else break;

        case CRT_CONTAINER:

            if ([self setCartridgeWithName:filename])
                return YES;
            else break;

        default:

            NSLog(@"Unsupported file type\n");
            return NO;
    }

failure:

    NSLog(@"Error while reading file\n");
    return NO;
}
	
- (BOOL)revertToSavedFromFile:(NSString *)filename ofType:(NSString *)type
{
	NSLog(@"MyDocument:revertToSavedFile:%@ ofType:%@", filename, type);
	
	if (![type isEqualToString:@"VC64"]) {
		NSLog(@"File is not of type VC64\n");
		return NO;
	}
			
	V64Snapshot *reverted = [V64Snapshot snapshotFromFile:filename];

	if (!reverted) {
		NSLog(@"Error while reverting to older snapshot\n");
		return NO;
	}
		
	[c64 loadFromSnapshot:reverted];
	return YES;
}

@end

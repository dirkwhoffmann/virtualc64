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

#import "C64GUI.h"

#define LOAD_OPTION_8_1 0
#define LOAD_OPTION_8 1
#define LOAD_OPTION_FLASH 2

@implementation MountDialog

@synthesize archive;
@synthesize doMount;
@synthesize doFlash;
@synthesize doType;

- (void) _initialize:(Archive *)a c64proxy:(C64Proxy *)proxy
{
    assert(a != NULL);
    assert(proxy != NULL);
    
    archive = a;
    c64 = proxy;
    
    bool isG64 = (archive->getType() == G64_CONTAINER);
    doMount = YES;
    doFlash = NO;
    doType = NO;
    loadOption = LOAD_OPTION_FLASH;

    // Let the table header show the logical archive name
    NSString *archiveName = [NSString stringWithFormat:@"%s", archive->getName()];
    [[[directory tableColumnWithIdentifier:@"filename"] headerCell] setStringValue:archiveName];
    
    // Establish necessary binding
    [directory deselectAll:self];
    [directory setTarget:self];
    [directory setDelegate:self];
    [directory setDataSource:self];
    if (!isG64) {
        [directory setAction:@selector(singleClickAction:)];
        [directory setDoubleAction:@selector(doubleClickAction:)];
        // Select first entry
        NSIndexSet *indexSet = [NSIndexSet indexSetWithIndex:0];
        [directory selectRowIndexes:indexSet byExtendingSelection:NO];
    }
    
    [doubleClickText setHidden:isG64];
    [loadOptions setHidden:isG64];
    [directory reloadData];
}

- (void) initialize:(Archive *)a c64proxy:(C64Proxy *)proxy
{
    [self _initialize:a c64proxy:proxy];
    
    [headerText setStringValue:@"Archive"];
    
    // Get physical path of archive
    NSString *archivePath = [NSString stringWithFormat:@"%s", archive->getPath()];
    NSString *archiveLastPath = [archivePath lastPathComponent];
    NSString *archiveExtension = [[archiveLastPath pathExtension] uppercaseString];
    
    // Set icon and title
    [diskIconFrame setTitle:archiveLastPath];
    
    if ([archiveExtension isEqualToString:@"D64"]) {
        
        [diskIcon setImage:[NSImage imageNamed:@"IconD64"]];
        
    } else if ([archiveExtension isEqualToString:@"T64"]) {
        
        [diskIcon setImage:[NSImage imageNamed:@"IconT64"]];
        
    } else if ([archiveExtension isEqualToString:@"PRG"]) {
        
        [diskIcon setImage:[NSImage imageNamed:@"IconPRG"]];

    } else if ([archiveExtension isEqualToString:@"P00"]) {
        
        [diskIcon setImage:[NSImage imageNamed:@"IconP00"]];
    }
    
    [CancelButton setTitle:@"Cancel"];
    [OKButton setTitle:@"Insert disk"];
    
    [self update];
}

- (int)selection
{
    return [directory selectedRow];
}

- (NSString *)selectedFilename
{
    NSString *result;
    
    if ([directory selectedRow] < 0) {
        result = @"";
    } else {
        result = [NSString stringWithFormat:@"%s", archive->getNameOfItem([directory selectedRow])];
    }
    
    return result;
}

- (NSString *)loadCommand
{
    NSLog(@"loadCommand");
    
    if ([directory selectedRow] < 0)
        return @"";

    NSString *name = [self selectedFilename];
    
    switch (loadOption) {
            
        case LOAD_OPTION_8_1:
            return [NSString stringWithFormat:@"LOAD \"%@\",8,1", name];
            
        case LOAD_OPTION_8:
            return [NSString stringWithFormat:@"LOAD \"%@\",8", name];
            
        case LOAD_OPTION_FLASH:
            return [NSString stringWithFormat:@"RUN"];
            
        default:
            assert(false);
    }

    return nil;
}

- (void)update
{
    // NSString *cmd = [self loadCommand];
    [[loadOptions itemAtIndex:0] setTitle:[NSString stringWithFormat:@"LOAD \"%@\",8,1",[self selectedFilename]]];
    [[loadOptions itemAtIndex:1] setTitle:[NSString stringWithFormat:@"LOAD \"%@\",8",[self selectedFilename]]];
    [loadOptions selectItemAtIndex:loadOption];
    
    [warningText setHidden:loadOption != LOAD_OPTION_FLASH || archive->getType() == G64_CONTAINER];
}

#pragma mark NSTableViewDataSource

- (int)numberOfRowsInTableView:(NSTableView *)aTableView
{
	if (archive == NULL)
		return 0;

	return archive->getNumberOfItems();
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(int)row
{
	if ([[aTableColumn identifier] isEqual:@"filename"]) {
      //  return [NSString stringWithFormat:@"%s", archive->getNameOfItem(row)];
        const char *itemName = archive->getNameOfItem(row);
        assert(itemName != NULL);

        unichar uName[18];
        memset(uName, 0, sizeof(uName));
        for (unsigned i = 0; i < strlen(itemName) && i < 18; i++)
            uName[i] = pet2unicode(itemName[i]);
        
        NSString *unicodename = [NSString stringWithCharacters:uName length:17];
        NSLog(@"%@", unicodename);
        return unicodename;
    }
	if ([[aTableColumn identifier] isEqual:@"filesize"]) {
        if (archive->getType() == G64_CONTAINER) {
            return @((int)archive->getSizeOfItem(row));
        } else {
            return @((int)archive->getSizeOfItemInBlocks(row));
        }
	}
	if ([[aTableColumn identifier] isEqual:@"filetype"]) {
		return [NSString stringWithFormat:@"%s", archive->getTypeOfItem(row)];
	}
	return @"???";
}

#pragma mark NSTableViewDelegate

- (BOOL)tableView:(NSTableView *)aTableView shouldSelectRow:(NSInteger)row
{
    // NSLog(@"Should select");
    // return (strcmp(archive->getTypeOfItem(row), "PRG") == 0);
    return YES;
}

- (NSCell *)tableView:(NSTableView *)tableView dataCellForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
    NSTextFieldCell *cell = [tableColumn dataCell];

    if (archive->getType() == G64_CONTAINER) {
        if (archive->getSizeOfItem(row) == 0) {
            [cell setTextColor:[NSColor grayColor]];
        } else {
            [cell setTextColor:[NSColor blackColor]];
        }
    } else {
        if(strcmp(archive->getTypeOfItem(row), "PRG")) {
            [cell setTextColor:[NSColor grayColor]];
        } else {
            [cell setTextColor:[NSColor blackColor]];
        }
    }
    
    return cell;
}

#pragma mark Action methods

- (IBAction)loadOptionsAction:(id)sender
{
    loadOption = [[sender selectedItem] tag];
    [self update];
}

- (void)singleClickAction:(id)sender
{
    selectedRow = [directory selectedRow];
    [self update];
}

- (void)doubleClickAction:(id)sender
{
    selectedRow = [directory selectedRow];
    [self update];
    
    doType = (selectedRow >= 0);
    doFlash = (loadOption == LOAD_OPTION_FLASH && selectedRow >= 0);
    
	[OKButton performClick:self];
}

@end

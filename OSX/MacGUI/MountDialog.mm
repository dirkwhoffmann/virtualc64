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

- (void) _initialize:(ArchiveProxy *)aproxy c64proxy:(C64Proxy *)proxy
{
    assert(aproxy != NULL);
    assert(proxy != NULL);
    
    archive = aproxy;
    c64 = proxy;
    
    cbmfont = [NSFont fontWithName:@"C64ProMono" size: 10];
    cbmfontsmall = [NSFont fontWithName:@"C64ProMono" size: 8];
    
    bool isG64orNIB = ([aproxy getType] == G64_CONTAINER || [aproxy getType] == NIB_CONTAINER);
    doMount = YES;
    doFlash = NO;
    doType = NO;
    loadOption = LOAD_OPTION_8_1;

    // Let the table header show the logical archive name
    /*
    NSTableHeaderCell *hc = [[directory tableColumnWithIdentifier:@"filename"] headerCell];
    [hc setStringValue:[aproxy getName]];
    [hc setFont:cbmfont];
    */
    
    // Establish necessary binding
    [directory deselectAll:self];
    [directory setTarget:self];
    [directory setDelegate:self];
    [directory setDataSource:self];
    [directory setIntercellSpacing:NSMakeSize(0, 0)];
    if (!isG64orNIB) {
        [directory setAction:@selector(singleClickAction:)];
        [directory setDoubleAction:@selector(doubleClickAction:)];
        // Select first entry
        NSIndexSet *indexSet = [NSIndexSet indexSetWithIndex:0];
        [directory selectRowIndexes:indexSet byExtendingSelection:NO];
    }
    [doubleClickText setHidden:isG64orNIB];
    [loadOptions setHidden:isG64orNIB];
    [directory reloadData];
}

- (void) initialize:(ArchiveProxy *)aproxy c64proxy:(C64Proxy *)proxy
{
    NSLog(@"Initialize MountDialog");
    
    [self _initialize:aproxy c64proxy:proxy];
    
    // Get physical path of archive
    NSString *archivePath = [aproxy getPath];
    NSString *archiveLastPath = [archivePath lastPathComponent];
    NSString *archiveExtension = [[archiveLastPath pathExtension] uppercaseString];
    
    // Set icon and title
    [headerText setStringValue:archivePath];
    
    if ([archiveExtension isEqualToString:@"D64"]) {
        
        [diskIcon setImage:[NSImage imageNamed:@"IconD64"]];
        [diskIconFrame setTitle:@"D64 archive"];
        loadOption = LOAD_OPTION_8_1;
        
    } else if ([archiveExtension isEqualToString:@"T64"]) {
        
        [diskIcon setImage:[NSImage imageNamed:@"IconT64"]];
        [diskIconFrame setTitle:@"T64 archive"];
        loadOption = LOAD_OPTION_FLASH;
        
    } else if ([archiveExtension isEqualToString:@"PRG"]) {
        
        [diskIcon setImage:[NSImage imageNamed:@"IconPRG"]];
        [diskIconFrame setTitle:@"PRG archive"];
        loadOption = LOAD_OPTION_FLASH;

    } else if ([archiveExtension isEqualToString:@"P00"]) {
        
        [diskIcon setImage:[NSImage imageNamed:@"IconP00"]];
        [diskIconFrame setTitle:@"P00 archive"];
        loadOption = LOAD_OPTION_FLASH;
    }
    
    [self update];
}

- (long)selection
{
    return [directory selectedRow];
}

- (NSString *)selectedFilename
{
    NSString *result;
    
    if ([directory selectedRow] < 0) {
        result = @"";
    } else {
        result = [archive getNameOfItem:[directory selectedRow]];
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
    [[loadOptions itemAtIndex:0] setTitle:[NSString stringWithFormat:@"LOAD \"%@\",8,1",[self selectedFilename]]];
    [[loadOptions itemAtIndex:1] setTitle:[NSString stringWithFormat:@"LOAD \"%@\",8",[self selectedFilename]]];
    /*
    [loadOptions itemAtIndex:0] setTitle:[NSString stringWithFormat:@"LOAD \"...\",8,1"]];
    [[loadOptions itemAtIndex:1] setTitle:[NSString stringWithFormat:@"LOAD \"...\",8"]];
    [loadOptions selectItemAtIndex:loadOption];
    */
    [warningText setHidden:loadOption != LOAD_OPTION_FLASH || [archive getType] == G64_CONTAINER];
}

#pragma mark NSTableViewDataSource

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
	if (archive == NULL)
		return 0;

    return [archive getNumberOfItems];
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(int)row
{
	if ([[aTableColumn identifier] isEqual:@"filename"]) {
        
        NSString *name = [archive getUnicodeNameOfItem:row maxChars:16];
        return name;
    }
    
	if ([[aTableColumn identifier] isEqual:@"filesize"]) {
        
        if ([archive getType] == G64_CONTAINER) {

            return @((int)[archive getSizeOfItem:row]);
        } else {
            return @((int)[archive getSizeOfItemInBlocks:row]);
        }
	}
    
	if ([[aTableColumn identifier] isEqual:@"filetype"]) {
        
        return [archive getTypeOfItem:row];
	}
    
	return @"???";
}

#pragma mark NSTableViewDelegate

- (void)tableView:(NSTableView *)tableView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
    // NSColor *bgcolor1 = [[c64 vic] color:6];
    // NSColor *bgcolor2 = [[c64 vic] color:6];
    NSColor *textcolor = [NSColor blackColor];
    NSColor *textcolorNonselect = [NSColor grayColor];
    
    [cell setFont:cbmfont];

    /*
    [cell setDrawsBackground:YES];
        if (row % 2) {
        [cell setBackgroundColor:bgcolor1];
    } else {
        [cell setBackgroundColor:bgcolor2];
    }
    */
    
    if ([archive getType] == G64_CONTAINER) {
        if ([archive getSizeOfItem:row] == 0) {
            [cell setTextColor:textcolorNonselect];
        } else {
            [cell setTextColor:textcolor];
        }
    } else {
        if([[archive getTypeOfItem:row] isEqualToString:@"PRG"]) {
            [cell setTextColor:textcolor];
        } else {
            [cell setTextColor:textcolorNonselect];
        }
    }
}

- (BOOL)tableView:(NSTableView *)aTableView shouldSelectRow:(NSInteger)row
{
    // NSLog(@"Should select");
    // return (strcmp(archive->getTypeOfItem(row), "PRG") == 0);
    return YES;
}

#if 0
- (NSCell *)tableView:(NSTableView *)tableView dataCellForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
    NSTextFieldCell *cell = [tableColumn dataCell];

    [cell setFont:cbmfont];
    [cell setBackgroundColor:darkblue];
    
    if ([archive getType] == G64_CONTAINER) {
        if ([archive getSizeOfItem:row] == 0) {
            [cell setTextColor:[NSColor grayColor]];
        } else {
            [cell setTextColor:lightblue];
        }
    } else {
        if([[archive getTypeOfItem:row] isEqualToString:@"PRG"]) {
            [cell setTextColor:lightblue];
        } else {
            [cell setTextColor:[NSColor grayColor]];
        }
    }
    
    return cell;
}
#endif

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

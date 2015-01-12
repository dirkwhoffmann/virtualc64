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

@implementation MountDialog

@synthesize archive;
@synthesize doMount;
@synthesize doFlash;
@synthesize doType;

- (void) initialize:(Archive *)a c64proxy:(C64Proxy *)proxy mountBeforeLoading:(bool)mount
{
	assert(a != NULL);

    archive = a;
    loadOption = 1;
    selectedRow = -1;
    
    c64 = proxy;
    doMount = mount;
    
    [CancelButton setHidden:!cancel];
    
    NSString *archiveName = [NSString stringWithFormat:@"%s", archive->getName()];
    NSString *archivePath = [NSString stringWithFormat:@"%s", archive->getPath()];
    NSString *archiveLastPath = [archivePath lastPathComponent];
    NSString *archiveExtension = [archiveLastPath pathExtension];
    
    [[[directory tableColumnWithIdentifier:@"filename"] headerCell] setStringValue:archiveName];
    
    if ([archiveExtension isEqualToString:@"T64"])
        [diskIcon setImage:[NSImage imageNamed:@"IconT64"]];
    else if ([archiveExtension isEqualToString:@"D64"])
        [diskIcon setImage:[NSImage imageNamed:@"IconD64"]];
    else if ([archiveExtension isEqualToString:@"PRG"])
        [diskIcon setImage:[NSImage imageNamed:@"IconPRG"]];
    else if ([archiveExtension isEqualToString:@"P00"])
        [diskIcon setImage:[NSImage imageNamed:@"IconP00"]];

    // NSLog(@"path %@",archivePath);
    // NSLog(@"lastPath %@",archiveLastPath);
    // NSLog(@"extension %@",archiveExtension);

    [diskIconFrame setTitle:archiveLastPath];
    // [writeProtect setTag:[[c64prox VC1541Proxy] writeProtection];
    
    [directory deselectAll:self];
    [directory setTarget:self];
    [directory setDelegate:self];
    [directory setDataSource:self];
    [directory setAction:@selector(singleClickAction:)];
	[directory setDoubleAction:@selector(doubleClickAction:)];
	[directory reloadData];

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
    
    if ([directory selectedRow] < 0)
        return @"";

    NSString *name = [self selectedFilename];
                      
    switch (loadOption) {
            
        case 1:
            return [NSString stringWithFormat:@"LOAD \"%@\",8,1", name];
            
        case 2:
            return [NSString stringWithFormat:@"LOAD \"%@\",8", name];
            
        case 3:
            return [NSString stringWithFormat:@"RUN"];
            
        default:
            assert(false);
    }

    return nil;
}

- (void)update
{
    NSString *cmd = [self loadCommand];
    
    doType = ([directory selectedRow] >= 0);
    doFlash = (loadOption == 3 && [directory selectedRow] >= 0);

    [loadText setStringValue:cmd];
    [loadOptions setEnabled:doType];
    [warningText setHidden:loadOption != 3];
    if (doMount && doFlash) {
        [CancelButton setHidden:NO];
        [OKButton setTitle:@"Insert and flash"];
    } else if (doFlash) {
        [CancelButton setHidden:NO];
        [OKButton setTitle:@"Flash"];
    } else if (doMount && doType) {
        [CancelButton setHidden:NO];
        [OKButton setTitle:@"Insert and load"];
    } else if (doType) {
        [CancelButton setHidden:NO];
        [OKButton setTitle:@"Load"];
    } else if (doMount) {
        [CancelButton setHidden:NO];
        [OKButton setTitle:@"Insert disc"];
    } else {
        [CancelButton setHidden:YES];
        [OKButton setTitle:@"OK"];
    }
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
		return [NSString stringWithFormat:@"%s", archive->getNameOfItem(row)];
	}
	if ([[aTableColumn identifier] isEqual:@"filesize"]) {
		return @((int)archive->getSizeOfItem(row));
	}
	if ([[aTableColumn identifier] isEqual:@"filetype"]) {
		return [NSString stringWithFormat:@"%s", archive->getTypeOfItem(row)];
	}
	return @"???";
}

#pragma mark NSTableViewDelegate

- (BOOL)tableView:(NSTableView *)aTableView shouldSelectRow:(NSInteger)row
{
    NSLog(@"Should select");
    return (strcmp(archive->getTypeOfItem(row), "PRG") == 0);
}

- (NSCell *)tableView:(NSTableView *)tableView dataCellForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
    NSTextFieldCell *cell = [tableColumn dataCell];

    if(strcmp(archive->getTypeOfItem(row), "PRG")) {
        [cell setTextColor:[NSColor grayColor]];
    } else {
        [cell setTextColor:[NSColor blackColor]];
    }
                                               
    return cell;
}

#pragma mark Action methods

- (IBAction)writeProtectAction:(id)sender
{
    if ([sender tag]) {
        NSLog(@"Write protection");
    } else {
        NSLog(@"No write protection");
    }
}

- (IBAction)loadOptionsAction:(id)sender
{
    loadOption = [[sender selectedItem] tag];
    [self update];
}

- (void)singleClickAction:(id)sender
{
    if (selectedRow == [directory selectedRow]) {
        selectedRow = -1;
        [directory deselectAll:self];
    } else {
        selectedRow = [directory selectedRow];
        [self update];
    }
}

- (void)doubleClickAction:(id)sender
{
	[OKButton performClick:self];
}

@end

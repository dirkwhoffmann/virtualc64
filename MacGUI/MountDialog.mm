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
@synthesize selectedFile;
// @synthesize selectedLoadOption;

- (void) initialize:(Archive *)a
{
	assert(a != NULL);

    archive = a;
    selectedLoadOption = 1;

    NSString *archiveName = [NSString stringWithFormat:@"%s", archive->getName()];
    NSString *archivePath = [NSString stringWithFormat:@"%s", archive->getPath()];
    NSString *archiveLastPath = [archivePath lastPathComponent];
    
    [[[directory tableColumnWithIdentifier:@"filename"] headerCell] setStringValue:archiveName];
    [diskIconFrame setTitle:archiveLastPath];
    
    
    [directory setTarget:self];
    [directory setAction:@selector(singleClickAction:)];
	[directory setDoubleAction:@selector(doubleClickAction:)];

    [self update];
    [directory setDelegate:self];
    [directory setDataSource:self];
	[directory reloadData];
}

- (NSString *)loadCommand
{
    NSString *s;
    
    switch (selectedLoadOption) {
            
        case 1:
            [warningText setHidden:YES];
            
            if ([directory selectedRow] < 0) {
                s = [NSString stringWithFormat:@""];
            } else {
                s = [NSString stringWithFormat:@"LOAD \"%@\",8,1", [self selectedFilename]];
            }
            break;
            
        case 2:
            [warningText setHidden:YES];
            
            if ([directory selectedRow] < 0) {
                s = [NSString stringWithFormat:@""];
            } else {
                s = [NSString stringWithFormat:@"LOAD \"%@\",8", [self selectedFilename]];
            }
            break;
            
        case 3:
            [warningText setHidden:NO];
            
            if ([directory selectedRow] < 0) {
                s = [NSString stringWithFormat:@""];
            } else {
                s = [NSString stringWithFormat:@"Flash %@ into memory", [self selectedFilename]];
            }
            break;
            
        default:
            assert(false);
    }

    // Remove spaces from the end
    int i;
    for (i = s.length - 1; i >= 0 && [s characterAtIndex:i] == ' '; i--);
    return [s substringToIndex:i+1];
}

- (void)update
{
    [loadText setStringValue:[self loadCommand]];
    [warningText setHidden:(selectedLoadOption != 3 || [directory selectedRow] < 0)];
    [OKButton setTitle:([directory selectedRow] < 0) ? @"Mount" : @"Load"];
}

- (int)selectedLoadOption
{
    return selectedLoadOption;
}

- (void)setSelectedLoadOption:(int)option
{
    selectedLoadOption = option;
    [self update];
 }

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
    selectedLoadOption = [[sender selectedItem] tag];
    NSLog(@"%d %d", (int)[[sender selectedItem] tag], (int)selectedLoadOption);
    [self update];
}

- (void)singleClickAction:(id)sender
{
    NSLog(@"singleClickAction");
    [self update];
}

- (void)doubleClickAction:(id)sender
{
	NSLog(@"doubleClickAction");
	[OKButton performClick:self];
}

- (int)getSelectedFile
{
	int row = [directory selectedRow];
	return (row < 0) ? 0 : row;
}

- (NSString *)selectedFilename
{
    if ([directory selectedRow] < 0) {
        return @"*";
    } else {
        return [NSString stringWithFormat:@"%s", archive->getNameOfItem([directory selectedRow])];
    }
}

@end

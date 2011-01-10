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

#import "MyDocument.h"

@implementation MountDialog

- (void) initialize:(Archive *)a
{
	assert(a != NULL);
	
	int numberOfFiles = a->getNumberOfItems();
	
	archive = a;
	[archiveName setStringValue:[NSString stringWithFormat:@"%s", archive->getName()]];
	[archiveType setStringValue:[NSString stringWithFormat:@"%d %s", numberOfFiles, numberOfFiles == 1 ? "file" : "files"]];
	if (archive->getNumberOfItems() > 0) {
		// select first item by default
		[directory selectRowIndexes:[NSIndexSet indexSetWithIndex:0] byExtendingSelection:NO];
		// register for double click events (double clicking an item will flash it into memory)
		[directory setTarget:self];
		[directory setDoubleAction:@selector(doubleClickAction:)];
	} else {
		[mountButton setEnabled:false];
		[flashButton setEnabled:false];
	}
	
	[directory reloadData];
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
		return [NSNumber numberWithInt:archive->getSizeOfItem(row)];
	}
	if ([[aTableColumn identifier] isEqual:@"filetype"]) {
		return [NSString stringWithFormat:@"%s", archive->getTypeOfItem(row)];
	}
	return @"???";
}

- (void)doubleClickAction:(id)sender
{
	NSLog(@"doubleClickAction");
	[flashButton performClick:self];
}

- (int)getSelectedFile
{
	int row = [directory selectedRow];
	return (row < 0) ? 0 : row;
}


@end

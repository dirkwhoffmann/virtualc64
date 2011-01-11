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

#import "MyDocument.h"

@implementation MyDocument

@synthesize c64;
@synthesize warpLoad;
@synthesize alwaysWarp;

- (void)makeWindowControllers
{
	NSLog(@"makeWindowControllers");
	
	MyController *myController;
	
	myController = [[[MyController alloc] initWithWindowNibName:@"MyDocument"] autorelease];
	[self addWindowController:myController];
}

- (id)init
{
	NSLog(@"init");
	
    self = [super init];
    	
	// Create virtual C64
	c64 = [[C64Proxy alloc] init];
	
	// Initialize variables (TODO)
	
	return self;
}

- (void)windowControllerDidLoadNib:(NSWindowController *) aController
{	
	NSLog(@"windowControllerDidLoadNib");
	
	[super windowControllerDidLoadNib:aController];
	
	// Fixate window aspect ratio
	[[self windowForSheet] setContentAspectRatio:NSMakeSize(652,432)];
}

@end

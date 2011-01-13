/*
 * (C) 2011 Dirk W. Hoffmann. All rights reserved.
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

@implementation CheatboxItem

@synthesize image;
@synthesize imageID;
@synthesize imageTitle;
@synthesize imageSubtitle;

- (id)initWithImage:(NSImage *)img imageID:(NSString *)imgID imageTitle:(NSString *)title imageSubtitle:(NSString *)subtitle
{
	if ((self = [super init])) {
		image = [img copy];
		imageID = [imgID copy];
		imageTitle = [title copy];
		imageSubtitle = [subtitle copy];
	}
	return self;
}

- (void)dealloc
{
	[image release];
	[imageID release];
	[super dealloc];
}

- (NSString *) imageUID
{
	return imageID;
}
- (NSString *) imageRepresentationType
{
	return IKImageBrowserNSImageRepresentationType;
}
- (id) imageRepresentation
{
	return image;
}

- (NSString*) imageTitle
{
	return imageTitle;
}

#if 0
- (NSRect)imageContainerFrame
{
	NSLog(@"imageContainerFrame");
	return NSMakeRect(2,2,40,20);
}

- (NSRect)imageFrame
{
	NSLog(@"imageFrame");
	return NSMakeRect(2,2,40,20);
}
#endif

@end


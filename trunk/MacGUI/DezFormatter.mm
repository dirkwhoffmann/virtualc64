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

@implementation DezFormatter
- (id)init
{
	self = [super init];
    if (self) {	
		format = @"%03d";
		min    = 0;
		max    = 255;
	}
	return self;
}

- (id)init:(NSString *)format_str min:(int)x max:(int)y
{
	self = [super init];
    if (self) {	
		format = format_str;
		min    = x;
		max    = y;
	}
	return self;
}

- (BOOL)getObjectValue:(id *)obj forString:(NSString *)string errorDescription:(NSString **)error
{
	int result;
    NSScanner *scanner;
    BOOL retval = NO;
	
    scanner = [NSScanner scannerWithString: string];
	if ([scanner scanInt:&result] && ([scanner isAtEnd])) {
        retval = YES;
        if (obj)
            *obj = [NSNumber numberWithInt:result];
    } else {
        if (error)
            *error = NSLocalizedString(@"Invalid decimal format", @"Error converting");
    }
    return retval;
}

- (NSString *)stringForObjectValue:(id)anObject
{
    if (![anObject isKindOfClass:[NSNumber class]]) {
        return nil;
    }
    return [NSString stringWithFormat:format, [anObject intValue]];
}
	
@end

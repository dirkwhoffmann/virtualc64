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


@implementation Formatter

- (id)init
{
	self = [super init];
	return self;
}

- (id)init:(int)_type inFormat:(NSString *)_inFormat outFormat:(NSString *)_outFormat
{
	self = [super init];
    if (self) {	
		type = _type;
		inFormat = _inFormat;
		outFormat = _outFormat;
	}
	return self;
}

- (BOOL)isPartialStringValid:(NSString *)partialString newEditingString:(NSString **)newString errorDescription:(NSString **)error
{
	NSString *regexTestString = [NSString stringWithString:inFormat];
	NSPredicate *regextest = [NSPredicate predicateWithFormat:@"SELF MATCHES %@", regexTestString];
	BOOL isOK = [regextest evaluateWithObject:partialString];

	if (!isOK) {
		*newString = nil; 
		NSBeep(); 
		return NO;
	}
	return YES;
}

- (BOOL)getObjectValue:(id *)obj forString:(NSString *)string errorDescription:(NSString **)error
{
    NSScanner *scanner = [NSScanner scannerWithString: string];

	if (!obj) 
		return NO;
	
	if (type == DECIMAL_FORMATTER) {
		int result;
		if ([scanner scanInt:&result] && [scanner isAtEnd])
			*obj = @((int)result);
		else
            *obj = @0;
		return YES;
	}
	
	if (type == HEXADECIMAL_FORMATTER) {
		unsigned int result;
		if ([scanner scanHexInt:&result] && [scanner isAtEnd])
            *obj = @((int)result);
		else
			*obj = @0;
		return YES;
	}

	return NO;
}

- (NSString *)stringForObjectValue:(id)anObject
{	
    if (![anObject isKindOfClass:[NSNumber class]]) {
        return nil;
    }
    return [NSString stringWithFormat:outFormat, [anObject intValue]];
}

@end

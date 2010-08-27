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

@implementation Disassembler

- (id)init:(C64Proxy *)c byteFormatter:(NSFormatter *)bf wordFormatter:(NSFormatter *)wf
{
	self = [super init];
    if (self) {	
		c64 = c;
		byteFormatter = bf;
		wordFormatter = wf;
	}
	return self;
}

- (BOOL)getObjectValue:(id *)obj forString:(NSString *)string errorDescription:(NSString **)error
{
	*obj = nil;
	return NO;
}

- (NSString *)stringForObjectValue:(id)anObject
{	
	uint16_t addr;
	uint8_t opcode;
	char *mnemonic;
	NSString *op = nil;
	
    if (![anObject isKindOfClass:[NSNumber class]])
        return nil;

	addr     = (uint16_t)[anObject intValue];
	opcode   = [c64 memPeek:addr];
	mnemonic = [[c64 cpu] getMnemonic:opcode];
		
	// Display instruction
	switch ([[c64 cpu] getAddressingMode:opcode]) {
		case CPU::ADDR_IMPLIED:
		case CPU::ADDR_ACCUMULATOR:
			op = @"";
			break;
		case CPU::ADDR_IMMEDIATE:
		case CPU::ADDR_ZERO_PAGE:
		case CPU::ADDR_ZERO_PAGE_X:
		case CPU::ADDR_ZERO_PAGE_Y:
		case CPU::ADDR_INDIRECT_X:
		case CPU::ADDR_INDIRECT_Y:
			op = [byteFormatter stringForObjectValue:[NSNumber numberWithInt:[c64 memPeek:(addr+1)]]];
			break;
		case CPU::ADDR_DIRECT:			
		case CPU::ADDR_INDIRECT:
		case CPU::ADDR_ABSOLUTE:
		case CPU::ADDR_ABSOLUTE_X:
		case CPU::ADDR_ABSOLUTE_Y:
			op = [wordFormatter stringForObjectValue:[NSNumber numberWithInt:[c64 memPeekWord:(addr+1)]]];
			break;
		case CPU::ADDR_RELATIVE:
			op = [wordFormatter stringForObjectValue:[NSNumber numberWithInt:addr+2+(int8_t)[c64 memPeek:(addr+1)]]];
			break;
	}
			
	switch ([[c64 cpu] getAddressingMode:opcode]) {
		case CPU::ADDR_IMPLIED:
		case CPU::ADDR_ACCUMULATOR:
			return [NSString stringWithFormat:@"%s", mnemonic];
		case CPU::ADDR_IMMEDIATE:					
			return [NSString stringWithFormat:@"%s #%@", mnemonic, op];
		case CPU::ADDR_ZERO_PAGE:	
			return [NSString stringWithFormat:@"%s %@", mnemonic, op];
		case CPU::ADDR_ZERO_PAGE_X:	
			return [NSString stringWithFormat:@"%s %@,X", mnemonic, op];
		case CPU::ADDR_ZERO_PAGE_Y:	
			return [NSString stringWithFormat:@"%s %@,Y", mnemonic, op];
		case CPU::ADDR_ABSOLUTE:	
		case CPU::ADDR_DIRECT:
			return [NSString stringWithFormat:@"%s %@", mnemonic, op];
		case CPU::ADDR_ABSOLUTE_X:	
			return [NSString stringWithFormat:@"%s %@,X", mnemonic, op];
		case CPU::ADDR_ABSOLUTE_Y:	
			return [NSString stringWithFormat:@"%s %@,Y", mnemonic, op];
		case CPU::ADDR_INDIRECT:	
			return [NSString stringWithFormat:@"%s (%@)", mnemonic, op];
		case CPU::ADDR_INDIRECT_X:	
			return [NSString stringWithFormat:@"%s (%@,X)", mnemonic, op];
		case CPU::ADDR_INDIRECT_Y:	
			return [NSString stringWithFormat:@"%s (%@),Y", mnemonic, op];
		case CPU::ADDR_RELATIVE:
			return [NSString stringWithFormat:@"%s %@", mnemonic, op];
	}
	return @"???";
}

@end

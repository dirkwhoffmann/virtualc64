/*
 * (C) 2009 A. Carl Douglas. All rights reserved.
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
#include <ctype.h> // for printReadable only
#include "Cartridge.h"

static void printReadable(const void *data, int length)
{
	int i;
	for(i = 0; i < length; i++) {
		char ch = ((char*)(data))[i];
		if (isascii(ch)) {
			printf("%02x %c ", ch, ch);
		} else {
			printf("%02x ? ", ch, ch);
		}
		if (i > 0 && i % 16 == 0) {
			printf("\n");
		}
	}
}

static uint16_t getHighLow(uint8_t hi, uint8_t lo)
{
	return (uint16_t)((hi << 8) | (lo));
}

//! Constructor
Cartridge::Cartridge()
{
	path = NULL;
	data = NULL;
	size = 0;
	
	numberOfChips = 0;
	for(int i = 0; i < MAX_CHIPS; i++) {
		chip[i] = NULL;
	}
}

//! Destructor
Cartridge::~Cartridge()
{
	if (path) free(path);
	if (data) free(data);
}

bool Cartridge::load(uint8_t **buffer)
{	
#if 0
	for (unsigned i = 0; i < sizeof(watchpoint); i++)
		watchpoint[i] = read8(buffer);	
	for (unsigned i = 0; i < sizeof(watchValue); i++) 
		watchValue[i] = read8(buffer);	
#endif
	return true;
}

bool Cartridge::save(uint8_t **buffer) 
{
#if 0
	for (unsigned i = 0; i < sizeof(watchpoint); i++)
		write8(buffer, watchpoint[i]);
	for (unsigned i = 0; i < sizeof(watchValue); i++) 
		write8(buffer, watchValue[i]);
#endif
	return true;
}

void Cartridge::reset()
{
	
}

//! Check file type
/*! Returns true, iff the specifies file is a valid cartridge file. */
bool Cartridge::fileIsValid(const char *filename)
{
	int magic_bytes[] = 
		{'C','6','4',' ','C','A','R','T','R','I','D','G','E',' ',' ',' ',EOF};
	
	assert(filename != NULL);
	
	if (!checkFileSuffix(filename, ".CRT") && !checkFileSuffix(filename, ".crt"))
		return false;
	
	if (!checkFileSize(filename, 0x40, -1))
		return false;
	
	if (!checkFileHeader(filename, magic_bytes))
		return false;
	
	return true;
}

bool Cartridge::loadFile(const char *filename)
{
	struct stat fileProperties;
	FILE *file = NULL;
	int c = 0;
	int i = 0;
	
	assert (filename != NULL);
	
	// Free old data
	//eject();
	
	// Check file type
	if (!fileIsValid(filename)) 
		return false;
	
	// Get file properties
    if (stat(filename, &fileProperties) != 0) {
		// Could not open file...
		return false;
	}
	
	// Open file
	if (!(file = fopen(filename, "r"))) {
		// Can't open for read (Huh?)
		return false;
	}
	
	// Allocate memory
	if ((data = (uint8_t *)malloc(fileProperties.st_size)) == NULL) {
		return false;
	}
	
	// Read data
	c = fgetc(file);
	while(c != EOF) {
		data[size++] = c;
		c = fgetc(file);
	}
	
	assert(size == fileProperties.st_size);
	
	fclose(file);
	
	// C64 CARTRIDGE
	uint8_t magic_cartridge[] = { 'C', '6', '4', ' ', 'C', 'A', 'R', 'T', 'R', 'I', 'D', 'G', 'E', ' ', ' ', ' ' };
	if (memcmp(magic_cartridge, &data[0], 16) != 0) {
		printf("Bad cartridge signature. Expected 'C64  CARTRIDGE  ', got ...\n");
		printReadable(&data[0], 16);
		return false;
	}
	printf ("Cartridge signature OK\n");
	
	// Cartridge header size
	uint16_t hi = getHighLow(data[0x0010], data[0x0011]);
	uint16_t lo  = getHighLow(data[0x0012], data[0x0013]);
	
	printf ("Header size is: 0x%04X 0x%04X (normally 0x40)\n", hi, lo);
	
	printf("Cartridge: %s\n", &data[0x0020]);
	printf("Cartridge type: %d\n", getHighLow(data[0x0016], data[0x0017]));
	printf("EXROM = %d, GAME = %d\n", data[0x0018], data[0x0019]);
	
	path = strdup(filename);
	
	// Load chip packets
	for (i = lo; i < size; ) {
		
		// Is this a chip packet?
		uint8_t magic_chip[] = { 'C', 'H', 'I', 'P' };
		if (memcmp(magic_chip, &data[i], 4) != 0) {
			printf("Unexpected data in cartridge, expected 'CHIP'\n");
			printReadable(&data[i], 4);
			return false;
		}
		
		// Check the size of the rom contained in the chip packet
		unsigned int romSize = getHighLow(data[i+0x000E], data[i+0x000F]);
		
		printf("Got chip packet with ROM size: 0x%04x (%d) (typically 0x2000 or 0x4000)\n", romSize, romSize);
		
		printf("Chip type (0 - ROM, 1 - RAM, 2 - FLASH): %d\n", getHighLow(data[i+0x0008], data[i+0x0009]));
		
		printf("Bank number: 0x%04x \n", getHighLow(data[i+0x000A], data[i+0x000B]));
		printf("Load address: 0x%04x \n", getHighLow(data[i+0x000C], data[i+0x000D]));
		
		Cartridge::Chip * chipPacket = new Cartridge::Chip();
		chipPacket->loadAddress = getHighLow(data[i+0x000C], data[i+0x000D]);
		chipPacket->size = romSize;
		
		if (romSize > sizeof(chipPacket->rom)) {
			printf("Chip Rom too large\n");
			return false;
		}
		
		memcpy(chipPacket->rom, &data[i+0x0010], romSize);
		
		chip[numberOfChips++] = chipPacket;
		
		i += 0x0010 + romSize;
	}
	
	printf("CRT container imported successfully\n");
	
	return true;
}

//! The GAME line status
bool Cartridge::gameIsHigh()
{
	//	0019 - Cartridge port GAME line status
	//	0 - inactive
	//	1 - active
	return data[0x19] != 0;
}

//! The EXROM line status
bool Cartridge::exromIsHigh()
{
	//	0018 - Cartridge port EXROM line status
	//	0 - inactive
	//	1 - active
	return data[0x18] != 0;
}

int Cartridge::getVersion()
{
	return getHighLow(data[0x14], data[0x15]);
}

Cartridge::Type Cartridge::getType()
{
	return (Cartridge::Type) getHighLow(data[0x16], data[0x17]);
}

Cartridge::Chip *Cartridge::getChip(int index)
{
	if (index >= 0 && index < numberOfChips)
	{
		return chip[index];
	}
	return NULL;
}

int Cartridge::getNumberChips()
{
	return numberOfChips;
}

Cartridge::Chip::Chip()
{
	size = 0;
}

Cartridge::Chip::~Chip()
{
	
}



/*
 * Written by Dirk Hoffmann based on the original code by A. Carl Douglas.
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

/*
 For general info about C64 cartridges,
 see: http://www.c64-wiki.com/index.php/Cartridge
 
 For information about bank switching,
 see: http://www.c64-wiki.com/index.php/Bankswitching
 
 For details about the .CRT format,
 see: http://ist.uwaterloo.ca/~schepers/formats/CRT.TXT
 
 As well read the Commodore 64 Programmers Reference Guide pages 260-267.
 */

#ifndef _CARTRIDGE_H
#define _CARTRIDGE_H

#include "Container.h"

class Cartridge : public Container {

private:
    
    //! Raw data of CRT container file
    uint8_t *data;
    
    //! Number of chips contained in cartridge file
    unsigned int numberOfChips;
    
    //! Indicates where each chip section starts
    uint8_t *chips[64];

public:
    
    //! Constructor
    Cartridge();
    
    //! Destructor
    ~Cartridge();
    
    //! Free allocated memory
    void dealloc();
    
    //! Type of container
    Container::ContainerType getType() { return CRT_CONTAINER; }
    
    //! Type of container in plain text
    const char *getTypeAsString() { return "CRT"; }
    
    //! Returns true of filename points to a valid file of that type
    static bool isCRTFile(const char *filename);
    
    //! Check file type
    bool fileIsValid(const char *filename) { return Cartridge::isCRTFile(filename); }

    
    
    
public:
	
	//! forward declarion of nested chip class
	class Chip;
	
private:
	
	//! Physical name of archive 
	char *path;
	
	
    
	//! Size of data array
	//unsigned int size;
	
	//! Chip packets
	Chip *chip[64];
		
	//! The type of cartridge (manufacturer/model)
	unsigned int type;
	
	//! The version of the cartridge file
	unsigned int version;
	
	//! Virtual cartridge ROM
	/*! Only $8000-9FFF and $A000-$BFFF are valid cartridge locations. */
	uint8_t rom[65536];
	
	void switchBank(int bankNumber);
	
	//! Get the chip (bank)
	Chip *getChip(unsigned index);
	
public:
	
	//! There can be many chips in a cartridge
	class Chip 
	{
		/*
		 Bytes: $0000-0003 - Contained ROM signature "CHIP" (note there can be more
		 than one image in a .CRT file)
		 0004-0007 - Total packet length ($00002010,  ROM  image  size  and
		 header combined) (high/low format)
		 0008-0009 - Chip type
		 0 - ROM
		 1 - RAM, no ROM data
		 2 - Flash ROM
		 000A-000B - Bank number ($0000 - normal cartridge)
		 000C-000D - Starting load address (high/low format)
		 000E-000F - ROM image size in bytes  (high/low  format,  typically
		 $2000 or $4000)
		 0010-xxxx - ROM data
		 */
		
	public:
		
		enum ChipType {
			CHIP_ROM = 0,
			CHIP_RAM = 1,
			CHIP_FLASH = 2
		};
		
		Chip();
		
		~Chip();
        
		unsigned int loadAddress;
		
		int size;
		
		uint8_t rom[65536];
	};
	
	enum CartridgeType {
		Normal_Cartridge = 0,
		Action_Replay = 1,
		KCS_Power_Cartridge = 2,
		Final_Cartridge_III = 3,
		Simons_Basic = 4,
		Ocean_type_1 = 5,
		Expert_Cartridge = 6,
		Fun_Play_Power_Play = 7,
		Super_Games = 8,
		Atomic_Power = 9,
		Epyx_Fastload = 10,
		Westermann_Learning = 11,
		Rex_Utility = 12,
		Final_Cartridge_I = 13,
		Magic_Formel = 14,
		C64_Game_System_System_3 = 15,
		Warpspeed = 16,
		Dinamic = 17,
		Zaxxon_Super_Zaxxon = 18,
		Magic_Desk_Domark_HES_Australia = 19,
		Super_Snapshot_5 = 20,
		Comal = 21,
		Structure_Basic = 22,
		Ross = 23,
		Dela_EP64 = 24,
		Dela_EP7x8 = 25,
		Dela_EP256 = 26,
		Rex_EP256 = 27
	};
	
	
	//! Factory method
	static Cartridge *cartridgeFromFile(const char *filename);
	

	bool readFromBuffer(const void *buffer, unsigned length);
	
    //
    // Cartridge information
    //
    
    //! Return logical cartridge name
    char *getCartridgeName() { return (char *)&data[0x20]; }
    
    //! Return cartridge version number
    uint16_t getCartridgeVersion() { return LO_HI(data[0x15], data[0x14]); }

    //! Return cartridge type
    uint16_t getCartridgeType() { return LO_HI(data[0x17], data[0x16]); }

    //! Return game line
    bool getGameLine() { return data[0x0018]; };

    //! Return exrom line
    bool getExromLine() { return data[0x0019]; };
    
    //
    // Chip information
    //
    
    //! Return how many chips are contained in this cartridge
    uint8_t getNumberOfChips() { return numberOfChips; }
    
    //! Return start address of chip data
    uint8_t *getChipData(unsigned nr) { return chips[nr]+0x10; }
    
    //! Return size of chip (8 KB or 16 KB)
    uint16_t getChipSize(unsigned nr) { return LO_HI(chips[nr][0xF], chips[nr][0xE]); }
    
    //! Return type of chip
    uint16_t getChipType(unsigned nr) { return LO_HI(chips[nr][0x9], chips[nr][0x8]); }
    
    //! Return bank information
    uint16_t getChipBank(unsigned nr) { return LO_HI(chips[nr][0x000B], chips[nr][0x000A]); }
    
    //! Returns start of chip rom in address space
    uint16_t getChipAddr(unsigned nr) { return LO_HI(chips[nr][0x000D], chips[nr][0x000C]); }


    
	
	//! The GAME line status
	bool gameIsHigh();
	
	//! The EXROM line status
	bool exromIsHigh();
	
	//! The cartridge is Ultimax mode, $E000-FFFE
	bool isUltimax() { return !gameIsHigh() && exromIsHigh(); }
	
	//! The cartridge covers $8000-9FFF
	bool is8k() { return gameIsHigh() && !exromIsHigh(); }
	
	//! The cartridge covers $8000-BFFF
	bool is16k() { return !gameIsHigh() && !exromIsHigh(); }
	
	//! True iff the given address is covered by this cartridge
	bool isRomAddr(uint16_t addr);
	
	void poke(uint16_t addr, uint8_t value);
	
	uint8_t peek(uint16_t addr);
	
	//! Cartridge version
	unsigned int getVersion();
	
	//! Cartridge type
	// CartridgeType getCartridgeType();
};

#endif

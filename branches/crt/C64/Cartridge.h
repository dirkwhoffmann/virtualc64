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

/*
 
 See: http://www.c64-wiki.com/index.php/Cartridge
 
 "The cartridge system implemented in the C64 provides an easy way to 
 hook 8 or 16 kilobytes of ROM into the computer's address space: 
 This allows for applications and games up to 16 K, or BASIC expansions 
 up to 8 K in size and appearing to the CPU along with the built-in 
 BASIC ROM. In theory, such a cartridge need only contain the 
 ROM circuit without any extra support electronics."
 
 Also: http://www.c64-wiki.com/index.php/Bankswitching
 
 As well read the Commodore 64 Programmers Reference Guide pages 260-267.
 
 */
#ifndef _CARTRIDGE_H
#define _CARTRIDGE_H

#include "basic.h"
#include "VirtualComponent.h"

#define MAX_CHIPS 64

class Cartridge : public VirtualComponent {

//! crt file format
/*! 
	This information came from: http://ist.uwaterloo.ca/~schepers/formats/CRT.TXT
 
			 *** CRT - CaRTridge Images (from the CCS64 emulator)
			 *** Document revision: 1.13
			 *** Last updated: June 12, 2007
			 *** Compiler/Editor: Peter Schepers
			 *** Contributors/sources: Per Hakan Sundell,
			 Markus Brenner
			 Marco Van Den Heuvel
 
 Bytes:    $0000-000F - 16-byte cartridge signature  "C64  CARTRIDGE"  (padded with space characters)
			0010-0013 - File header length  ($00000040,  in  high/low  format,
						calculated from offset $0000). The default  (also  the
						minimum) value is $40.  Some  cartridges  exist  which
						show a value of $00000020 which is wrong.
			0014-0015 - Cartridge version (high/low, presently 01.00)
			0016-0017 - Cartridge hardware type ($0000, high/low)
						0 - Normal cartridge
						1 - Action Replay
						2 - KCS Power Cartridge
						3 - Final Cartridge III
						4 - Simons Basic
						5 - Ocean type 1*
						6 - Expert Cartridge
						7 - Fun Play, Power Play
						8 - Super Games
						9 - Atomic Power
						10 - Epyx Fastload
						11 - Westermann Learning
						12 - Rex Utility
						13 - Final Cartridge I
						14 - Magic Formel
						15 - C64 Game System, System 3
						16 - WarpSpeed
						17 - Dinamic**
						18 - Zaxxon, Super Zaxxon (SEGA)
						19 - Magic Desk, Domark, HES Australia
						20 - Super Snapshot 5
						21 - Comal-80
						22 - Structured Basic
						23 - Ross
						24 - Dela EP64
						25 - Dela EP7x8
						26 - Dela EP256
						27 - Rex EP256
			0018 - Cartridge port EXROM line status
						0 - inactive
						1 - active
			0019 - Cartridge port GAME line status
						0 - inactive
						1 - active
			001A-001F - Reserved for future use
			0020-003F - 32-byte cartridge  name  "CCSMON"  (uppercase,  padded
						with null characters)
			0040-xxxx - Cartridge contents (called CHIP PACKETS, as there  can
						be more than one  per  CRT  file).  See  below  for  a
						breakdown of the CHIP format.

	(*Note: Ocean type 1 includes Navy Seals, Robocop 2 & 3,  Shadow  of  the
	Beast, Toki, Terminator 2 and more)

	(**Note: Dinamic includes Narco Police and more)
 
 
	 Type     Size   Game   EXRom  Low Bank  High Bank
	          in K   Line   Line    (ROML)    (ROMH)
	 -------------------------------------------------
	 Normal    8k     hi     lo     $8000      ----
	 Normal    16k    lo     lo     $8000     $A000
	 Ultimax   8k     lo     hi     $E000      ----
 
 */
public:
	
	//! forward declarion of nested chip class
	class Chip;
	
private:
	
	//! Physical name of archive 
	char *path;
	
	//! Name of the CRT container file
	char name[256];
	
	//! File pointer
	/*! Stores an offset into the data array */
	int fp;
	
	//! Raw data of CRT container file
	uint8_t *data;
	
	//! Size of data array
	unsigned int size;
	
	//! Chip packets
	Chip *chip[MAX_CHIPS];
	
	//! The number of chips (banks) in this cartridge file
	unsigned int numberOfChips;
	
	//! The type of cartridge (manufacturer/model)
	unsigned int type;
	
	//! The version of the cartridge file
	unsigned int version;
	
	//! Virtual cartridge ROM
	/*! Only $8000-9FFF and $A000-$BFFF are valid cartridge locations. */
	uint8_t rom[65536];
	
	void switchBank(int bankNumber);
	
	//! Get the chip (bank)
	Chip *getChip(int index);
	
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
		
		enum Type {
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
	
	enum Type {
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
	
	//! Constructor
	Cartridge();
	
	//! Destructor
	~Cartridge();
	
	//! Load snapshot
	bool load(uint8_t **ptr);
	
	//! Save snapshot
	bool save(uint8_t **ptr);
	
	//! Reset
	void reset();
	
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
	Type getType();
	
	//! Check file type
	/*! Returns true, iff the specifies file is a valid cartridge file. */
	static bool fileIsValid(const char *filename);
	
	//! Load physical archive from disc
	bool loadFile(const char *filename);
	
};

#endif

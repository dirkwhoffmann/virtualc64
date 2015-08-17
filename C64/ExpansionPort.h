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


#ifndef _EXPANSIONPORT_H
#define _EXPANSIONPORT_H

#include "VirtualComponent.h"

class ExpansionPort : public VirtualComponent {
 
public:
    
    //! Cartridge types
    enum CartridgeType {
        CRT_NORMAL = 0,
        CRT_ACTION_REPLAY = 1,
        CRT_KCS_POWER_CARTRIDGE = 2,
        CRT_FINAL_CARTRIDGE_III = 3,
        CRT_SIMONS_BASIC = 4,
        CRT_OCEAN_TYPE_1 = 5,
        CRT_EXPORT_CARTRIDGE = 6,
        CRT_FUN_PLAY_POWER_PLAY = 7,
        CRT_SUPER_GAMES = 8,
        CRT_ATOMIC_POWER = 9,
        CRT_EPYX_FASTLOAD = 10,
        CRT_WESTERMANN_LEARNING = 11,
        CRT_REX_UTILITY = 12,
        CRT_FINAL_CARTRIDGE_I = 13,
        CRT_MAGIC_FORMEL = 14,
        CRT_C64_GAME_SYSTEM_SYSTEM_3 = 15,
        CRT_WARPSPEED = 16,
        CRT_DINAMIC = 17,
        CRT_ZAXXON_SUPER_ZAXXON = 18,
        CRT_MAGIC_DESK_DOMARK_HES_AUSTRALIA = 19,
        CRT_SUPER_SNAPSHOT_5 = 20,
        CRT_COMAL = 21,
        CRT_STRUCTURE_BASIC = 22,
        CRT_ROSS = 23,
        CRT_DELA_EP64 = 24,
        CRT_DELA_EP7x8 = 25,
        CRT_DELA_EP256 = 26,
        CRT_REX_EP256 = 27,
        CRT_NONE = 255
    };

private:
    
    //! Type of attached cartridge (CRT_NONE if no cartridge is plugged in)
    uint8_t type;
    
    //! Game line of cartridge (HIGH if no cartridge is attached)
    bool gameLine;

    //! Exrom line of cartridge (HIGH if no cartridge is attached)
    bool exromLine;
    
    //! A cartridge can contain up to 64 chips that contain ROM data
    uint8_t *chip[64];

    //! Load address of chip
    uint16_t chipStartAddress[64];

    //! Chip size in Number of bytes in chip
    uint16_t chipSize[64];

    //! Virtual cartridge ROM (32 kb starting at $8000)
    uint8_t rom[32768];
    
    //! Indicates if ROM is blended in (0x01) or or out (0x00)
    /*! Each array item represents a 4k block above $8000 */
    uint8_t blendedIn[16];
    
public:
    
    //! Constructor
    ExpansionPort(C64 *c64);
    
    //! Destructor
    ~ExpansionPort();
    
    //! Revert to initial state
    void reset(C64 *c64);
    
    //! Revert to initial state but keep cartridge data in place
    void softreset();

    //! Dump current configuration into message queue
    void ping();

    //! Size of internal state
    uint32_t stateSize();

    //! Load state
    void loadFromBuffer(uint8_t **buffer);
    
    //! Save state
    void saveToBuffer(uint8_t **buffer);
    
    //! Dump internal state to console
    void dumpState();	
    
    //! Returns true if cartride ROM is blended in at the specified location
    bool romIsBlendedIn(uint16_t addr) { return blendedIn[addr >> 12]; }
    
    //! Peek fallthrough
    uint8_t peek(uint16_t addr) { return rom[addr & 0x7FFF]; }
    
    //! Poke fallthrough
    void poke(uint16_t addr, uint8_t value);
    
    //! Getter and setter
    CartridgeType getCartridgeType() { return (CartridgeType)type; }
    
    bool getGameLine() { return gameLine; }
    void setGameLine(bool value);
    
    bool getExromLine() { return exromLine; }
    void setExromLine(bool value);
    
    //! Make chip contents visible
    void switchBank(unsigned nr);

    //! Returns true if a cartridge is attached to the expansion port
    inline bool getCartridgeAttached() { return type != CRT_NONE; }

    //! Attach a single cartridge chip
    void attachChip(unsigned nr, Cartridge *c);

    //! Attach a cartridge to the expansion port (cartridges can contain multiple chips)
    bool attachCartridge(Cartridge *c);

    //! Remove a cartridge from the expansion port
    void detachCartridge();

};
    
#endif

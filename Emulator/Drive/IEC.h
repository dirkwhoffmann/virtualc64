// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _IEC_H
#define _IEC_H

#include "C64Component.h"

class IEC : public C64Component {

public:
    
	// Current values of the IEC bus lines
	bool atnLine;
	bool clockLine;
	bool dataLine;
	 	
    /* Indicates if the bus lines variables need an undate, because the values
     * coming from the C64 side have changed.
     * DEPRECATED
     */
    bool isDirtyC64Side;

    /* Indicates if the bus lines variables need an undate, because the values
     * coming from the drive side have changed.
     * DEPRECATED
     */
    bool isDirtyDriveSide;

    // Bus driving values from drive 1
    bool device1Atn;
    bool device1Clock;
    bool device1Data;
    
    // Bus driving values from drive 2
    bool device2Atn;
    bool device2Clock;
    bool device2Data;
    
    // Bus driving values from the CIA
    bool ciaAtn;
    bool ciaClock;
    bool ciaData;
    
private:
    
	// Used to determine if the bus is idle or if data is transferred
	u32 busActivity;
	
    
    //
    // Initializing
    //
    
public:
        
	IEC(C64 &ref);
    const char *getDescriptionNew() override { return "IEC"; }
    
private:
    
    void _reset() override;

    
    //
    // Analyzing
    //
    
private:
    
    void _dump() override;
    void dumpTrace();

    
    //
    // Serializing
    //
    
private:
    
    template <class T>
    void applyToPersistentItems(T& worker)
    {
    }
    
    template <class T>
    void applyToResetItems(T& worker)
    {
        worker
        
        & atnLine
        & clockLine
        & dataLine
        & isDirtyC64Side
        & isDirtyDriveSide
        & device1Atn
        & device1Clock
        & device1Data
        & device2Atn
        & device2Clock
        & device2Data
        & ciaAtn
        & ciaClock
        & ciaData
        & busActivity;
    }
    
    size_t _size() override { COMPUTE_SNAPSHOT_SIZE }
    size_t _load(u8 *buffer) override { LOAD_SNAPSHOT_ITEMS }
    size_t _save(u8 *buffer) override { SAVE_SNAPSHOT_ITEMS }
    
    
    //
    // Accessing
    //
    
public:

    // Returns true if the IEC is transfering data
    bool isBusy() { return busActivity > 0; }
    
    // Requensts an update of the bus lines from the C64 side
    // DEPRECATED
    void setNeedsUpdateC64Side() { isDirtyC64Side = true; }

    // Requensts an update of the bus lines from the drive side
    // DEPRECATED
    void setNeedsUpdateDriveSide() { isDirtyDriveSide = true; }

    /* Updates all three bus lines. The new values are determined by VIA1
     * (drive side) and CIA2 (C64 side).
     */
    void updateIecLinesC64Side();
    void updateIecLinesDriveSide();

	/* Execution function for observing the bus activity. This method is
     * invoked periodically. It's purpose is to determines if data is
     * transmitted on the bus.
     */
	void execute();
    
private:
    
    void updateIecLines();
    
    /* Work horse for method updateIecLines. It returns true if at least one
     * line changed it's value.
     */
    bool _updateIecLines();
};
	
#endif

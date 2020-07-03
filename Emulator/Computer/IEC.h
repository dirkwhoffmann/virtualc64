// -----------------------------------------------------------------------------
// This file is part of VirtualC64
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v2
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#ifndef _IEC_INC
#define _IEC_INC

#include "VirtualComponent.h"

class IEC : public VirtualComponent {

public:
    
	//! @brief    Current value of the IEC bus atn line
	bool atnLine;

	//! @brief    Current value of the IEC bus clock line
	bool clockLine;

	//! @brief    Current value of the IEC bus data line
	bool dataLine;
	 	
    /*! @brief    Indicates if the bus lines variables need an undate,
     *            because the values coming from the C64 side have changed.
     *  @deprecated
     */
    bool isDirtyC64Side;

    /*! @brief    Indicates if the bus lines variables need an undate,
     *            because the values coming from the drive side have changed.
     *  @deprecated
     */
    bool isDirtyDriveSide;

    //! @brief    Bus driving values from drive 1 side
    bool device1Atn;
    bool device1Clock;
    bool device1Data;
    
    //! @brief    Bus driving values from drive 2 side
    bool device2Atn;
    bool device2Clock;
    bool device2Data;
    
    //! @brief    Bus driving values from CIA side
    bool ciaAtn;
    bool ciaClock;
    bool ciaData;
    
private:
    
	//! @brief    Used to determine if the bus is idle or if data is transferred
	uint32_t busActivity;
	
public:
    
	//! @brief    Constructor
	IEC();
	
	//! @brief    Destructor
	~IEC();
			
	//! @brief    Method from VirtualComponent
	void reset();

    //! @brief    Method from VirtualComponent
    void ping();
	
    //! @brief    Method from VirtualComponent
	void dump();
	
	//! @brief    Writes trace output to console.
	void dumpTrace();
	
    //! @brief    Returns true if the IEC currently transfers data.
    bool isBusy() { return busActivity > 0; }
    
    //! @brief    Requensts an update of the bus lines from the C64 side.
    //! @deprecated
    void setNeedsUpdateC64Side() { isDirtyC64Side = true; }

    //! @brief    Requensts an update of the bus lines from the drive side.
    //! @deprecated
    void setNeedsUpdateDriveSide() { isDirtyDriveSide = true; }

    //! @brief    Updates all three bus lines.
    /*! @details  The new values are determined by VIA1 (drive side) and
     *            CIA2 (C64 side).
     */
    void updateIecLinesC64Side();
    void updateIecLinesDriveSide();

	//! @brief    Execution function for observing the bus activity.
    /*! @details  This method is invoked periodically. It's only purpose is to
     *            determines if data is transmitted on the bus.
     */
	void execute();
    
private:
    
    void updateIecLines();
    
    //! @brief    Work horse for method updateIecLines
    /*! @details  Returns true if at least one line changed it's value.
     */
    bool _updateIecLines();
};
	
#endif

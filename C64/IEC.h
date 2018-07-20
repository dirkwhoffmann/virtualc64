/*!
 * @header      IEC.h
 * @author      Dirk W. Hoffmann, www.dirkwhoffmann.de
 * @copyright   2006 - 2018 Dirk W. Hoffmann
 */
/*              This program is free software; you can redistribute it and/or modify
 *              it under the terms of the GNU General Public License as published by
 *              the Free Software Foundation; either version 2 of the License, or
 *              (at your option) any later version.
 *
 *              This program is distributed in the hope that it will be useful,
 *              but WITHOUT ANY WARRANTY; without even the implied warranty of
 *              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *              GNU General Public License for more details.
 *
 *              You should have received a copy of the GNU General Public License
 *              along with this program; if not, write to the Free Software
 *              Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _IEC_INC
#define _IEC_INC

#include "VirtualComponent.h"

class IEC : public VirtualComponent {

public:
    
	//! @brief    Indicates if the floopy drive is connected to the IEC bus.
	bool driveIsConnected;
	
	//! @brief    Current value of the IEC bus atn line
	bool atnLine;

	//! @brief    Current value of the IEC bus clock line
	bool clockLine;

	//! @brief    Current value of the IEC bus data line
	bool dataLine;
	 	
    //! @brief    Indicates if the bus lines variables need an undate.
    bool isDirty;
    
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
	void dumpState();
	
	//! @brief    Writes trace output to console.
	void dumpTrace();
	
	//! @brief    Connects the virtual drive to the IEC bus.
	void connectDrive();
	
	//! @brief    Disconnects the virtual drive from the IEC bus.
	void disconnectDrive();
		
    //! @brief    Returns true if the IEC currently transfers data.
    bool isBusy() { return busActivity > 0; }
    
    //! @brief    Requensts an update of the bus lines.
    void setNeedsUpdate() { isDirty = true; }
    
    //! @brief    Updates all three bus lines.
    /*! @details  The new values are determined by VIA1 (drive side) and
     *            CIA2 (C64 side).
     */
	void updateIecLines();
    
	//! @brief    Executin function for observing the bus activity.
    /*! @details  This method is invoked periodically. It's only purpose is to
     *            determines if data is transmitted on the bus.
     */
	void execute();
    
private:
    
    //! @brief    Work horse for method updateIecLines
    /*! @details  Returns true if at least one line changed it's value.
     */
    bool _updateIecLines();
};
	
#endif

/*
 * Original implementation by Benjamin Klein
 * Rewritten and maintained by Dirk W. Hoffmann. All rights reserved.
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


#import <IOKit/hid/IOHIDLib.h>
#import <IOKit/hid/IOHIDManager.h>

#import "Joystick.h"

@class C64Proxy;
@class JoystickManagerProxy;


class USBJoystick
{

public:
    
    //! @brief    Indicates if this object represents a plugged in USB joystick device
    bool pluggedIn;
    
    //! @brief    Location ID of the represented USB joystick
    int locationID;
    
    //! @brief    Mapping to one of the two virtual joysticks of the emulator
    /*! @details  Initially, this pointer is NULL, meaning that the USB joystick has not yet been selected
     *            as input device. It can be selected as input device via bindJoystick(). In that case, it
     *            will point to one of the two static Joystick objects hold by the emulator.
     */
    JoystickProxy *joystick;

    USBJoystick()
    {
        pluggedIn = false;
        locationID = 0;
        joystick = NULL;
    }
    
    //! @brief Connects the USB device to port A of the emulator
    void bindJoystickToPortA(C64Proxy *c64) { if (pluggedIn) joystick = [c64 joystickA]; }

    //! @brief Connects the USB device to port A of the emulator
    void bindJoystickToPortB(C64Proxy *c64) { if (pluggedIn) joystick = [c64 joystickB]; }
    
    //! @brief Unconnect USB device
    void unbindJoystick() { joystick = nil; }
    
    void pullJoystick(GamePadDirection dir) { [joystick pullJoystick:dir]; }
    void releaseJoystick(GamePadDirection dir) { [joystick releaseJoystick:dir]; }
    void releaseXAxis() { [joystick releaseXAxis]; }
    void releaseYAxis() { [joystick releaseYAxis]; }
};

class IOHIDDeviceInfo
{
private:
    
    int _locationID;
    char *_name;
    
public:
    
    IOHIDDeviceInfo();
    IOHIDDeviceInfo(IOHIDDeviceRef device);
    IOHIDDeviceInfo(const IOHIDDeviceInfo &copy);
    ~IOHIDDeviceInfo();
    
    int GetLocationID() { return _locationID; }
    void setLocationID(int value) { _locationID = value; }
    char *GetName() { return _name; }
    void setName(char *value) { _name = value; }
};

class JoystickManager 
{
    
private:
    
    C64Proxy *_proxy;
    IOHIDManagerRef _manager;
    USBJoystick usbjoy[2]; // At most 2 USB joysticks can be plugged in

    
public:

    JoystickManager(C64Proxy *proxy);
	~JoystickManager();
		
	bool initialize();
	void dispose();

    //! @brief Returns true, if USBJoystick object 'nr' represents a plugged in USB joystick
    bool joystickIsPluggedIn(int nr);

    //! @brief Assigns a USBJoystick object to one of the two emulator Joystick objects
    //! @deprecated
    void bindJoystick(int nr, JoystickProxy *joy);

    //! @brief Assigns a USBJoystick object to port A of the emulator
    void bindJoystickToPortA(int nr) { assert (nr >= 1 && nr <= 2); usbjoy[nr - 1].bindJoystickToPortA(_proxy); }

    //! @brief Assigns a USBJoystick object to port B of the emulator
    void bindJoystickToPortB(int nr) { assert (nr >= 1 && nr <= 2); usbjoy[nr - 1].bindJoystickToPortB(_proxy); }

    //! @brief Remove any binding to port A of the emulator
    void unbindJoysticksFromPortA();

    //! @brief Remove any binding to port B of the emulator
    void unbindJoysticksFromPortB();

	static void MatchingCallback_static(void *inContext, IOReturn inResult, void *inSender, IOHIDDeviceRef inIOHIDDeviceRef);
	void MatchingCallback(void *inContext, IOReturn inResult, void *inSender, IOHIDDeviceRef inIOHIDDeviceRef);
		
	static void RemoveCallback_static(void *inContext, IOReturn inResult, void *inSender);
	void RemoveCallback(void *inContext, IOReturn inResult, void *inSenderf);
		
	static void InputValueCallback_static(void *inContext, IOReturn inResult, void *inSender, IOHIDValueRef inIOHIDValueRef);
	void InputValueCallback(void *inContext, IOReturn inResult, void *inSender, IOHIDValueRef inIOHIDValueRef);

    void IOHIDElement_SetDoubleProperty(IOHIDElementRef element, CFStringRef key, double value);
    
    bool addJoystickProxyWithLocationID(int locationID);
    USBJoystick *getJoystickProxyWithLocationID(int locationID);
    void removeJoystickProxyWithLocationID(int locationID);
    void listJoystickManagers();
};




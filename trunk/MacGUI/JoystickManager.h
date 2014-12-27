/*
 * (C) 2009 - 2015 Benjamin Klein, Dirk Hoffmann. All rights reserved.
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

// TO BE REMOVED
#import <set>

#import "Joystick.h"

@class C64Proxy;

// TO BE REMOVED
using std::set;

typedef struct {
    JoystickManager *manager;
    int locationID;
    IOHIDDeviceRef deviceRef;
} CallbackContext;

class JoystickProxy
{
    private:
    
    Joystick *_joystick;
    set<int> _pressedButtons; // Can't we get rid of the SDL library?

    public:

    JoystickProxy();
	// JoystickProxy(Joystick *joystick);
		
	void ChangeButton(int index, bool pressed);
	void ChangeAxisX(JoystickAxisState state) const;
	void ChangeAxisY(JoystickAxisState state) const;
	
	// Joystick *GetJoystick() const; // DEPRECATED

    // Bind virtual joystick of the emulator
    // 'Joystick' must be one of the two objects initialized by the emulator
    // It can be either the object representing port 1 or the object representing port 2
    void bindJoystick(Joystick *joy);
};

class JoystickManager 
{
	public:

    JoystickManager(C64Proxy *proxy);
	~JoystickManager();
		
	bool Initialize();
	void Dispose();

    bool joystickIsPluggedIn(int nr);

    // Bind virtual joystick of the emulator
    // 'Joystick' must be one of the two objects initialized by the emulator
    // It can be either the object representing port 1 or the object representing port 2
    void bindJoystick(int nr, Joystick *joy);
    
	static void MatchingCallback_static(void *inContext, IOReturn inResult, void *inSender, IOHIDDeviceRef inIOHIDDeviceRef);
	void MatchingCallback(void *inContext, IOReturn inResult, void *inSender, IOHIDDeviceRef inIOHIDDeviceRef);
		
	static void RemoveCallback_static(void *inContext, IOReturn inResult, void *inSender);
	void RemoveCallback(void *inContext, IOReturn inResult, void *inSenderf);
		
	static void InputValueCallback_static(void *inContext, IOReturn inResult, void *inSender, IOHIDValueRef inIOHIDValueRef);
	void InputValueCallback(void *inContext, IOReturn inResult, void *inSender, IOHIDValueRef inIOHIDValueRef);

	private:
    
    C64Proxy *_proxy;
	bool _initialized; // DO WE NEED THIS?
	IOHIDManagerRef _manager;
    int locationID1;
    int locationID2;
    JoystickProxy *proxy1;
    JoystickProxy *proxy2;
		
	static const int UsageToSearch[][2];
	static const unsigned MaxJoystickCount;

    void IOHIDElement_SetDoubleProperty(IOHIDElementRef element, CFStringRef key, double value);
    
    void addJoystickProxyWithLocationID(int locationID, JoystickProxy *proxy);
    JoystickProxy *getJoystickProxyWithLocationID(int locationID);
    void removeJoystickProxyWithLocationID(int locationID);
    void listJoystickManagers();
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
		    
    inline int GetLocationID() { return _locationID; }
    inline void setLocationID(int value) { _locationID = value; }
    inline const char *GetName() { return ( _name ? _name : "<NO NAME>" ); }
    inline void setName(char *value) { _name = value; }
};


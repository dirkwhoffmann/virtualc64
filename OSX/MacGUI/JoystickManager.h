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

class JoystickManagerProxy
{
    private:
    
    Joystick *_joystick;
    set<int> _pressedButtons; // Can't we get rid of the SDL library?

    public:

    JoystickManagerProxy();
		
	void ChangeButton(int index, bool pressed);
	void ChangeAxisX(JoystickAxisState state) const;
	void ChangeAxisY(JoystickAxisState state) const;
	
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
	IOHIDManagerRef _manager;
    int locationID1;
    int locationID2;
    JoystickManagerProxy *proxy1;
    JoystickManagerProxy *proxy2;
		
	static const int UsageToSearch[][2];
	static const unsigned MaxJoystickCount;

    void IOHIDElement_SetDoubleProperty(IOHIDElementRef element, CFStringRef key, double value);
    
    bool addJoystickProxyWithLocationID(int locationID, JoystickManagerProxy *proxy);
    JoystickManagerProxy *getJoystickProxyWithLocationID(int locationID);
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
		    
    int GetLocationID() { return _locationID; }
    void setLocationID(int value) { _locationID = value; }
    char *GetName() { return _name; } 
    void setName(char *value) { _name = value; }
};


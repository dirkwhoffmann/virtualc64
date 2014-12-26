/*
 * (C) 2009 Benjamin Klein. All rights reserved.
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
#import <map>
#import <set>
#import "Joystick.h"

@class C64Proxy;

using std::map;
using std::set;


typedef struct {
    JoystickManager *manager;
    int locationID;
    IOHIDDeviceRef deviceRef;
} CallbackContext;

class JoystickProxy
{
	public:
		JoystickProxy();
		JoystickProxy( Joystick *joystick );
		
		void ChangeButton( int index, bool pressed );
		void ChangeAxisX( JoystickAxisState state ) const;
		void ChangeAxisY( JoystickAxisState state ) const;
		
		Joystick *GetJoystick() const;
		
	private:
		Joystick *_joystick;
		set<int> _pressedButtons;
};

class JoystickManager 
{
	public:

    JoystickManager(C64Proxy *proxy);
	~JoystickManager();
		
	bool Initialize();
	void Dispose();
		
	static void MatchingCallback_static(void *inContext, IOReturn inResult, void *inSender, IOHIDDeviceRef inIOHIDDeviceRef);
	void MatchingCallback(void *inContext, IOReturn inResult, void *inSender, IOHIDDeviceRef inIOHIDDeviceRef);
		
	static void RemoveCallback_static(void *inContext, IOReturn inResult, void *inSender);
	void RemoveCallback(void *inContext, IOReturn inResult, void *inSenderf);
		
	static void InputValueCallback_static(void *inContext, IOReturn inResult, void *inSender, IOHIDValueRef inIOHIDValueRef);
	void InputValueCallback(void *inContext, IOReturn inResult, void *inSender, IOHIDValueRef inIOHIDValueRef);

	private:
    
    void IOHIDElement_SetDoubleProperty( IOHIDElementRef element, CFStringRef key, double value );
		
    void addJoystickProxyWithLocationID(int locationID, JoystickProxy *proxy);
    JoystickProxy *getJoystickProxyWithLocationID(int locationID);
    void removeJoystickProxyWithLocationID(int locationID);
    
	C64Proxy *_proxy;
		
	bool _initialized;
	IOHIDManagerRef _manager;
    int locationID1;
    int locationID2;
    JoystickProxy *proxy1;
    JoystickProxy *proxy2;
		
	static const int UsageToSearch[][ 2 ];
	static const unsigned MaxJoystickCount;
};

class IOHIDDeviceInfo
{
    private:

    int _locationID;
    char *_name;

    public:

    IOHIDDeviceInfo();
	IOHIDDeviceInfo( IOHIDDeviceRef device );
	IOHIDDeviceInfo( const IOHIDDeviceInfo &copy );
	~IOHIDDeviceInfo();
		    
    inline int GetLocationID() { return _locationID; }
    inline void setLocationID(int value) { _locationID = value; }
    inline const char *GetName() { return ( _name ? _name : "<NO NAME>" ); }
    inline void setName(char *value) { _name = value; }
};


/*
 * (C) 2006 Dirk W. Hoffmann. All rights reserved.
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

#import <Cocoa/Cocoa.h>
#import <QuickTime/QuickTime.h>

#define LIGHT_X_TAG 0
#define THETA_TAG   1
#define RADIUS_TAG  2

// Forward declaration
@class MyController;

// Mac keycodes of special keys
const uint16_t MAC_F1 = 122;
const uint16_t MAC_F2 = 120;
const uint16_t MAC_F3 = 99;
const uint16_t MAC_F4 = 118;
const uint16_t MAC_F5 = 96;
const uint16_t MAC_F6 = 97;
const uint16_t MAC_F7 = 98;
const uint16_t MAC_F8 = 100;
const uint16_t MAC_APO = 39;
const uint16_t MAC_DEL = 51;
const uint16_t MAC_RET = 36;
const uint16_t MAC_CL = 123;
const uint16_t MAC_CR = 124;
const uint16_t MAC_CU = 126;
const uint16_t MAC_CD = 125;
const uint16_t MAC_SPC = 49;
const uint16_t MAC_ESC = 53;
const uint16_t MAC_HAT = 10;

// Graphics constants
const int TEXTURE_WIDTH = 512;
const int TEXTURE_HEIGHT= 512;
const int TEXTURE_DEPTH = 4;

const int BG_TEXTURE_WIDTH = 1024;
const int BG_TEXTURE_HEIGHT= 512;
const int BG_TEXTURE_DEPTH = 4;

@interface MyOpenGLView : NSOpenGLView
{
	IBOutlet MyController *controller;
	IBOutlet C64Proxy* c64proxy; 
	
	C64 *c64; // DEPRECATED. GET RID OF THIS VARIABLE AND RENAME c64proxy to c64
	
    NSBitmapImageRep *theImage;
	NSOpenGLContext *glcontext;
	CVDisplayLinkRef displayLink;
	NSRecursiveLock *lock;

	float currentXAngle, targetXAngle, deltaXAngle;
	float currentYAngle, targetYAngle, deltaYAngle;
	float currentZAngle, targetZAngle, deltaZAngle;
	float currentEyeX, targetEyeX, deltaEyeX;
	float currentEyeY, targetEyeY, deltaEyeY;
	float currentEyeZ, targetEyeZ, deltaEyeZ;
	
    /*! Stores a fingerprint of each joystick emulation key.
     *  The user can choose from 2 maps */
    int joyKeycode[2][5];

    /*!  Stores a printabel character for each joystick emulation key.
     *   These values are only used in the properties dialog for pretty printing the keycodes */
    char joyChar[2][5];

	int frames;

	//! If false, OpenGL drawing is disabled (only used in performance debugging)
	bool enableOpenGL;

    //! If true, the OpenGL view covers the whole window area (used to hide the status bar)
    bool drawInEntireWindow;

    //! If false, 3D drawing is switched off (2D drawing is used in fullscreen mode, only)
	bool drawIn3D;

	//! If false, the C64 screen is not drawn (background texture or black screen will be visible)
	bool drawC64texture;

	//! If false, the background is not drawn 
	bool drawBackground;

	//! If false, only the front facing part of the texture cube is drawn
	bool drawEntireCube;
		
	//! Turns anti-aliasing on and off
	bool antiAliasing;
	
    //! Stores which keys are currently pressed
    /*! Array index is a Mac keycode and the stored value the pressed key on the c64 keyboard */
    unsigned int pressedKeys[256];
    
    // Textures
    uint8_t data[TEXTURE_WIDTH * TEXTURE_HEIGHT * TEXTURE_DEPTH];
    GLuint texture;   // C64 screen
    GLuint bgTexture; // Background

	// Texture cut-out (fist and last visible texture coordinates)
	float textureXStart;
	float textureXEnd;
	float textureYStart;
	float textureYEnd;
}

@property C64 *c64;
@property (readonly) int frames;
@property bool enableOpenGL;
@property bool drawIn3D;
@property bool drawC64texture;
@property bool drawBackground;
@property bool drawEntireCube;
@property bool antiAliasing;

- (void) cleanUp;

#pragma mark Animation

//! Returns true if view is currently drawing animation effects
- (bool) animates;

- (float)eyeX;
- (void)setEyeX:(float)newX;
- (float)eyeY;
- (void)setEyeY:(float)newY;
- (float)eyeZ;
- (void)setEyeZ:(float)newZ;

- (bool)drawInEntireWindow;
- (void)setDrawInEntireWindow:(bool)b;

//! Computes a fingerprint for the keycode/modifierFlags combination that uniquely identifies a key for joystick emulation
- (int)fingerprintForKey:(int)keycode withModifierFlags:(int)flags;

//! @brief Returns the keycode for a joystick emulation key
- (int)joyKeycode:(int)nr direction:(JoystickDirection)dir;

//! @brief Sets the keycode for a joystick emulation key
- (void)setJoyKeycode:(int)keycode keymap:(int)nr direction:(JoystickDirection)dir;

//! @brief Returns a joystick emulation key as printable character
- (char)joyChar:(int)nr direction:(JoystickDirection)dir;

//! @brief Sets the printable character for a joystick emulation key
- (void)setJoyChar:(char)c keymap:(int)nr direction:(JoystickDirection)dir;

//! Trigger animation effect. Zooms in from far away
- (void) zoom;

//! Trigger animation effect. Scroll in from below
- (void) scroll;

//! Trigger animation effect. Combination of zooming and rotating
- (void) fadeIn;

//! Trigger animation effect. Rotate cube in one direction
- (void) rotate;

//! Trigger animation effect. Rotate cube in other direction
- (void) rotateBack;


//! Compute geometry parameters for next animation cycle
- (void) updateAngles;

#pragma mark Drawing

-(CVReturn)getFrameForTime:(const CVTimeStamp *)timeStamp flagsOut:(CVOptionFlags *)flagsOut;
// - (void) drawC64texture:(bool)value;

#pragma mark Graphics conversion

- (NSImage *)screenshot;
- (NSImage *) flipImage: (NSImage *)image;
- (NSImage *) expandImage: (NSImage *)image toSize:(NSSize) size;
- (int) makeTexture:(NSImage *)img;

#pragma mark Joystick simulation

// Compares keycode with the joystick simulation keys and pulls joystick if appropriate
- (BOOL)pullJoystick:(int)nr withKeycode:(int)keycode device:(int)d;

// Compares keycode with the joystick simulation keys and releases joystick if appropriate
- (BOOL)releaseJoystick:(int)nr withKeycode:(int)keycode device:(int)d;

#pragma mark Joystick simulation

//! Get mac specific character and keycode and translate it to a virtual c64 key
/*! The returned value can be passed to the pressKey() method of the emulator */
- (int)translateKey:(char)key plainkey:(char)plainkey keycode:(short)keycode flags:(int)flags;

@end

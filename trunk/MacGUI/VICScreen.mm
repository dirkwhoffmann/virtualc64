/*
 * (C) 2006 - 2008 Dirk W. Hoffmann. All rights reserved.
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

#import "MyDocument.h"

const float TEX_LEFT   = 0.0 / ((float)TEXTURE_WIDTH);
const float TEX_RIGHT  = (VIC::SCREEN_WIDTH+2*VIC::BORDER_WIDTH) / ((float)TEXTURE_WIDTH);
const float TEX_TOP    = 37.0 / ((float)TEXTURE_HEIGHT);
const float TEX_BOTTOM = 263.0 / ((float)TEXTURE_HEIGHT);

const float BG_TEX_LEFT   = 0.0; 
const float BG_TEX_RIGHT  = 642.0 / BG_TEXTURE_WIDTH;
const float BG_TEX_TOP    = 0.0; 
const float BG_TEX_BOTTOM = 482.0 / BG_TEXTURE_HEIGHT;

@implementation VICScreen

// --------------------------------------------------------------------------------
//                                  Initializiation
// --------------------------------------------------------------------------------

- (id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat *)pixFmt
{
	if ((self = [super initWithFrame:frameRect pixelFormat:pixFmt]) != nil) {
		[self prepare];
	} else {
		NSLog(@"Can't initiaize OpenGL view\n");
	}
	return self;
}

- (id)initWithCoder:(NSCoder *)c
{
	self = [super initWithCoder:c];
	[self prepare];
	return self;
}


// --------------------------------------------------------------------------------
//                                 Getter and Setter
// --------------------------------------------------------------------------------

- (void) setC64:(C64 *)c
{
	c64 = c;
}

- (int) getFrames
{
	return frames;
}


// --------------------------------------------------------------------------------
//                               Animation effects
// --------------------------------------------------------------------------------

- (void)updateAngles
{
	// Set target angles
	
	// Update current angles
	if (currentXAngle != targetXAngle || 
		currentYAngle != targetYAngle || 
		currentZAngle != targetZAngle || 		
		currentDistance != targetDistance) { 

		if (fabs(currentXAngle - targetXAngle) < fabs(deltaX)) currentXAngle = targetXAngle;
		else                                                   currentXAngle += deltaX;

		if (fabs(currentYAngle - targetYAngle) < fabs(deltaY)) currentYAngle = targetYAngle;
		else                                                   currentYAngle += deltaY;

		if (fabs(currentZAngle - targetZAngle) < fabs(deltaZ)) currentZAngle = targetZAngle;
		else                                                   currentZAngle += deltaZ;

		if (fabs(currentDistance - targetDistance) < fabs(deltaDistance))
			currentDistance = targetDistance;
		else 
			currentDistance += deltaDistance;
	
		if (currentXAngle >= 360.0) currentXAngle -= 360.0;
		if (currentXAngle < 0.0) currentXAngle += 360.0;
		if (currentYAngle >= 360.0) currentYAngle -= 360.0;	
		if (currentYAngle < 0.0) currentYAngle += 360.0;
		if (currentZAngle >= 360.0) currentZAngle -= 360.0;	
		if (currentZAngle < 0.0) currentZAngle += 360.0;
	}
}

- (void)setReadyToDraw:(int)b
{
	readyToDraw = b;
}

- (int)isReadyToDraw
{
	return readyToDraw;
}

- (void)startAnimation
{	
	const int ANIM_CYCLES = 60; // 1 sec.

	targetXAngle = -1.0;
	targetYAngle = -1.0; // will rotate forever
	targetZAngle = -1.0;
	targetDistance = 0.4;
	deltaX = -1.0;
	deltaY = -0.8;
	deltaZ = -0.6;
	deltaDistance = (targetDistance - currentDistance) / ANIM_CYCLES;
}

- (void)stopAnimation
{
	const int ANIM_CYCLES = 120; 
	
	targetXAngle   = 0;
	targetYAngle   = 0;
	targetZAngle   = 0;
	targetDistance = 0; //0; //-0.5;
	deltaX         = (targetXAngle - currentXAngle) / ANIM_CYCLES;
	deltaY         = (targetYAngle - currentYAngle) / ANIM_CYCLES;	
	deltaZ         = (targetZAngle - currentZAngle) / ANIM_CYCLES;	
	deltaDistance  = (targetDistance - currentDistance) / ANIM_CYCLES;
}

- (void)toggleAnimation
{
	if (targetXAngle == 0 && targetYAngle == 0 && targetZAngle == 0)
		[self startAnimation];
	else
		[self stopAnimation];
}

- (void)zoom
{
	const int ANIM_CYCLES = 120; 
	
	currentDistance= 6;
	targetXAngle   = 0;
	targetYAngle   = 0;
	targetZAngle   = 0;
	targetDistance = 0; //-0.5;
	deltaX         = (targetXAngle - currentXAngle) / ANIM_CYCLES;
	deltaY         = (targetYAngle - currentYAngle) / ANIM_CYCLES;	
	deltaZ         = (targetZAngle - currentZAngle) / ANIM_CYCLES;	
	deltaDistance  = (targetDistance - currentDistance) / ANIM_CYCLES;
}

- (void)rotateBack
{
	const int ANIM_CYCLES = 60; 
		
	targetXAngle   = 0;
	targetZAngle   = 0;
	targetYAngle   += 90;
	NSLog(@"Rotating...\n");

	targetDistance = 0; // -0.5;
	deltaX         = (targetXAngle - currentXAngle) / ANIM_CYCLES;
	deltaY         = (targetYAngle - currentYAngle) / ANIM_CYCLES;	
	deltaZ         = (targetZAngle - currentZAngle) / ANIM_CYCLES;	
	deltaDistance  = (targetDistance - currentDistance) / ANIM_CYCLES;	

	if (targetYAngle >= 360) 
		targetYAngle -= 360;
}

- (void)rotate
{
	const int ANIM_CYCLES = 60; 
	
	targetXAngle   = 0;
	targetZAngle   = 0;
	targetYAngle   -= 90;
	NSLog(@"Rotating...\n");

	targetDistance = 0; // -0.5;
	deltaX         = (targetXAngle - currentXAngle) / ANIM_CYCLES;
	deltaY         = (targetYAngle - currentYAngle) / ANIM_CYCLES;	
	deltaZ         = (targetZAngle - currentZAngle) / ANIM_CYCLES;	
	deltaDistance  = (targetDistance - currentDistance) / ANIM_CYCLES;	

	if (targetYAngle < 0) 
		targetYAngle += 360;
}

/*
- (void)tiltOn
{
	const int ANIM_CYCLES = 30; 
	
	targetXAngle   = 0;
	targetZAngle   = 0;
	targetYAngle   = 90;
	targetDistance = 0; // -0.5;
	deltaX         = (targetXAngle - currentXAngle) / ANIM_CYCLES;
	deltaY         = (targetYAngle - currentYAngle) / ANIM_CYCLES;	
	deltaZ         = (targetZAngle - currentZAngle) / ANIM_CYCLES;	
	deltaDistance  = (targetDistance - currentDistance) / ANIM_CYCLES;	
}

- (void)tiltOff
{
	const int ANIM_CYCLES = 30; 
	
	targetXAngle   = 0;
	targetZAngle   = 0;
	targetYAngle   = 0;
	targetDistance = 0; // -0.5;
	deltaX         = (targetXAngle - currentXAngle) / ANIM_CYCLES;
	deltaY         = (targetYAngle - currentYAngle) / ANIM_CYCLES;	
	deltaZ         = (targetZAngle - currentZAngle) / ANIM_CYCLES;	
	deltaDistance  = (targetDistance - currentDistance) / ANIM_CYCLES;	
}
*/


// --------------------------------------------------------------------------------
//                                    Graphics
// --------------------------------------------------------------------------------

- (NSImage *) flipImage: (NSImage *)image
{
	assert(image != nil);
	
	NSSize size = [image size];
	NSImage *newImage = [[NSImage alloc] initWithSize:size];
	bool flipped = [image isFlipped];
	[image setFlipped:!flipped];
	// [newImage setFlipped:YES];
	[newImage lockFocus];
	[image drawInRect:NSMakeRect(0,0,size.width,size.height) 
			 fromRect:NSMakeRect(0,0,[image size].width, [image size].height) 
			operation:NSCompositeSourceOver fraction:1.0];
	[newImage unlockFocus];
	[image setFlipped:!flipped];	
	return newImage;
}

- (NSImage *) expandImage: (NSImage *)image toSize:(NSSize) size
{
	assert(image != nil);

	NSImage *newImage = [[NSImage alloc] initWithSize:size];
	[newImage setFlipped:YES];
	[newImage lockFocus];
	[image drawInRect:NSMakeRect(0,0,size.width,size.height) 
			fromRect:NSMakeRect(0,0,[image size].width, [image size].height) 
			operation:NSCompositeSourceOver fraction:1.0];
	[newImage unlockFocus];
	return newImage;
}

- (NSImage *) extendImage: (NSImage *)image toSize:(NSSize) size
{
	assert(image != nil);
	
	NSImage *newImage = [[NSImage alloc] initWithSize:size];
	[newImage setFlipped:YES];
	[newImage lockFocus];
	[image drawInRect:NSMakeRect(0,0,[image size].width, [image size].height) 
			 fromRect:NSMakeRect(0,0,[image size].width, [image size].height) 
			operation:NSCompositeSourceOver fraction:1.0];
	[newImage unlockFocus];
	return newImage;
}

- (int) makeTexture:(NSImage *)image
{
	assert(image != nil);

	unsigned int tid = 0;
	int texformat = GL_RGB;
	
	NSBitmapImageRep *imgBitmap = [[NSBitmapImageRep alloc] initWithData:[image TIFFRepresentation]];
	[imgBitmap retain];
	
	if ([imgBitmap samplesPerPixel] == 4)
		texformat = GL_RGBA;
	else if ([imgBitmap samplesPerPixel] == 3)
		texformat = GL_RGB;
	else if ([imgBitmap samplesPerPixel] == 2)
		texformat = GL_LUMINANCE_ALPHA;
	else if ([imgBitmap samplesPerPixel] == 1)
		texformat = GL_LUMINANCE;
	
	glGenTextures(1, (GLuint *)&tid);
	glBindTexture(GL_TEXTURE_2D, tid);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, 
					  [imgBitmap pixelsWide], 
					  [imgBitmap pixelsHigh], 
					  texformat, 
					  GL_UNSIGNED_BYTE, 
					  [imgBitmap bitmapData]);
	[imgBitmap release];
	return tid;
}

- (void)prepare
{

	// Keyboard
	for (int i = 0; i < 256; i++) {
		kb[i] = 0xff;
	}
	emulateJoystick1 = emulateJoystick2 = false;

	// Lookup table:
	//       
	//              0                    1                     2                     3                   4                       5                     6                   7
	// 
	// 0           DEL                 RETURN                CUR LR                  F7                 F1                      F3                    F5                 CUR UD
	// 1            3                    W                     A                     4                   Z                       S                     E                 LSHIFT
	// 2            5                    R                     D                     6                   C                       F                     T                   X
	// 3            7                    Y                     G                     8                   B                       H                     U                   V
	// 4            9                    I                     J                     0                   M                       K                     O                   N
	// 5            +                    P                     L                     -                   .                       :                     @                   ,
	// 6           LIRA                  *                     ;                   HOME               RSHIFT                     =                     ^                   /
	// 7            1                   <-                    CTRL                   2                 SPACE                     C=                    Q                  STOP		
	
	kb[MAC_DEL] = 0x0000; kb[MAC_RET] = 0x0001; kb[MAC_CR]  = 0x0002; kb[MAC_F7]  = 0x0003;  kb[MAC_F1]  = 0x0004;   kb[MAC_F3]  = 0x0005;  kb[MAC_F5]  = 0x0006; kb[MAC_CD]  = 0x0007;	
	kb[MAC_3]   = 0x0100; kb[MAC_W]   = 0x0101; kb[MAC_A]   = 0x0102; kb[MAC_4]   = 0x0103;  kb[MAC_Z]   = 0x0104;   kb[MAC_S]   = 0x0105;  kb[MAC_E]   = 0x0106; 
	kb[MAC_5]   = 0x0200; kb[MAC_R]   = 0x0201; kb[MAC_D]   = 0x0202; kb[MAC_6]   = 0x0203;  kb[MAC_C]   = 0x0204;   kb[MAC_F]   = 0x0205;  kb[MAC_T]   = 0x0206; kb[MAC_X]   = 0x0207;	
	kb[MAC_7]   = 0x0300; kb[MAC_Y]   = 0x0301; kb[MAC_G]   = 0x0302; kb[MAC_8]   = 0x0303;  kb[MAC_B]   = 0x0304;   kb[MAC_H]   = 0x0305;  kb[MAC_U]   = 0x0306; kb[MAC_V]   = 0x0307;	
	kb[MAC_9]   = 0x0400; kb[MAC_I]   = 0x0401; kb[MAC_J]   = 0x0402; kb[MAC_0]   = 0x0403;  kb[MAC_M]   = 0x0404;   kb[MAC_K]   = 0x0405;  kb[MAC_O]   = 0x0406; kb[MAC_N]   = 0x0407;	
	kb[MAC_PLS] = 0x0500; kb[MAC_P]   = 0x0501; kb[MAC_L]   = 0x0502; kb[MAC_MNS] = 0x0503;  kb[MAC_DOT] = 0x0504;                                                kb[MAC_COM] = 0x0507;	
	kb[MAC_APO] = 0x0604;                                                                                                                                         kb[MAC_HAT] = 0x0606; 	
	kb[MAC_1]   = 0x0700; kb[MAC_HAT] = 0x0701;                       kb[MAC_2]   = 0x0703;  kb[MAC_SPC] = 0x0704;                          kb[MAC_Q]   = 0x0706;	

	// Graphics
	frames = 0;
	readyToDraw = NO;
	screenBuffer = NULL;
	
	// Open GL initialization
	glcontext = [self openGLContext];
	[glcontext makeCurrentContext];
	
	// Configure the view
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST); 

	// Disable everything we don't need
	glDisable(GL_DITHER);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_FOG);

	glEnable(GL_TEXTURE_2D);
	
	// Create screen texture
	glGenTextures(1, (GLuint *)&texture);
	glBindTexture(GL_TEXTURE_2D, texture);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	
	// Create background texture
	NSImage *bgImage = [NSImage imageNamed:@"c64"];
	NSImage *bgImageResized = [self extendImage:bgImage toSize:NSMakeSize(BG_TEXTURE_WIDTH,BG_TEXTURE_HEIGHT)];
	bgTexture = [self makeTexture:bgImageResized];
	
	// Sync screen refresh to the monitor refresh rate
	const GLint VBL = 1;
	CGLSetParameter(CGLGetCurrentContext(),  kCGLCPSwapInterval, &VBL);
	
	// Drag and Drop
	[self registerForDraggedTypes:
		[NSArray arrayWithObject:NSFilenamesPboardType]];

	// Start animation
	currentDistance= 6;
}

- (void) reshape
{
	NSRect rect = [self bounds]; 
	// NSLog(@"%@ reshape (%d %d)", self, (int)rect.size.width, (int)rect.size.height);

	[glcontext makeCurrentContext];
	glViewport(0, 0, (int)rect.size.width, (int)rect.size.height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, rect.size.width/rect.size.height, 0.2, 7);
}


- (void) awakeFromNib
{
}

//- (void)windowControllerDidLoadNib:(NSWindowController *) aController
//{
//}

- (void) updateTexture:(int *)buffer
{
	assert(buffer != NULL);
	screenBuffer = buffer;
	
	// Can't we update the texture data here? 	
}

- (void)drawRect:(NSRect)r
{	
	bool animation = false;
	
	// Check if we need to transform geometry etc.
    if (currentXAngle != targetXAngle || currentYAngle != targetYAngle || currentZAngle != targetZAngle || currentDistance != targetDistance) {
		animation = true;
	}
		
	frames++;

	[glcontext makeCurrentContext];

	// Clear screen and depth buffer
	uint32_t col = 0xffffffff;
	glClearColor((float)EXTRACT_RED(col)/0xff, (float)EXTRACT_GREEN(col)/0xff, (float)EXTRACT_BLUE(col)/0xff, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Select screen texture
	glBindTexture(GL_TEXTURE_2D, texture);			

	// Set location
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set the view point
	float eyeX, eyeY, eyeZ;
	
	eyeX = 0; 
	eyeY = 0; 
	eyeZ = 1.33; 
			  
	//	NSLog(@"eyeX = %f eyeY = %f eyeZ = %f\n", eyeX, eyeY, eyeZ);
	gluLookAt(eyeX, eyeY, eyeZ, 0, 0, 0, 0, 1, 0);
	
	if (animation) {
		// Draw background image if visible
		float depth = -5.0f;
		float scale = 9.2f;
		glBindTexture(GL_TEXTURE_2D, bgTexture); 
		glBegin(GL_QUADS);		
		glTexCoord2f(BG_TEX_RIGHT, BG_TEX_TOP);
		glVertex3f(scale*0.64f, scale*-0.4f, depth); // Top right
		glTexCoord2f(BG_TEX_LEFT, BG_TEX_TOP);
		glVertex3f(scale*-0.64f, scale*-0.4f, depth); // Top left
		glTexCoord2f(BG_TEX_LEFT, BG_TEX_BOTTOM);
		glVertex3f(scale*-0.64f, scale*0.4f, depth); // Bottom left
		glTexCoord2f(BG_TEX_RIGHT, BG_TEX_BOTTOM);
		glVertex3f(scale*0.64f, scale*0.4f, depth); // Bottom right
		glEnd();		
	}

	// Rotate (if necessary)
	if (animation) {
		// Rotate around Z axis
		glRotatef(currentZAngle,0.0f,0.0f,1.0f);
	
		// Rotate around Y axis
		glRotatef(currentYAngle,0.0f,1.0f,0.0f);

		// Rotate around X axis
		glRotatef(currentXAngle,1.0f,0.0f,0.0f);
	}

	// Zoom in or zoom out
	glTranslatef(0, 0, -currentDistance);
	
	glBindTexture(GL_TEXTURE_2D, texture);			
	if (screenBuffer != NULL) {
		if (c64->isHalted()) {
			// If emulation is halted, we brighten up the display by adding some fog...
			GLfloat fogColor[4]= {1.0f, 1.0f, 1.0f, 1.0f};
			glFogfv(GL_FOG_COLOR, fogColor);
			glFogi(GL_FOG_MODE, GL_EXP);
			glFogf(GL_FOG_DENSITY, 1.0f);				
			glHint(GL_FOG_HINT, GL_DONT_CARE);	
			glFogf(GL_FOG_START, 0.0f);	
			glFogf(GL_FOG_END, 2.0f);
			glEnable(GL_FOG);	
		} else {
			glDisable(GL_FOG);
		}		
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, VIC::TOTAL_SCREEN_WIDTH, TEXTURE_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer);
	}

	glBindTexture(GL_TEXTURE_2D, texture);		
	glBegin(GL_QUADS);			
		// FRONT
		glTexCoord2f(TEX_RIGHT, TEX_TOP);
		glVertex3f( 0.64f, 0.4f, 0.64f);		// Top Right Of The Quad (Front)
		glTexCoord2f(TEX_LEFT, TEX_TOP);
		glVertex3f(-0.64f, 0.4f, 0.64f);		// Top Left Of The Quad (Front)
		glTexCoord2f(TEX_LEFT, TEX_BOTTOM);
		glVertex3f(-0.64f,-0.4f, 0.64f);		// Bottom Left Of The Quad (Front)
		glTexCoord2f(TEX_RIGHT, TEX_BOTTOM);
		glVertex3f( 0.64f,-0.4f, 0.64f);		// Bottom Right Of The Quad (Front)
	
		if (animation) {
		// TOP
		glColor3f(1.0f,1.0f,1.0f);				// Set The Color
		glTexCoord2f(TEX_RIGHT, TEX_TOP);
		glVertex3f( 0.64f, 0.4f,-0.64f);		// Top Right (TOP)
		glTexCoord2f(TEX_LEFT, TEX_TOP);
		glVertex3f(-0.64f, 0.4f,-0.64f);		// Top Left (TOP)
		glTexCoord2f(TEX_LEFT, TEX_BOTTOM);
		glVertex3f(-0.64f, 0.4f, 0.64f);		// Bottom Left (TOP)
		glTexCoord2f(TEX_RIGHT, TEX_BOTTOM);
		glVertex3f( 0.64f, 0.4f, 0.64f);		// Bottom Right (TOP)

		// BOTTOM
		glColor3f(1.0f,1.0f,1.0f);			    // Set The Color
		glTexCoord2f(TEX_RIGHT, TEX_TOP);
		glVertex3f( 0.64f,-0.4f, 0.64f);		// Top Right (BOTTOM)
		glTexCoord2f(TEX_LEFT, TEX_TOP);
		glVertex3f(-0.64f,-0.4f, 0.64f);		// Top Left (BOTTOM)
		glTexCoord2f(TEX_LEFT, TEX_BOTTOM);
		glVertex3f(-0.64f,-0.4f,-0.64f);		// Bottom Left (BOTTOM)
		glTexCoord2f(TEX_RIGHT, TEX_BOTTOM);
		glVertex3f( 0.64f,-0.4f,-0.64f);	    // Bottom right (BOTTOM)

		// BACK
		glTexCoord2f(TEX_LEFT, TEX_BOTTOM);
		glVertex3f( 0.64f,-0.4f,-0.64f);		// Bottom Left Of The Quad (Back)
		glTexCoord2f(TEX_RIGHT, TEX_BOTTOM);
		glVertex3f(-0.64f,-0.4f,-0.64f);		// Bottom Right Of The Quad (Back)
		glTexCoord2f(TEX_RIGHT, TEX_TOP);
		glVertex3f(-0.64f, 0.4f,-0.64f);		// Top Right Of The Quad (Back)
		glTexCoord2f(TEX_LEFT, TEX_TOP);
		glVertex3f( 0.64f, 0.4f,-0.64f);		// Top Left Of The Quad (Back)
							  
		// LEFT
		glTexCoord2f(TEX_RIGHT, TEX_TOP);
		glVertex3f(-0.64f, 0.4f, 0.64f);		// Top Right Of The Quad (Left)
		glTexCoord2f(TEX_LEFT, TEX_TOP);
		glVertex3f(-0.64f, 0.4f,-0.64f);		// Top Left Of The Quad (Left)
		glTexCoord2f(TEX_LEFT, TEX_BOTTOM);
		glVertex3f(-0.64f,-0.4f,-0.64f);		// Bottom Left Of The Quad (Left)
		glTexCoord2f(TEX_RIGHT, TEX_BOTTOM);
		glVertex3f(-0.64f,-0.4f, 0.64f);		// Bottom Right Of The Quad (Left)

		// RIGHT
		glTexCoord2f(TEX_RIGHT, TEX_TOP);
		glVertex3f( 0.64f, 0.4f,-0.64f);		// Top Right Of The Quad (Right)
		glTexCoord2f(TEX_LEFT, TEX_TOP);
		glVertex3f( 0.64f, 0.4f, 0.64f);		// Top Left Of The Quad (Right)
		glTexCoord2f(TEX_LEFT, TEX_BOTTOM);
		glVertex3f( 0.64f,-0.4f, 0.64f);		// Bottom Left Of The Quad (Right)
		glTexCoord2f(TEX_RIGHT, TEX_BOTTOM);
		glVertex3f( 0.64f,-0.4f,-0.64f);		// Bottom Right Of The Quad (Right)
		}
		
		glEnd();		

		// Flush screen
	    glFinish();
		[glcontext flushBuffer];
}

- (NSImage *)screenshot
{
	int height=(int)NSHeight([self visibleRect]);
	int width=(int)NSWidth([self visibleRect]);
	
	NSBitmapImageRep *imageRep;
	NSImage *image;
	
	imageRep=[[[NSBitmapImageRep alloc] initWithBitmapDataPlanes:NULL
													  pixelsWide:width
													  pixelsHigh:height
												   bitsPerSample:8
												 samplesPerPixel:4
														hasAlpha:YES
														isPlanar:NO
												  colorSpaceName:NSCalibratedRGBColorSpace
													 bytesPerRow:width*4
													bitsPerPixel:0] autorelease];
	[[self openGLContext] makeCurrentContext];
	glReadPixels(0,0,width,height,GL_RGBA,GL_UNSIGNED_BYTE,[imageRep bitmapData]);
	image=[[[NSImage alloc] initWithSize:NSMakeSize(width,height)] autorelease];
	[image addRepresentation:imageRep];

	NSImage *screenshot = [self flipImage:image];
	return screenshot;
}

- (void) setFullscreenMode:(bool)b
{
	if (b) {
		NSLog(@"Entering fullscreen mode");
		[self enterFullScreenMode:[NSScreen mainScreen] withOptions:nil];
	} else {
		NSLog(@"Exiting fullscreen mode");
		[self exitFullScreenModeWithOptions:nil];
	}
}

- (void) toggleFullscreenMode
{
	[self setFullscreenMode:![self isInFullScreenMode]];
}

// --------------------------------------------------------------------------------
//                                  Keyboard events 
// --------------------------------------------------------------------------------

- (BOOL)acceptsFirstResponder
{
    NSLog(@"%@ accepting first responder...", self);
	return YES;
}

- (BOOL)resignFirstResponder
{
    NSLog(@"%@ resigning first responder...", self);
	return YES;
}

- (BOOL)becomeFirstResonder 
{
    NSLog(@"%@ is becoming first responder...", self);
	return YES;
}

- (void)keyDown:(NSEvent *)event
{
	char c                 = [[event characters] UTF8String][0];
	unsigned short keycode = [event keyCode];
	unsigned int   flags   = [event modifierFlags];

    if (c64->getDeviceOfPort(0) == IPD_KEYBOARD || c64->getDeviceOfPort(1) == IPD_KEYBOARD) {
		switch (keycode) {
			case MAC_CU: 
				// debug("Up\n");
				if (c64->getDeviceOfPort(0) == IPD_KEYBOARD) c64->cia1->clearJoystickBits(1, 1);
				if (c64->getDeviceOfPort(1) == IPD_KEYBOARD) c64->cia1->clearJoystickBits(2, 1);
				return;
			case MAC_CD:
				// debug("Down\n");
				if (c64->getDeviceOfPort(0) == IPD_KEYBOARD) c64->cia1->clearJoystickBits(1, 2);
				if (c64->getDeviceOfPort(1) == IPD_KEYBOARD) c64->cia1->clearJoystickBits(2, 2);
				return;
			case MAC_CL:
				// debug("Left\n");
				if (c64->getDeviceOfPort(0) == IPD_KEYBOARD) c64->cia1->clearJoystickBits(1, 4);
				if (c64->getDeviceOfPort(1) == IPD_KEYBOARD) c64->cia1->clearJoystickBits(2, 4);
				return;
			case MAC_CR:
				// debug("Right\n");
				if (c64->getDeviceOfPort(0) == IPD_KEYBOARD) c64->cia1->clearJoystickBits(1, 8);
				if (c64->getDeviceOfPort(1) == IPD_KEYBOARD) c64->cia1->clearJoystickBits(2, 8);
				return;
			case MAC_SPC:
				// debug("Fire\n");
				if (c64->getDeviceOfPort(0) == IPD_KEYBOARD) c64->cia1->clearJoystickBits(1, 16);
				if (c64->getDeviceOfPort(1) == IPD_KEYBOARD) c64->cia1->clearJoystickBits(2, 16);
				return;
		}
	}
	
	// The following characters need special handling
	switch (c) {
		case ';':	c64->keyboard->pressKey(6,2); return;
		case ':':	c64->keyboard->pressKey(5,5); return;
		case '=':	c64->keyboard->pressKey(6,5); return;
		case '*':	c64->keyboard->pressKey(6,1); return;
		case '@':	c64->keyboard->pressKey(5,6); return;
		case '<':	c64->keyboard->pressShiftKey(); c64->keyboard->pressKey(5,7); return;
		case '>':	c64->keyboard->pressShiftKey(); c64->keyboard->pressKey(5,4); return;
		case '?':	c64->keyboard->pressShiftKey(); c64->keyboard->pressKey(6,7); return;
		case '#':	c64->keyboard->pressShiftKey(); c64->keyboard->pressKey(1,0); return;		
	}
	
	switch (keycode) {			
		case MAC_F2: c64->keyboard->pressShiftKey(); c64->keyboard->pressKey(0,4); return;
		case MAC_F4: c64->keyboard->pressShiftKey(); c64->keyboard->pressKey(0,5); return;
		case MAC_F6: c64->keyboard->pressShiftKey(); c64->keyboard->pressKey(0,6); return;
		case MAC_F8: c64->keyboard->pressShiftKey(); c64->keyboard->pressKey(0,3); return;
		case MAC_CU: c64->keyboard->pressShiftKey(); c64->keyboard->pressKey(0,7); return;
		case MAC_CL: c64->keyboard->pressShiftKey(); c64->keyboard->pressKey(0,2); return;
	}
	
	// For all other characters, we use a direct key mapping
	if (flags & NSShiftKeyMask)
		c64->keyboard->pressShiftKey();
	if (flags & NSAlternateKeyMask)
		c64->keyboard->pressCommodoreKey();
	c64->keyboard->pressKey(kb[keycode] >> 8, kb[keycode] & 0xFF);
}

- (void)keyUp:(NSEvent *)event
{
	unsigned short keycode = [event keyCode];

#if 0
	// TO BE REMOVED: FOR DEBUGGING ONLY
	if (keycode == MAC_F7) {
		c64->floppy->dumpTrack();
	}
	if (keycode == MAC_F8) {
		c64->floppy->dumpFullTrack();
	}
#endif	

    if (c64->getDeviceOfPort(0) == IPD_KEYBOARD || c64->getDeviceOfPort(1) == IPD_KEYBOARD) {
		switch (keycode) {
			case MAC_CU: 
				// debug("Releasing Up\n");
				if (c64->getDeviceOfPort(0) == IPD_KEYBOARD) c64->cia1->setJoystickBits(1, 1);
				if (c64->getDeviceOfPort(1) == IPD_KEYBOARD) c64->cia1->setJoystickBits(2, 1);
				return;
			case MAC_CD:
				// debug("Releasing Down\n");
				if (c64->getDeviceOfPort(0) == IPD_KEYBOARD) c64->cia1->setJoystickBits(1, 2);
				if (c64->getDeviceOfPort(1) == IPD_KEYBOARD) c64->cia1->setJoystickBits(2, 2);
				return;
			case MAC_CL:
				// debug("Releasing Left\n");
				if (c64->getDeviceOfPort(0) == IPD_KEYBOARD) c64->cia1->setJoystickBits(1, 4);
				if (c64->getDeviceOfPort(1) == IPD_KEYBOARD) c64->cia1->setJoystickBits(2, 4);
				return;
			case MAC_CR:
				// debug("Releasing Right\n");
				if (c64->getDeviceOfPort(0) == IPD_KEYBOARD) c64->cia1->setJoystickBits(1, 8);
				if (c64->getDeviceOfPort(1) == IPD_KEYBOARD) c64->cia1->setJoystickBits(2, 8);
				return;
			case MAC_SPC:
				// debug("Releasing Fire\n");
				if (c64->getDeviceOfPort(0) == IPD_KEYBOARD) c64->cia1->setJoystickBits(1, 16);
				if (c64->getDeviceOfPort(1) == IPD_KEYBOARD) c64->cia1->setJoystickBits(2, 16);
				return;
		}
	}
	// release all keys
	//keyboard->releaseAll();
		
	// We always relase the special keys
	// That's the easiest way to cope with race conditions due to fast typing
	// (Problems can occur, if a new kit is hit before the previous is released)
	c64->keyboard->releaseShiftKey();
	c64->keyboard->releaseCommodoreKey();
	c64->keyboard->releaseKey(6,2);
	c64->keyboard->releaseKey(5,5);
	c64->keyboard->releaseKey(6,5);
	c64->keyboard->releaseKey(6,1);
	c64->keyboard->releaseKey(5,6);
	c64->keyboard->releaseKey(5,7);
	c64->keyboard->releaseKey(5,4);	
	c64->keyboard->releaseKey(6,7);
	c64->keyboard->releaseKey(1,0);
	
	switch (keycode) {			
		case MAC_F2: c64->keyboard->releaseShiftKey(); c64->keyboard->releaseKey(0,4); return;
		case MAC_F4: c64->keyboard->releaseShiftKey(); c64->keyboard->releaseKey(0,5); return;
		case MAC_F6: c64->keyboard->releaseShiftKey(); c64->keyboard->releaseKey(0,6); return;
		case MAC_F8: c64->keyboard->releaseShiftKey(); c64->keyboard->releaseKey(0,3); return;
		case MAC_CU: c64->keyboard->releaseShiftKey(); c64->keyboard->releaseKey(0,7); return;
		case MAC_CL: c64->keyboard->releaseShiftKey(); c64->keyboard->releaseKey(0,2); return;
	}
	
	if (keycode == MAC_ESC) {
		// The escape key will exit fullscreen mode
		[self setFullscreenMode:false];
	}
	
	// For all other characters, we use a direct key mapping
	c64->keyboard->releaseKey(kb[keycode] >> 8, kb[keycode] & 0xFF);
}

// --------------------------------------------------------------------------------
//                           Joystick (via HID Interface)
// --------------------------------------------------------------------------------

// TODO

// --------------------------------------------------------------------------------
//                                  Drag and Drop 
// --------------------------------------------------------------------------------

- (unsigned int)draggingEntered:(id <NSDraggingInfo>)sender
{
	if ([sender draggingSource] != self) {
		NSPasteboard *pb = [sender draggingPasteboard];
		NSString *type = [pb availableTypeFromArray:
			[NSArray arrayWithObject:NSFilenamesPboardType]];
		if (type != nil) {
//			[self tiltOn]; 
			[self setNeedsDisplay:YES];
			return NSDragOperationCopy;
		}
	}
	return NSDragOperationNone;
}

- (void)draggingExited:(id <NSDraggingInfo>)sender
{
//	[self tiltOff];
	[self setNeedsDisplay:YES];
}

- (BOOL)prepareForDragOperation:(id <NSDraggingInfo>)sender
{
	return YES;
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender
{
    NSPasteboard *paste = [sender draggingPasteboard];
	//gets the dragging-specific pasteboard from the sender
    NSArray *types = [NSArray arrayWithObjects:NSFilenamesPboardType, nil];
	//a list of types that we can accept
    NSString *desiredType = [paste availableTypeFromArray:types];
    NSData *carriedData = [paste dataForType:desiredType];
	Archive *archive = NULL;
	
    if (nil == carriedData) {
        //the operation failed for some reason
        NSRunAlertPanel(@"Paste Error", @"Sorry, the paste operation failed", nil, nil, nil);
        return NO;
    }

	//the pasteboard was able to give us some meaningful data
    if ([desiredType isEqualToString:NSFilenamesPboardType]) {
        //we have a list of file names in an NSData object
        NSArray *fileArray = [paste propertyListForType:@"NSFilenamesPboardType"];
		//be caseful since this method returns id.  
		//We just happen to know that it will be an array.
        NSString *path = [fileArray objectAtIndex:0];
			
		// Invoke targetselector on drag finish
		NSLog(@"Got filename %@", path);
			
		// Try to load file

		// Is it a saved image?
#if 0
		if (c64->loadSnapshot([path UTF8String])) {
			[self rotate];
			// PROBLEM: The associated file of the document does not change
			// We might want to call something like [doc readFromFileWrapper:ofType:error:]
			NSLog(@"Snapshot loaded");
			return YES;
		}
#endif
		
		// Is it a ROM file?
		if ([myDoc loadRom:path]) {
			NSLog(@"ROM loaded");
			// Try to run...
			// Update romDialog...
			return YES;
		}
		
		// Is it raw VC 1541 data?
		if (VC1541::isG64Image([path UTF8String])) {
			c64->floppy->readG64Image([path UTF8String]);
			// [self rotate];
			NSLog(@"G64 loaded");
			return YES;	
		}
		
		// Is it an archive?
		if (T64Archive::fileIsValid([path UTF8String])) {
			archive = new T64Archive();
		} else if (D64Archive::fileIsValid([path UTF8String])) {
			archive = new D64Archive();
		} else if (PRGArchive::fileIsValid([path UTF8String])) {
			archive = new PRGArchive();		
		} else if (P00Archive::fileIsValid([path UTF8String])) {
			archive = new P00Archive();		
		} 
		
		// Load archive if applicable
		if (archive != NULL) {
			if (!archive->loadFile([path UTF8String])) {
				return NO;
			}
		
			// Display mount dialog
			[myDoc setArchive:archive]; 
			[myDoc showMountDialog];	
			return YES;
		}
	}
	
	return NO;
}

- (void)concludeDragOperation:(id <NSDraggingInfo>)sender
{
	[self setNeedsDisplay:YES];
}

@end

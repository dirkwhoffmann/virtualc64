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

#import "C64GUI.h"

// Background texture
const float BG_TEX_LEFT   = 0.0; 
const float BG_TEX_RIGHT  = 1.0; // 642.0 / BG_TEXTURE_WIDTH;
const float BG_TEX_TOP    = 0.0; 
const float BG_TEX_BOTTOM = 1.0; // 482.0 / BG_TEXTURE_HEIGHT;

static CVReturn MyRenderCallback(CVDisplayLinkRef displayLink, 
								 const CVTimeStamp *inNow, 
								 const CVTimeStamp *inOutputTime, 
								 CVOptionFlags flagsIn, 
								 CVOptionFlags *flagsOut, 
                                 void *displayLinkContext)
{
	return [(MyOpenGLView *)displayLinkContext getFrameForTime:inOutputTime flagsOut:flagsOut];
}

void checkForOpenGLErrors()
{
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		NSLog(@"OpenGL PANIC: %s", gluErrorString(error));
		exit(0);
	}
}

@implementation MyOpenGLView

@synthesize c64, frames, enableOpenGL, drawC64texture, drawBackground, drawEntireCube, antiAliasing;

// --------------------------------------------------------------------------------
//                                  Initializiation
// --------------------------------------------------------------------------------

- (id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat *)pixFmt
{
	NSLog(@"MyOpenGLView::initWithFrame");
	
	if ((self = [super initWithFrame:frameRect pixelFormat:pixFmt]) == nil) {
		NSLog(@"ERROR: Can't initiaize VICscreen\n");
	}
	return self;
}

- (id)initWithCoder:(NSCoder *)c
{
	NSLog(@"MyOpenGLView::initWithCoder");

	if ((self = [super initWithCoder:c]) == nil) {
		NSLog(@"ERROR: Can't initiaize VICscreen\n");
	}
		return self;
}

-(void)awakeFromNib
{
	NSLog(@"MyOpenGLView::awakeFromNib");
	
	// Lock around draw method
	lock = [NSRecursiveLock new];
		
	// Initial scene position
	targetXAngle = targetYAngle = targetZAngle = 0;
	deltaXAngle = deltaYAngle = deltaZAngle = 0;
	currentEyeX = currentEyeY = currentEyeZ = 0;
	deltaEyeX = deltaEyeY = deltaEyeZ = 0;
	
	drawC64texture = false;
	drawBackground = true;
	drawEntireCube = false;
	antiAliasing = true;
	
	// Core video
	displayLink = nil;
	
	// Graphics
	frames = 0;
	enableOpenGL = true;  

	// Keyboard
	for (int i = 0; i < 256; i++) {
		kb[i] = 0xff;
	}
	emulateJoystick1 = emulateJoystick2 = false;
	
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
						  kb[MAC_DIV] = 0x0607;
	kb[MAC_1]   = 0x0700; kb[MAC_HAT] = 0x0701;                       kb[MAC_2]   = 0x0703;  kb[MAC_SPC] = 0x0704;                          kb[MAC_Q]   = 0x0706;
	
	
	// Drag and Drop
	[self registerForDraggedTypes:
	 [NSArray arrayWithObjects:NSFilenamesPboardType,NSFileContentsPboardType,nil]];
}

- (void) dealloc 
{
	[self cleanUp];
    [super dealloc];
}

-(void)cleanUp
{    
	NSLog(@"Deallocating OpenGL ressources");
	
	// release display link
    if (displayLink) {
    	CVDisplayLinkStop(displayLink);
        CVDisplayLinkRelease(displayLink);
        displayLink = NULL;
    }
    
	// release current frame
    //if (currentFrame) {
    //	CVOpenGLTextureRelease(currentFrame);
    //    currentFrame = NULL;
    //}
	    
    if (lock) {
    	[lock release];
        lock = nil;
    }
}

#if 0
- (void) drawC64texture:(bool)value
{ 
	drawC64texture = value; 
}
#endif

- (void)prepareOpenGL
{
	NSLog(@"MyOpenGLView::prepareOpenGL");	
	
	// Set up context
	glcontext = [self openGLContext];
	assert(glcontext != NULL);
	[glcontext makeCurrentContext];
	
	// Configure the view
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST); 
	checkForOpenGLErrors();

	// Disable everything we don't need
	glDisable(GL_DITHER);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_FOG);
	
	// Create C64 monitor texture
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, (GLuint *)&texture);
	checkForOpenGLErrors();
	assert(texture > 0);
	
	glBindTexture(GL_TEXTURE_2D, texture);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	checkForOpenGLErrors();
	
	// Create background texture
	// NSImage *bgImage = [NSImage imageNamed:@"c64"];
	// NSImage *bgImageResized = [self extendImage:bgImage toSize:NSMakeSize(BG_TEXTURE_WIDTH,BG_TEXTURE_HEIGHT)];
	// NSImage *bgImage = [NSImage imageNamed:@"C64G_P8"];
	//NSImage *bgImage = [NSImage imageNamed:@"c64orig"];
	
	// NSString *file = @"/Library/Desktop Pictures/Nature/Aurora.jpg";
	// NSImage *bgImage = [[NSImage alloc] initWithContentsOfFile:file];
	NSURL *url = [[NSWorkspace sharedWorkspace] desktopImageURLForScreen:[NSScreen mainScreen]];
	NSImage *bgImage = [[NSImage alloc] initWithContentsOfURL:url];
	NSImage *bgImageResized = [self expandImage:bgImage toSize:NSMakeSize(BG_TEXTURE_WIDTH,BG_TEXTURE_HEIGHT)];
	bgTexture = [self makeTexture:bgImageResized];
	checkForOpenGLErrors();
	
	// Turn on synchronization
	const GLint VBL = 1;
	[[self openGLContext] setValues:&VBL forParameter:NSOpenGLCPSwapInterval];
	checkForOpenGLErrors();
	
    // Create display link for the main display
    CVDisplayLinkCreateWithCGDisplay(kCGDirectMainDisplay, &displayLink);
	checkForOpenGLErrors();
	
    if (displayLink != NULL) {
		CVReturn success;
    	// Set the current display of a display link
    	if ((success = CVDisplayLinkSetCurrentCGDisplay(displayLink, kCGDirectMainDisplay)) != 0) {
			NSLog(@"CVDisplayLinkSetCurrentCGDisplay failed with return code %d", success);
			CVDisplayLinkRelease(displayLink);
			exit(0);
		}
        
        // Set the renderer output callback function
    	if ((success = CVDisplayLinkSetOutputCallback(displayLink, &MyRenderCallback, self)) != 0) {
			NSLog(@"CVDisplayLinkSetOutputCallback failed with return code %d", success);
  	        CVDisplayLinkRelease(displayLink);
			exit(0);
		}
        
        // Activates display link
    	if ((success = CVDisplayLinkStart(displayLink)) != 0) {
			NSLog(@"CVDisplayLinkStart failed with return code %d", success);
		        CVDisplayLinkRelease(displayLink);
			exit(0);				
		}	

		NSLog(@"Display link activated successfully");
	}
}

// --------------------------------------------------------------------------------
//                               Animation effects
// --------------------------------------------------------------------------------

- (bool)animates
{	
	return (currentXAngle != targetXAngle || currentYAngle != targetYAngle || currentZAngle != targetZAngle || 		
			currentEyeX != targetEyeX || currentEyeY != targetEyeY || currentEyeZ != targetEyeZ);
}

- (void)setEyeX:(float)newX
{
	currentEyeX = targetEyeX = newX;
}

- (void)setEyeY:(float)newY
{
	currentEyeY = targetEyeY = newY;
}

- (void)setEyeZ:(float)newZ
{
	currentEyeZ = targetEyeZ = newZ;
}

- (void)updateAngles
{	
	if ([self animates]) {
	
		if (fabs(currentXAngle - targetXAngle) < fabs(deltaXAngle)) currentXAngle = targetXAngle;
		else														currentXAngle += deltaXAngle;

		if (fabs(currentYAngle - targetYAngle) < fabs(deltaYAngle)) currentYAngle = targetYAngle;
		else														currentYAngle += deltaYAngle;

		if (fabs(currentZAngle - targetZAngle) < fabs(deltaZAngle)) currentZAngle = targetZAngle;
		else														currentZAngle += deltaZAngle;

		if (fabs(currentEyeX - targetEyeX) < fabs(deltaEyeX))       currentEyeX   = targetEyeX;
		else														currentEyeX   += deltaEyeX;

		if (fabs(currentEyeY - targetEyeY) < fabs(deltaEyeY))       currentEyeY   = targetEyeY;
		else														currentEyeY   += deltaEyeY;

		if (fabs(currentEyeZ - targetEyeZ) < fabs(deltaEyeZ))       currentEyeZ   = targetEyeZ;
		else														currentEyeZ   += deltaEyeZ;
		
		if (currentXAngle >= 360.0) currentXAngle -= 360.0;
		if (currentXAngle < 0.0) currentXAngle += 360.0;
		if (currentYAngle >= 360.0) currentYAngle -= 360.0;	
		if (currentYAngle < 0.0) currentYAngle += 360.0;
		if (currentZAngle >= 360.0) currentZAngle -= 360.0;	
		if (currentZAngle < 0.0) currentZAngle += 360.0;

	} else {
		drawEntireCube = false;
	}
}

- (void)computeAnimationDeltaSteps:(int)animationCycles
{
	deltaXAngle = (targetXAngle - currentXAngle) / animationCycles;
	deltaYAngle = (targetYAngle - currentYAngle) / animationCycles;	
	deltaZAngle = (targetZAngle - currentZAngle) / animationCycles;	
	deltaEyeX = (targetEyeX - currentEyeX) / animationCycles;
	deltaEyeY = (targetEyeY - currentEyeY) / animationCycles;
	deltaEyeZ = (targetEyeZ - currentEyeZ) / animationCycles;
}

- (void)zoom
{
	NSLog(@"Zooming in...\n");

	currentEyeZ     = 6;
	targetXAngle    = 0;
	targetYAngle    = 0;
	targetZAngle    = 0;
	
	[self computeAnimationDeltaSteps:120 /* 2 sec */];
}

- (void)rotateBack
{
	NSLog(@"Rotating back...\n");

	targetXAngle   = 0;
	targetZAngle   = 0;
	targetYAngle   += 90;
	
	[self computeAnimationDeltaSteps:60 /* 1 sec */];

	if (targetYAngle >= 360) 
		targetYAngle -= 360;

	drawEntireCube = true;
}

- (void)rotate
{
	NSLog(@"Rotating...\n");

	targetXAngle   = 0;
	targetZAngle   = 0;
	targetYAngle   -= 90;
	drawEntireCube = true;
	
	[self computeAnimationDeltaSteps:60 /* 1 sec */];

	if (targetYAngle < 0) 
		targetYAngle += 360;
}

- (void)scroll
{
	NSLog(@"Scrolling...\n");
	
	currentEyeY    = 0.9;
	targetXAngle   = 0;
	targetYAngle   = 0;
	targetZAngle   = 0;

	[self computeAnimationDeltaSteps:120];		
}

- (void)fadeIn
{
	NSLog(@"Fading in...\n");
	
	
	currentXAngle  = -90;
	currentEyeZ    = 5.0;
	
	currentEyeY    = 2.5;
	targetXAngle   = 0;
	targetYAngle   = 0;
	targetZAngle   = 0;
	
	[self computeAnimationDeltaSteps:120];	
}

#if 0
- (void)moveToX:(float)newX
{
	targetEyeX = newX;
	[self computeAnimationDeltaSteps:60 /* 1 sec */];
}

- (void)moveToY:(float)newY 
{
	targetEyeY = newY;
	[self computeAnimationDeltaSteps:60 /* 1 sec */];
}

- (void)moveToZ:(float)newZ 
{
	targetEyeZ = newZ;
	[self computeAnimationDeltaSteps:60 /* 1 sec */];
}
#endif


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

- (void)update
{
    [lock lock];
	[super update];
    [lock unlock];
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
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

- (CVReturn)getFrameForTime:(const CVTimeStamp*)timeStamp flagsOut:(CVOptionFlags*)flagsOut
{
	NSAutoreleasePool *pool = [NSAutoreleasePool new];
	
	// Update angles for screen animation
	[self updateAngles];
	
	// Draw scene
   	[self drawRect:NSZeroRect];
    
    [pool release];
	
	return kCVReturnSuccess;
}

- (void)drawRect:(NSRect)r
{	
	if (!c64 || !enableOpenGL) 
		return;
	
	[lock lock]; 
			
	frames++;

	// Determine screen geometry (differs between NTSC and PAL)
	textureXStart = (float)c64->vic->getFirstVisiblePixel() / (float)TEXTURE_WIDTH;
	textureXEnd = (float)c64->vic->getLastVisiblePixel() / (float)TEXTURE_WIDTH;
	textureYStart = (float)c64->vic->getFirstVisibleLine() / (float)TEXTURE_HEIGHT;
	textureYEnd = (float)c64->vic->getLastVisibleLine() / (float)TEXTURE_HEIGHT;
	dimX = 0.64;
	dimY = dimX * (float)c64->vic->getTotalScreenHeight() / (float)c64->vic->getTotalScreenWidth() / c64->vic->getPixelAspectRatio();

	
	[glcontext makeCurrentContext];

	// Changing the shadeModel has no effect
	// glShadeModel(antiAliasing ? GL_SMOOTH : GL_FLAT);

	// Clear screen and depth buffer
	//glClearColor((float)EXTRACT_RED(col)/0xff, (float)EXTRACT_GREEN(col)/0xff, (float)EXTRACT_BLUE(col)/0xff, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Update screen texture
	glBindTexture(GL_TEXTURE_2D, texture);			
	if (c64) {
		void *buf = c64->vic->screenBuffer(); 
		assert(buf != NULL);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, c64->vic->getTotalScreenWidth(), TEXTURE_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, buf);
		checkForOpenGLErrors();
	}
	
	// Set location
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
				  
	// Set viewpoint
	gluLookAt(0, 0, 1.33, 0, 0, 0, 0, 1, 0);
		
	bool animation = [self animates];

	// bool drawBackground = animation || !drawC64texture;
	// bool drawBackground = !drawC64texture;
	if (drawBackground) {
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

	// Zoom in or zoom out
	glTranslatef(-currentEyeX, -currentEyeY, -0.065306 - currentEyeZ);

	if (animation) {
		
		// Rotate around Z axis
		glRotatef(currentZAngle,0.0f,0.0f,1.0f);
		
		// Rotate around Y axis
		glRotatef(currentYAngle,0.0f,1.0f,0.0f);
		
		// Rotate around X axis
		glRotatef(currentXAngle,1.0f,0.0f,0.0f);

	}
	
	if (drawC64texture) {	

		// If emulation is halted, we brighten up the display by adding some fog...
		if (c64->isHalted()) {
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

		// NSLog(@"drawingC64Texture");
		glBindTexture(GL_TEXTURE_2D, texture);		
		glBegin(GL_QUADS);			
		
		// FRONT
		glTexCoord2f(textureXEnd, textureYStart);
		glVertex3f( dimX, dimY, dimX);		// Top Right Of The Quad (Front)
		glTexCoord2f(textureXStart, textureYStart);
		glVertex3f(-dimX, dimY, dimX);		// Top Left Of The Quad (Front)
		glTexCoord2f(textureXStart, textureYEnd);
		glVertex3f(-dimX,-dimY, dimX);		// Bottom Left Of The Quad (Front)
		glTexCoord2f(textureXEnd, textureYEnd);
		glVertex3f( dimX,-dimY, dimX);		// Bottom Right Of The Quad (Front)
	
		if (drawEntireCube) {
			
			// TOP
			glColor3f(1.0f,1.0f,1.0f);				// Set The Color
			glTexCoord2f(textureXEnd, textureYStart);
			glVertex3f( dimX, dimY,-dimX);		// Top Right (TOP)
			glTexCoord2f(textureXStart, textureYStart);
			glVertex3f(-dimX, dimY,-dimX);		// Top Left (TOP)
			glTexCoord2f(textureXStart, textureYEnd);
			glVertex3f(-dimX, dimY, dimX);		// Bottom Left (TOP)
			glTexCoord2f(textureXEnd, textureYEnd);
			glVertex3f( dimX, dimY, dimX);		// Bottom Right (TOP)
				
			// BOTTOM
			glColor3f(1.0f,1.0f,1.0f);			    // Set The Color
			glTexCoord2f(textureXEnd, textureYStart);
			glVertex3f( dimX,-dimY, dimX);		// Top Right (BOTTOM)
			glTexCoord2f(textureXStart, textureYStart);
			glVertex3f(-dimX,-dimY, dimX);		// Top Left (BOTTOM)
			glTexCoord2f(textureXStart, textureYEnd);
			glVertex3f(-dimX,-dimY,-dimX);		// Bottom Left (BOTTOM)
			glTexCoord2f(textureXEnd, textureYEnd);
			glVertex3f( dimX,-dimY,-dimX);	    // Bottom right (BOTTOM)

			// BACK
			glTexCoord2f(textureXStart, textureYEnd);
			glVertex3f( dimX,-dimY,-dimX);		// Bottom Left Of The Quad (Back)
			glTexCoord2f(textureXEnd, textureYEnd);
			glVertex3f(-dimX,-dimY,-dimX);		// Bottom Right Of The Quad (Back)
			glTexCoord2f(textureXEnd, textureYStart);
			glVertex3f(-dimX, dimY,-dimX);		// Top Right Of The Quad (Back)
			glTexCoord2f(textureXStart, textureYStart);
			glVertex3f( dimX, dimY,-dimX);		// Top Left Of The Quad (Back)
						  
			// LEFT
			glTexCoord2f(textureXEnd, textureYStart);
			glVertex3f(-dimX, dimY, dimX);		// Top Right Of The Quad (Left)
			glTexCoord2f(textureXStart, textureYStart);
			glVertex3f(-dimX, dimY,-dimX);		// Top Left Of The Quad (Left)
			glTexCoord2f(textureXStart, textureYEnd);
			glVertex3f(-dimX,-dimY,-dimX);		// Bottom Left Of The Quad (Left)
			glTexCoord2f(textureXEnd, textureYEnd);
			glVertex3f(-dimX,-dimY, dimX);		// Bottom Right Of The Quad (Left)
				
			// RIGHT
			glTexCoord2f(textureXEnd, textureYStart);
			glVertex3f( dimX, dimY,-dimX);		// Top Right Of The Quad (Right)
			glTexCoord2f(textureXStart, textureYStart);
			glVertex3f( dimX, dimY, dimX);		// Top Left Of The Quad (Right)
			glTexCoord2f(textureXStart, textureYEnd);
			glVertex3f( dimX,-dimY, dimX);		// Bottom Left Of The Quad (Right)
			glTexCoord2f(textureXEnd, textureYEnd);
			glVertex3f( dimX,-dimY,-dimX);		// Bottom Right Of The Quad (Right)
		}
			
		glEnd();		
	}

	// Flush screen
	glFinish();
	[glcontext flushBuffer];

    [lock unlock];
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

		drawBackground = false; 
		oldCurrentEyeX = currentEyeX; // always draw centered
		[self setEyeX:0.0];
		
		[self enterFullScreenMode:[NSScreen mainScreen] withOptions:nil];
	} else {
		NSLog(@"Exiting fullscreen mode");
		
		drawBackground = true;
		[self setEyeX:oldCurrentEyeX];
		
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
    // NSLog(@"%@ accepting first responder...", self);
	return YES;
}

- (BOOL)resignFirstResponder
{
    // NSLog(@"%@ resigning first responder...", self);
	return YES;
}

- (BOOL)becomeFirstResonder 
{
    // NSLog(@"%@ is becoming first responder...", self);
	return YES;
}

- (void)keyDown:(NSEvent *)event
{
	unsigned int   c       = [[event characters] UTF8String][0];
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

	if ((c >= 32 && c <= 64) || (c >= 97 && c <= 122)) {
		c64->keyboard->pressKey(c);
		return;
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
	unsigned int c         = [[event characters] UTF8String][0];
	
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
	// (Problems can occur, if a new key is hit before the previous is released)
	c64->keyboard->releaseShiftKey();
	c64->keyboard->releaseCommodoreKey();
	
	if ((c >= 32 && c <= 64) || (c >= 97 && c <= 122)) {
		c64->keyboard->releaseKey(c);
		return;
	}
		
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
//                                  Drag and Drop 
// --------------------------------------------------------------------------------

- (unsigned int)draggingEntered:(id <NSDraggingInfo>)sender
{
	NSLog(@"draggingEntered");
	if ([sender draggingSource] == self)
		return NSDragOperationNone;

	NSPasteboard *pb = [sender draggingPasteboard];
	NSString *besttype = [pb availableTypeFromArray:[NSArray arrayWithObjects:NSFilenamesPboardType,NSFileContentsPboardType,nil]];

	if (besttype == NSFilenamesPboardType) {
		NSLog(@"Dragged in filename");
		return NSDragOperationCopy;
	}

	if (besttype == NSPasteboardTypeString) {
		NSLog(@"Dragged in string");
		return NSDragOperationCopy;
	}

	if (besttype == NSFileContentsPboardType) {
		NSLog(@"Dragged in file contents");
		return NSDragOperationCopy;
	}
	
	return NSDragOperationNone;
}

- (void)draggingExited:(id <NSDraggingInfo>)sender
{
}

- (BOOL)prepareForDragOperation:(id <NSDraggingInfo>)sender
{
	return YES;
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender
{
	NSLog(@"performDragOperation");
	//NSString *type;
    NSPasteboard *pb = [sender draggingPasteboard];
	
	if ([[pb types] containsObject:NSFileContentsPboardType]) {
		
        NSFileWrapper *fileWrapper = [pb readFileWrapper];
		NSData *fileData = [fileWrapper regularFileContents];
		V64Snapshot *snapshot = [V64Snapshot snapshotFromBuffer:[fileData bytes] length:[fileData length]];
		[[controller c64] loadFromSnapshot:snapshot];
		return YES;
	}

	if ([[pb types] containsObject:NSFilenamesPboardType]) {
		
        NSString *path = [[pb propertyListForType:@"NSFilenamesPboardType"] objectAtIndex:0];			
		NSLog(@"Got filename %@", path);
		
		// Is it an image file?
		V64Snapshot *snapshot = [V64Snapshot snapshotFromFile:path];
		if (snapshot) {
			[[controller c64] loadFromSnapshot:snapshot];
			NSLog(@"Image file loaded");
			return YES;
		}
			
		// Is it a ROM file?
		if ([[controller document] loadRom:path]) {
			NSLog(@"ROM loaded");
			return YES;
		}
		
		// Is it raw VC 1541 data?
		if (VC1541::isG64Image([path UTF8String])) {
			c64->floppy->readG64Image([path UTF8String]);
			// [self rotate];
			NSLog(@"G64 loaded");
			return YES;	
		}
		
		// Is it a cartridge?
		if ([[controller document] setCartridgeWithName:path]) {
			[controller mountCartridge];
			return YES;
		}
		
		// Is it an archive?
		if ([[controller document] setArchiveWithName:path]) {
			[controller showMountDialog];			
			return YES;
		}					
	}
	
	return NO;	
}

- (void)concludeDragOperation:(id <NSDraggingInfo>)sender
{
}

@end

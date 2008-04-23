#import <Cocoa/Cocoa.h>
#import "C64Proxy.h"

#define MAX_CONSOLE_ENTRIES 1000

@interface ConsoleController : NSWindowController {

	C64Proxy *c64;
	MyDocument *mydoc;

	char *ringbuffer[MAX_CONSOLE_ENTRIES];		
	int writePtr;		
			
	pthread_mutex_t ringbuffer_lock;
			
	IBOutlet NSButton *trace;
	IBOutlet NSButton *dumpCPU;
	IBOutlet NSButton *dumpCIA;
	IBOutlet NSButton *dumpVIC;
	IBOutlet NSButton *dumpIEC;
	IBOutlet NSButton *dumpMemory;
	IBOutlet NSButton *clear;
	IBOutlet NSTableView *console;		
	IBOutlet NSScrollView *consoleScroll;
	IBOutlet NSTextView *log;
	IBOutlet NSImageView *bgImage;

	// NSAutoreleasePool *pool;
}

- (void)setC64:(C64Proxy *)proxy;
- (void)setDoc:(MyDocument *)doc;

- (IBAction)traceCPUAction:(id)sender;
- (IBAction)traceIECAction:(id)sender;
- (IBAction)dumpCPUAction:(id)sender;
- (IBAction)dumpCIAAction:(id)sender;
- (IBAction)dumpVICAction:(id)sender;
- (IBAction)dumpIECAction:(id)sender;
- (IBAction)dumpMemoryAction:(id)sender;
- (IBAction)dumpDriveAction:(id)sender;
- (IBAction)refreshAction:(id)sender;
- (IBAction)clearAction:(id)sender;
- (IBAction)rasterlineAction:(id)sender;
- (IBAction)hideSpritesAction:(id)sender;

//- (void)_insertText:(NSString *)message;
- (void)_insertText:(char *)message;
- (void)insertText:(char *)message;

@end

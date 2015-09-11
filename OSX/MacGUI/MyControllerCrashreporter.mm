//
//  MyControllerCrashreporter.m
//
//  Created by Dirk Hoffmann on 11.09.15.
//
//

#import "C64GUI.h"
#import "CrashReporter/CrashReporter.h"

@implementation MyController(Crashreporter)

- (void)checkForCrashReports
{
    NSLog(@"checkForCrashReports");
    
    PLCrashReporter *crashReporter = [PLCrashReporter sharedReporter];
    NSError *error;
    
    // Check if we previously crashed
    if ([crashReporter hasPendingCrashReport])
        [self handleCrashReport];
    
    // Enable the Crash Reporter
    if (![crashReporter enableCrashReporterAndReturnError: &error])
        NSLog(@"Warning: Could not enable crash reporter: %@", error);
}


- (void) handleCrashReport {
    PLCrashReporter *crashReporter = [PLCrashReporter sharedReporter];
    NSData *crashData;
    NSError *error;
    PLCrashReport *report;
    NSString* crashreport;
    
    // Try loading the crash report
    crashData = [crashReporter loadPendingCrashReportDataAndReturnError: &error];
    if (crashData == nil) {
        NSLog(@"Could not load crash report: %@", error);
        goto finish;
    }
    
    // Parse crash report
    report = [[PLCrashReport alloc] initWithData: crashData error: &error];
    if (report == nil) {
        NSLog(@"Could not parse crash report");
        goto finish;
    }
    
    NSLog(@"Crashed on %@", report.systemInfo.timestamp);
    NSLog(@"Crashed with signal %@ (code %@, address=0x%lld", report.signalInfo.name, report.signalInfo.code, report.signalInfo.address);
    
    // Decode data
    crashreport = [PLCrashReportTextFormatter stringValueForCrashReport:report withTextFormat:PLCrashReportTextFormatiOS];
    NSLog(@"Crash log \n\n\n%@ \n\n\n", crashreport);
 
#if 0
    // Show alert dialog
    if ([self showCrashReportDialog]) {
        NSString* subject = @"VirtualC64 crash log";
        NSString* body = crashreport;
        NSString* to = @"dirkwhoffmann@gmx.de";
        
        NSString *encodedSubject = [NSString stringWithFormat:@"SUBJECT=%@", [subject stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding]];
        NSString *encodedBody = [NSString stringWithFormat:@"BODY=%@", [body stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding]];
        NSString *encodedTo = [to stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
        NSString *encodedURLString = [NSString stringWithFormat:@"mailto:%@?%@&%@", encodedTo, encodedSubject, encodedBody];
        NSURL *mailtoURL = [NSURL URLWithString:encodedURLString];
        
        [[NSWorkspace sharedWorkspace] openURL:mailtoURL];
    }
#endif
    
    // Purge the report
finish:
    [crashReporter purgePendingCrashReport];
    return;
}


- (bool) showCrashReportDialog {
    
    NSAlert *alert = [[NSAlert alloc] init];
    
    // [alert setIcon:[NSImage imageNamed:@"diskette"]];
    [alert addButtonWithTitle:@"Send crash log..."];
    [alert addButtonWithTitle:@"Cancel"];
    [alert setMessageText: @"VirtualC64 has crashed recently."];
    [alert setInformativeText: @"Please help us to improve this application by sending the recorded crash log to the development team."];
    [alert setAlertStyle: NSCriticalAlertStyle];
    
    unsigned result = [alert runModal];
    
    return (result == NSAlertFirstButtonReturn);
}

@end


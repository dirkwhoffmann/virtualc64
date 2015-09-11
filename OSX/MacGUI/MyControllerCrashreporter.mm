//
//  MyControllerCrashreporter.m
//
//  Created by Dirk Hoffmann on 11.09.15.
//
//

#import "C64GUI.h"

#import <Fabric/Fabric.h>
#import <Crashlytics/Crashlytics.h>

@implementation MyController(Crashreporter)

- (void)checkForCrashReports
{
    NSLog(@"checkForCrashReports");
    
    // Initialize Crashlytics
    [[NSUserDefaults standardUserDefaults] registerDefaults:@{ @"NSApplicationCrashOnExceptions": @YES }];
    [Fabric with:@[[Crashlytics class]]];
}

@end


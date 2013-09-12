//
//  AppDelegate.h
//  AuroraUI
//
//  Created by Espen Nordahl on 14/08/13.
//  Copyright (c) 2013 Espen Nordahl. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "RenderViewController.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>

@property (assign) IBOutlet NSWindow *window;
@property (assign) IBOutlet RenderViewController *renderViewController;

-(IBAction)buttonPressed:(id)sender;

-(void)imageDidUpdate;

@end

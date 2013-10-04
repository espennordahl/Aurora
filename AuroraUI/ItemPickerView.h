//
//  ItemPickedView.h
//  Aurora
//
//  Created by Espen Nordahl on 30/09/13.
//  Copyright (c) 2013 Espen Nordahl. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@protocol ItemPickerDelegate <NSObject>

-(void)nextItemSelected:(id)sender;
-(void)previousItemSelected:(id)sender;

@end

@interface ItemPickerView : NSView

@property id <ItemPickerDelegate> delegate;

@property NSString *name;
@property NSColor *textColor;
@property NSColor *glowColor;

@end

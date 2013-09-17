//
//  ValueWheelView.h
//  Aurora
//
//  Created by Espen Nordahl on 14/09/13.
//  Copyright (c) 2013 Espen Nordahl. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@protocol ValueViewDelegate <NSObject>

-(void)valueChanged:(id)sender;

@end

@interface ValueWheelView : NSView

@property float floatValue;

-(void)setTarget:(id)target;
-(void)removeTarget:(id)target;

@end

//
//  LightEditViewController.h
//  Aurora
//
//  Created by Espen Nordahl on 30/09/13.
//  Copyright (c) 2013 Espen Nordahl. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "ItemPickerView.h"
#import "ValueWheelView.h"
#include "light.h"
#include "session.h"


@interface LightEditViewController : NSViewController <ItemPickerDelegate, ValueViewDelegate>

-(id)initWithSession:(Aurora::Session*)session;

-(void)selectLight:(Aurora::Light*)light;

@end

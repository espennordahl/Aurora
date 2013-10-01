//
//  LightEditViewController.m
//  Aurora
//
//  Created by Espen Nordahl on 30/09/13.
//  Copyright (c) 2013 Espen Nordahl. All rights reserved.
//

#import "LightEditViewController.h"

@interface LightEditViewController ()

@property ValueWheelView *m_exposureView;
@property ItemPickerView *m_nameView;
@property Aurora::Session *m_session;
@property Aurora::Light *m_light;

@end

@implementation LightEditViewController

@synthesize m_session;
@synthesize m_exposureView;
@synthesize m_light;
@synthesize m_nameView;

-(id)initWithSession:(Aurora::Session*)session
{
    if(self = [super init]){
        m_light = Nil;
        
        self.view = [[NSView alloc] initWithFrame:CGRectMake(0, 0, 1024, 1024)];
        
        m_nameView = [[ItemPickerView alloc] initWithFrame:CGRectMake(0, 0, 512, 100)];
        [self.view addSubview:m_nameView];
        
        m_exposureView = [[ValueWheelView alloc] initWithFrame:CGRectMake(0, 120, 256, 256)];
        [m_exposureView setTarget:self];
        [self.view addSubview:m_exposureView];
        
        m_session = session;

        if (m_session->lights().size()) {
            [self selectLight:m_session->lights()[0]];
        }

    }
    
    return self;
}

-(void)selectLight:(Aurora::Light *)light
{
    m_light = light;
    m_nameView.name = [NSString stringWithUTF8String:light->name().c_str()];
    m_exposureView.floatValue = m_light->exposure();
}

-(void)nextItemSelected:(id)sender
{
    [self _selectLightWithOffset:1];
}

-(void)previousItemSelected:(id)sender
{
    [self _selectLightWithOffset:-1];
}

-(void)_selectLightWithOffset:(int)offset
{
    std::vector<Aurora::Light*> lights = m_session->lights();
    
    if (lights.size() == 1) {
        return;
    }
    
    for (int i=0; i<lights.size(); ++i) {
        if (m_light == lights[i]) {
            [self selectLight:lights[(i+offset) % (lights.size()-1)]];
        }
    }
}

-(void)valueChanged:(NSNotification*)notification
{
    NSLog(@"Value changed: %f", m_exposureView.floatValue);
    m_session->addAttributeChange(Aurora::AttributeChange(m_light->name(), "exposure", m_exposureView.floatValue, Aurora::AttributeChange::kLightChange));
}


@end

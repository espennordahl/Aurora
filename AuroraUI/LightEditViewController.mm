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
@property Aurora::Light *m_light;
@property BOOL m_selecting;
@property NSSlider *m_rslider;
@property NSSlider *m_gslider;
@property NSSlider *m_bslider;
@property NSView *m_backgroundView;

@end

@implementation LightEditViewController

@synthesize session = _session;
@synthesize m_exposureView;
@synthesize m_light;
@synthesize m_nameView;
@synthesize m_selecting;
@synthesize m_rslider;
@synthesize m_gslider;
@synthesize m_bslider;
@synthesize m_backgroundView;

-(void)awakeFromNib
{
    m_light = Nil;
    
    [self.view addObserver:self forKeyPath:@"frame" options:Nil context:Nil];
    self.view.wantsLayer = YES;
    self.view.layer.backgroundColor = [NSColor blackColor].CGColor;
    
    m_backgroundView = [[NSView alloc] init];
    m_backgroundView.wantsLayer = YES;
    m_backgroundView.layer.backgroundColor = [NSColor grayColor].CGColor;
    [self.view addSubview:m_backgroundView];
    
    m_exposureView = [[ValueWheelView alloc] init];
    [m_exposureView setTarget:self];
    [self.view addSubview:m_exposureView];

    m_nameView = [[ItemPickerView alloc] init];
    m_nameView.delegate = self;
    [self.view addSubview:m_nameView];

    m_rslider = [[NSSlider alloc] init];
    m_rslider.minValue = 0;
    m_rslider.maxValue = 1;
    [m_rslider setTarget:self];
    [m_rslider setAction:@selector(_rChanged:)];
    [self.view addSubview:m_rslider];
    
    m_gslider = [[NSSlider alloc] init];
    m_gslider.minValue = 0;
    m_gslider.maxValue = 1;
    [m_gslider setTarget:self];
    [m_gslider setAction:@selector(_gChanged:)];
    [self.view addSubview:m_gslider];
    
    m_bslider = [[NSSlider alloc] init];
    m_bslider.minValue = 0;
    m_bslider.maxValue = 1;
    [m_bslider setTarget:self];
    [m_bslider setAction:@selector(_bChanged:)];
    [self.view addSubview:m_bslider];
}

-(void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
    if (object == self.view) {
        if ([keyPath isEqualToString:@"frame"]) {
            [self _resizeViews];
        }
    }
}

-(void)setSession:(Aurora::Session*)session
{
    _session = session;
    if (self.session->lights().size()) {
        [self selectLight:self.session->lights()[0]];
    }
}

-(Aurora::Session*)session
{
    return _session;
}

-(void)selectLight:(Aurora::Light *)light
{
    m_selecting = YES;
    
    m_light = light;
    m_nameView.name = [NSString stringWithUTF8String:light->name().c_str()];
    m_exposureView.floatValue = m_light->exposure();
    
    [m_rslider setFloatValue:m_light->color.r];
    [m_gslider setFloatValue:m_light->color.g];
    [m_bslider setFloatValue:m_light->color.b];
    
    m_selecting = NO;
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
    std::vector<Aurora::Light*> lights = _session->lights();
    
    if (lights.size() == 1) {
        return;
    }
    
    for (int i=0; i<lights.size(); ++i) {
        if (m_light == lights[i]) {
            [self selectLight:lights[(i+offset) % (lights.size())]];
            break;
        }
    }
}

-(void)valueChanged:(NSNotification*)notification
{
    if (!m_selecting) {
        NSLog(@"Value changed: %f", m_exposureView.floatValue);
        _session->addAttributeChange(Aurora::AttributeChange(m_light->name(), "exposure", m_exposureView.floatValue, Aurora::AttributeChange::kLightChange));
    }
}

-(void)_rChanged:(id)sender
{
    if (!m_selecting) {
        _session->addAttributeChange(Aurora::AttributeChange(m_light->name(), "r", m_rslider.floatValue, Aurora::AttributeChange::kLightChange));
    }

}

-(void)_gChanged:(id)sender
{
    if (!m_selecting) {
        _session->addAttributeChange(Aurora::AttributeChange(m_light->name(), "g", m_gslider.floatValue, Aurora::AttributeChange::kLightChange));
    }
}

-(void)_bChanged:(id)sender
{
    if (!m_selecting) {
        _session->addAttributeChange(Aurora::AttributeChange(m_light->name(), "b", m_bslider.floatValue, Aurora::AttributeChange::kLightChange));
    }
}

-(void)_resizeViews
{
    m_backgroundView.frame = CGRectMake(self.view.frame.origin.x,
                                        self.view.frame.origin.y,
                                        400,
                                        self.view.frame.size.height);
    
    m_exposureView.frame = CGRectMake(self.view.frame.origin.x + 25,
                                      self.view.frame.origin.y + 10,
                                      MIN(150, self.view.frame.size.width),
                                      MIN(150, self.view.frame.size.height));
    
    m_nameView.frame = CGRectMake(self.view.frame.origin.x,
                                  self.view.frame.origin.y + m_exposureView.frame.size.height,
                                  200,
                                  self.view.frame.size.height - m_exposureView.frame.size.height);
    
    const float sliderErode = 25;
    m_rslider.frame = CGRectMake(CGRectGetMaxX(m_exposureView.frame) + sliderErode,
                                 1 * self.view.frame.size.height/2,
                                 MIN(self.view.frame.size.width - m_exposureView.frame.size.width - sliderErode*2, 200),
                                 50);
    
    m_gslider.frame = CGRectOffset(m_rslider.frame, 0, -self.view.frame.size.height/5);

    m_bslider.frame = CGRectOffset(m_gslider.frame, 0, -self.view.frame.size.height/5);
}

@end

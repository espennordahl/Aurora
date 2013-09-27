//
//  AppDelegate.m
//  AuroraUI
//
//  Created by Espen Nordahl on 14/08/13.
//  Copyright (c) 2013 Espen Nordahl. All rights reserved.
//

#import "AppDelegate.h"

#include "session.h"

class SessionBridge : public Aurora::Session{
public:
    SessionBridge(const std::string &scenefile):
    Aurora::Session(scenefile)
    {};
    
    void imageDidUpdate(){
        [delegate performSelectorOnMainThread:@selector(imageDidUpdate:) withObject:Nil waitUntilDone:NO];
    }
    
    AppDelegate *delegate;
};

@interface AppDelegate ()

@property SessionBridge *m_session;

@end

@implementation AppDelegate

@synthesize m_session;
@synthesize wheel = _wheel;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    [self.renderViewController.view addObserver:self forKeyPath:@"frame" options:Nil context:Nil];
    [self _setupSession];
    m_session->start();
}

-(void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
    if (object == self.renderViewController.view) {
        if ([keyPath isEqualToString:@"frame"]) {
            m_session->setResolution(self.renderViewController.view.frame.size.width, self.renderViewController.view.frame.size.height);
        }
    }
}



-(void)_setupSession
{
    m_session = new SessionBridge("/Users/espennordahl/Documents/Aurora/pyAurora/tmp.asc");
    m_session->setResolution(self.renderViewController.view.frame.size.width, self.renderViewController.view.frame.size.height);
    m_session->delegate = self;
}

-(IBAction)buttonPressed:(id)sender
{
    m_session->stop();
    m_session->start();
}

-(void)imageDidUpdate:(NSNotification*)notification
{
    void *pixeldata = m_session->imageFile();
    int width = m_session->width();
    int height = m_session->height();
    
    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL,
                                                              pixeldata,
                                                              width*height*4,
                                                              NULL);
    
    int bitsPerComponent = 8;
    int bitsPerPixel = 4*8;
    int bytesPerRow = 4 * 8 * width / 8;

    CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
    CGBitmapInfo bitmapInfo = kCGImageAlphaLast;
    CGColorRenderingIntent renderingIntent = kCGRenderingIntentDefault;
    CGImageRef imageRef = CGImageCreate(width,
                                        height,
                                        bitsPerComponent,
                                        bitsPerPixel,
                                        bytesPerRow,
                                        colorSpaceRef,
                                        bitmapInfo,
                                        provider,
                                        NULL,
                                        NO,
                                        renderingIntent);

    NSImage *render = [[NSImage alloc] initWithCGImage:imageRef size:NSZeroSize];
    
    [self.renderViewController imageDidChange:render];
}

-(void)setWheel:(ValueWheelView *)wheel
{
    [_wheel removeTarget:self];
    [wheel setTarget:self];
    _wheel = wheel;
}

-(ValueWheelView*)wheel
{
    return _wheel;
}

-(void)valueChanged:(NSNotification*)notification
{
    NSLog(@"Value changed: %f", self.wheel.floatValue);
    m_session->stop();
    m_session->addAttributeChange(Aurora::AttributeChange("pCubeShape1", "abc", self.wheel.floatValue, Aurora::AttributeChange::kShaderChange));
    m_session->start();
}

@end

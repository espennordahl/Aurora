//
//  session.cpp
//  Aurora
//
//  Created by Espen Nordahl on 7/09/13.
//  Copyright (c) 2013 Espen Nordahl. All rights reserved.
//

#include "session.h"
#include <tbb.h>

using namespace Aurora;

Session::Session():
m_renderer(Renderer(""))
{
    m_renderer.setDelegate(this);
}

Session::Session(const std::string scenefile):
m_renderer(Renderer(scenefile.c_str()))
{
    m_renderer.setDelegate(this);
}

class render_task : public tbb::task {
public:
    render_task(Renderer *renderer): m_renderer(renderer){};
    
    task* execute(  ) {
        m_renderer->render();
        return NULL;
    }
    
private:
    Renderer *m_renderer;
};

void Session::start(){
    render_task& task = *new(tbb::task::allocate_root()) render_task(&m_renderer);
    tbb::task::spawn(task);
}

class stop_task : public tbb::task {
public:
    stop_task(Renderer *renderer): m_renderer(renderer){};
    
    task* execute(  ) {
        m_renderer->stop();
        return NULL;
    }
    
private:
    Renderer *m_renderer;
};


void Session::stop(){
    stop_task& task = *new(tbb::task::allocate_root()) stop_task(&m_renderer);
    tbb::task::spawn(task);
}

void Session::addObject(ObjectPtr object){
    
}

void Session::removeObject(ObjectPtr object){
    
}

void Session::objectChanged(ObjectPtr object, ChangeType change){
    
}

void Session::setResolution(int width, int height){
    
}

const std::vector<ObjectPtr> &Session::objects() const{
    return std::vector<ObjectPtr>();
}

void *Session::imageFile(){
    return m_renderer.displayDriver->copy();
}

int Session::width(){
    return m_renderer.displayDriver->width();
}

int Session::height(){
    return m_renderer.displayDriver->height();
}


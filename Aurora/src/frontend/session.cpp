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
    m_renderer.parseSceneDescription();
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
    applyAttributeChanges();
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
    tbb::task::spawn_root_and_wait(task);
}

void Session::addObject(ObjectPtr object){
    m_objects.push_back(object);
}

void Session::removeObject(ObjectPtr object){
    
}

void Session::objectChanged(ObjectPtr object, ChangeType change){
    
}

void Session::setResolution(int width, int height){
    bool running = m_renderer.isRunning();
    if (running) {
        stop();
    }
    m_renderer.setResolution(width, height);
    if (running) {
        start();
    }
}

const std::vector<ObjectPtr> &Session::objects() const{
    return m_objects;
}

char *Session::imageFile(){
    return m_renderer.displayDriver->copy();
}

int Session::width(){
    return m_renderer.displayDriver->width();
}

int Session::height(){
    return m_renderer.displayDriver->height();
}

void Session::addAttributeChange(const AttributeChange &change)
{
    m_changeMutex.lock();
    
    m_changes.push_back(change);

    m_changeMutex.unlock();
}

void Session::applyAttributeChanges()
{
    m_changeMutex.lock();
    
    for (int i=0; i<m_changes.size(); ++i) {
        bool foundObject = false;
        for (int j=0; j<m_objects.size(); ++j) {
            if (m_objects[j]->name() == m_changes[i].objectName()) {
                foundObject = true;
                m_objects[j]->applyAttributeChange(m_changes[i]);
            }
        }
        assert(foundObject);
    }
    
    m_changes.clear();
    
    m_changeMutex.unlock();
    
}
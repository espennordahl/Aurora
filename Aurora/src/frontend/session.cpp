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

const std::vector<Light*> &Session::lights() const{
    return m_renderer.lights;
}


const std::vector<ObjectPtr> &Session::objects() const{
    return m_renderer.objects;
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

char *Session::proxyFile(int level){
    return m_renderer.displayDriver->proxy(level);
}

void Session::addAttributeChange(const AttributeChange &change)
{
    bool running = m_renderer.isRunning();
    if (running) {
        stop();
    }
    
    m_changeMutex.lock();
    m_changes.push_back(change);
    m_changeMutex.unlock();

    if (running) {
        start();
    }
}

void Session::applyAttributeChanges()
{
    m_changeMutex.lock();
    
    std::vector<Aurora::ObjectPtr> objects = this->objects();
    std::vector<Aurora::Light*> lights = this->lights();
    
    for (int i=0; i<m_changes.size(); ++i) {
        bool foundObject = false;
        
        for (int j=0; j<objects.size(); ++j) {
            if (objects[j]->name() == m_changes[i].objectName()) {
                foundObject = true;
                objects[j]->applyAttributeChange(m_changes[i]);
                break;
            }
        }
        
        if (foundObject) {
            continue;
        }

        for (int l=0; l<lights.size(); ++l) {
            if (lights[l]->name() == m_changes[i].objectName()) {
                foundObject = true;
                lights[l]->applyAttributeChange(m_changes[i]);
                break;
            }
        }
        
        assert(foundObject);
    }
    
    
    
    m_changes.clear();
    
    m_changeMutex.unlock();
    
}
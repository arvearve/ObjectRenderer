//
// Created by Arve Nygård on 19/01/16.
//

#ifndef OBJECTRENDERER_RENDERER_H
#define OBJECTRENDERER_RENDERER_H
#include "luxcore/luxcore.h"
using namespace luxrays;
using namespace luxcore;
class Renderer {
    Properties props;
    RenderConfig config;
    RenderSession session;
    int renderTime;
    static void log(const char*);
    static void log(const std::string);
    vector<Point> getCameraPositions() const;
    void waitAndSave();
    void stats();
public:
    Renderer(const std::string filename, const int renderTime)
            :props(filename), config(props), session(&config), renderTime(renderTime){
        Init();
        log("loaded scene file: " + filename);
    };

    void render();

};


#endif //OBJECTRENDERER_RENDERER_H

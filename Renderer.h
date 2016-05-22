//
// Created by Arve Nygård on 19/01/16.
//

#ifndef OBJECTRENDERER_RENDERER_H
#define OBJECTRENDERER_RENDERER_H
#include "luxcore/luxcore.h"
#include <iostream>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <vector>
using namespace luxrays;
using namespace luxcore;
namespace fs = boost::filesystem;
class Renderer {
    Properties props;
    RenderConfig config;
    RenderSession session;
    int environmentIndex;
    int renderTime;
    fs::path configFile;
    fs::directory_iterator current_mesh;
    vector<std::string> material_names;
    vector<fs::path> environment_files;
    vector<std::string> env_objects;
    vector<std::string> env_lights;
    float camera_distance;
    static void log(const char*);
    static void log(const std::string);
    vector<Point> getCameraPositions() const;
    void waitAndSave(const int);
    void stats();
    std::string getRandomMaterial();
    bool hasNextMesh();
    void renameFile(const int image_number);
    void loadMaterials();
    vector<boost::filesystem::path> getEnvironments();
public:
    Renderer(const std::string filename, const int renderTime):
            props(filename),
            config(props),
            session(&config),
            renderTime(renderTime),
            configFile(boost::filesystem::path(filename)),
            current_mesh(boost::filesystem::directory_iterator(configFile.parent_path() /= "models/"))

    {
        camera_distance = 0.156;
        environmentIndex = 0;
        Init();
        loadMaterials();
        environment_files = getEnvironments();
        log("loaded scene file: " + filename);
    };
    void render();




//    void addRandomBase();

    void addNextMesh();

    void addRandomEnvironment();

    void cleanScene();

};


#endif //OBJECTRENDERER_RENDERER_H

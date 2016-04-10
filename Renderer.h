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
    int renderTime;
    fs::path configFile;
    fs::directory_iterator current_mesh;
    vector<std::string> material_names;
    vector<std::string> environment_names;
    vector<fs::path> base_models;
    static void log(const char*);
    static void log(const std::string);
    vector<Point> getCameraPositions() const;
    void waitAndSave(const int);
    void stats();
    std::string getRandomMaterial();
    void loadMaterials();
    vector<fs::path> loadBases() {
        log("loading bases");
        fs::path base_models_folder = configFile.parent_path() /= "bases/";
        fs::directory_iterator end_itr;
        vector<fs::path> result;
        for (fs::directory_iterator itr(base_models_folder); itr != end_itr; ++itr) {
            // Ignore folders.
            if (fs::is_regular_file(itr->path())) {
                result.push_back(itr->path());
            }
        }
        return result;
    }

    void addRandomEnvironmentMap();
public:
    Renderer(const std::string filename, const int renderTime):
            props(filename),
            config(props),
            session(&config),
            renderTime(renderTime),
            configFile(boost::filesystem::path(filename)),
            current_mesh(boost::filesystem::directory_iterator(configFile.parent_path() /= "models/")),
            base_models(loadBases())
    {
        Init();
        loadMaterials();
        addRandomEnvironmentMap();
        log("loaded scene file: " + filename);
    };
    void render();
    bool hasNextMesh();
    void renameFile(const int image_number);


    void addRandomBase();

    void addNextMesh();
};


#endif //OBJECTRENDERER_RENDERER_H

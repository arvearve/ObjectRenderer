//
// Created by Arve Nygård on 19/01/16.
//

#include <iostream>
#include <boost/format.hpp>
#include "Renderer.h"
void Renderer::log(const char *string) {
    std::cout << string << std::endl;
}

void Renderer::log(const std::string string) {
    log(string.c_str());
}

void Renderer::waitAndSave() {
    const double startTime = WallClockTime();
    for (;;) {
        boost::this_thread::sleep(boost::posix_time::millisec(1000));

        const double elapsedTime = WallClockTime() - startTime;

        // Print some information about the rendering progress

        stats();
        if (elapsedTime > renderTime) {
            // Time to stop the rendering
            break;
        }
    }
    session.GetFilm().SaveOutputs();
}


void Renderer::render() {
    session.Start();

    Scene scene = config.GetScene();
    int camera_position_number = 0;
    std::string oldFileName = session.GetRenderConfig().GetProperty("film.outputs.1.filename").GetValuesString();
    std::string oldFileNameAlpha = session.GetRenderConfig().GetProperty("film.outputs.2.filename").GetValuesString();
    std::vector<Point> cameraPositions = getCameraPositions();
    for (std::vector<Point>::iterator point = cameraPositions.begin(); point != cameraPositions.end(); ++point) {
        session.BeginSceneEdit();
        scene.Parse(
                Property("scene.camera.lookat.orig")(point->x*2.f, point->y*2.f, point->z*2.f) <<
                Property("scene.camera.lookat.target")(0.f, 0.f, 0.f));
        session.EndSceneEdit();
        waitAndSave();
        std::string filename = (boost::format("%s_%d.png") % oldFileName % camera_position_number).str();
        std::string filenameAlpha = (boost::format("%s_%d.png") % oldFileNameAlpha % camera_position_number).str();
        boost::filesystem::rename(oldFileName, filename);
        boost::filesystem::rename(oldFileNameAlpha, filenameAlpha);
        std::cout << "Filename set to: " << filename << std::endl;
        camera_position_number++;
    }
}

void Renderer::stats() {
    session.UpdateStats();
    const Properties stats = session.GetStats();
    std::cout << boost::format(
            "[Elapsed time: %3d/%dsec][Samples %4d][Avg. samples/sec % 3.2fM on %.1fK tris]\n") %
                         ((int)stats.Get("stats.renderengine.time").Get<double>())%
                         (renderTime)%
                         (stats.Get("stats.renderengine.pass").Get<u_int>()) %
                         (stats.Get("stats.renderengine.total.samplesec").Get<double>()  / 1000000.0) %
                         (stats.Get("stats.dataset.trianglecount").Get<double>() / 1000.0);
}

/* Return the coordinates of each vertex in a Regular dodecahedron of unit size,
 * except those that lie below 0 on the Z axis*/
vector<Point> Renderer::getCameraPositions() const {
    std::vector<Point> result;
    const float phi = 1.618f; // Golden ratio: ϕ = (1+sqrt(5))/2
    const float phi_inverse = 0.618f; // 1/ϕ
    result.push_back(Point( phi, 0.f, 0.f));
    result.push_back(Point( phi_inverse,  phi, 0.f));
    result.push_back(Point(-phi_inverse,  phi, 0.f));
    result.push_back(Point(-phi, 0.f, 0.f));
    result.push_back(Point(-phi_inverse, -phi, 0.f));
    result.push_back(Point( phi_inverse, -phi, 0.f));
    result.push_back(Point( phi, 0.f, phi_inverse));
    result.push_back(Point( 1.f,  1.f, 1.f));
    result.push_back(Point(-1.f,  1.f, 1.f));
    result.push_back(Point(-phi, 0.f, phi_inverse));

    result.push_back(Point(-1.f, -1.f, 1.f));
    result.push_back(Point( 1.f, -1.f, 1.f));
    result.push_back(Point(0.f,  phi_inverse, phi));
    result.push_back(Point(0.f, -phi_inverse, phi));
    return result;
}


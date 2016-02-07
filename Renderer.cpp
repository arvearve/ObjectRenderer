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

void Renderer::render() {
    session.Start();
}

void Renderer::save() {
    session.GetFilm().SaveOutputs();
    session.Stop();
}

void Renderer::stats() {
    session.UpdateStats();
    const Properties stats = session.GetStats();
    std::cout << boost::format("[Elapsed time: %3d/%dsec][Samples %4d][Avg. samples/sec % 3.2fM on %.1fK tris]\n") %
            (int)stats.Get("stats.renderengine.time").Get<double>() %
            renderTime %
            stats.Get("stats.renderengine.pass").Get<u_int>() %
            (stats.Get("stats.renderengine.total.samplesec").Get<double>()  / 1000000.0) %
            (stats.Get("stats.dataset.trianglecount").Get<double>() / 1000.0);
}

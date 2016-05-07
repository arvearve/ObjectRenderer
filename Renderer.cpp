#include "Renderer.h"

void Renderer::log(const char *string) {
    std::cout << string << std::endl;
}

void Renderer::log(const std::string string) {
    log(string.c_str());
}

void Renderer::waitAndSave(const int image_number) {
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
    renameFile(image_number);
}

bool Renderer::hasNextMesh(){
    fs::directory_iterator end_itr;
    return !(end_itr == current_mesh);
}

void Renderer::addNextMesh() {
    std::string model_filename = current_mesh->path().string();
    std::cerr << "selecting model: " << model_filename << std::endl;
    config.GetScene().Parse(
            Property("scene.shapes.subject.type")("mesh") <<
            Property("scene.shapes.subject.ply")(model_filename));
    config.GetScene().RemoveUnusedMeshes();
}

void Renderer::render() {
    session.Start();
    Scene scene = config.GetScene();
    std::vector<Point> cameraPositions = getCameraPositions();
    // Render each mesh
    while(hasNextMesh()){
        session.BeginSceneEdit();
        cleanScene();
        addNextMesh();
        addRandomEnvironment();

        // Every mesh gets a random material
        config.GetScene().Parse(
                Property("scene.objects.subject.material")(getRandomMaterial()) <<
                Property("scene.objects.subject.shape")("subject") <<
                Property("scene.objects.subject.id")(1)
        );
        session.EndSceneEdit();
        // Render from multiple camera positions
        for (int image_number = 0; image_number < cameraPositions.size(); image_number++) {
            Point point = cameraPositions[image_number];
            session.BeginSceneEdit();
            scene.Parse(
                    Property("scene.camera.lookat.orig")(point.x*2.f, point.y*2.f, point.z*2.f) <<
                    Property("scene.camera.lookat.target")(0.f, 0.f, 0.f));
            session.EndSceneEdit();
            waitAndSave(image_number);
        }
        current_mesh++;
    }
}

void Renderer::renameFile(const int image_number) {
    fs::path output_folder = fs::path("output") / current_mesh->path().filename();
    fs::create_directories(output_folder);

    fs::path source_rgb(session.GetRenderConfig().GetProperty("film.outputs.1.filename").GetValuesString());
    fs::path source_mask(session.GetRenderConfig().GetProperty("film.outputs.2.filename").GetValuesString());

    fs::path target_rgb = output_folder / fs::path((boost::format("%d.png") % image_number).str());
    fs::path target_mask = output_folder / fs::path((boost::format("%d_mask.png") % image_number).str());

    fs::rename(source_rgb, target_rgb);
    fs::rename(source_mask, target_mask);
    std::cout << "Saving file: " << target_rgb << std::endl;
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
//    result.push_back(Point(-phi_inverse, -phi, 0.f));
//    result.push_back(Point( phi_inverse, -phi, 0.f));
//    result.push_back(Point( phi, 0.f, phi_inverse));
//    result.push_back(Point( 1.f,  1.f, 1.f));
//    result.push_back(Point(-1.f,  1.f, 1.f));
//    result.push_back(Point(-phi, 0.f, phi_inverse));
//
//    result.push_back(Point(-1.f, -1.f, 1.f));
//    result.push_back(Point( 1.f, -1.f, 1.f));
//    result.push_back(Point(0.f,  phi_inverse, phi));
//    result.push_back(Point(0.f, -phi_inverse, phi));
    return result;
}

std::string Renderer::getRandomMaterial() {
    unsigned long index = rand() % material_names.size();
    std::string material = material_names[index];
    std::cout << "Selecting random material: " << material << std::endl;
    return material;
}

void Renderer::loadMaterials() {
    log("Loading materials...");
    fs::path materials_folder = configFile.parent_path() /= "materials/";
    Properties all_materials;

    fs::directory_iterator end_itr;
    for (fs::directory_iterator itr(materials_folder); itr != end_itr; ++itr) {
        // Ignore folders.
        if (fs::is_regular_file(itr->path())) {
            Properties material(itr->path().string());
            all_materials << material;
        }
    }
    config.GetScene().Parse(all_materials);
    vector<std::string> materialKeys = all_materials.GetAllUniqueSubNames("scene.materials");
    std::vector<std::string>::const_iterator it;
    for(it = materialKeys.begin(); it != materialKeys.end(); it++){
        std::string materialName = Property::ExtractField(*it, 2);
        material_names.push_back(materialName);
    }
}
vector<fs::path> Renderer::getEnvironments() {
    fs::path env_folder = configFile.parent_path() /= "environment/";
    std::vector<fs::path> env_files;
    fs::recursive_directory_iterator end_itr;
    for (fs::recursive_directory_iterator itr(env_folder); itr != end_itr; ++itr) {
        // Ignore folders.
        if (fs::is_regular_file(itr->path()) && itr->path().extension() == ".scn") {
            env_files.push_back(itr->path());
        }
    }
    std::cerr << "loaded " << env_files.size() << "envs. " << std::endl;
    return env_files;
}

void Renderer::addRandomEnvironment() {
    unsigned long index = rand() % environment_files.size();
    std::string env_file = environment_files[index].string();
    std::cerr << "using environment: " << env_file << std::endl;
    config.GetScene().Parse(env_file);
}

void Renderer::cleanScene() {
    Scene scene = config.GetScene();
    vector<std::string> objects = scene.ToProperties().GetAllUniqueSubNames("scene.objects");
    for (std::string object:objects) {
        std::cerr << "removing object: " << object << std::endl;
        scene.DeleteObject(object);
    }
    vector<std::string> lights = scene.ToProperties().GetAllUniqueSubNames("scene.lights");
    for (std::string light:lights) {
        std::cerr << "removing light: " << light << std::endl;
        scene.DeleteLight(light);
    }
    scene.RemoveUnusedMeshes();
}



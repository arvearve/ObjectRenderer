#include "boost/program_options.hpp"
#include "Renderer.h"
#include <iostream>


void deviceInfo() {
    {
        luxrays::Context ctx;
        const vector<luxrays::DeviceDescription *> &deviceDescriptions = ctx.GetAvailableDeviceDescriptions();

        // Print device info
        for (size_t i = 0; i < deviceDescriptions.size(); ++i) {
            luxrays::DeviceDescription *desc = deviceDescriptions[i];
            std::cout << "Device " << i << " name: " << desc->GetName() << "\n";
            std::cout << "Device " << i << " type: " << luxrays::DeviceDescription::GetDeviceType(desc->GetType()) << "\n";
            std::cout << "Device " << i << " compute units: " << desc->GetComputeUnits() << "\n";
            std::cout << "Device " << i << " preferred float vector width: " << desc->GetNativeVectorWidthFloat() << "\n";
            std::cout << "Device " << i << " max allocable memory: " << desc->GetMaxMemory() / (1024 * 1024) << "MBytes" << "\n";
            std::cout << "Device " << i << " max allocable memory block size: " << desc->GetMaxMemoryAllocSize() / (1024 * 1024) << "MBytes" << "\n";
        }
    }
    std::cout << "\n";
}



int main(int argc, const char* argv[]){
    using namespace boost::program_options;
    options_description description("ObjectRenderer Usage");

    description.add_options()
            ("help,h", "Display this help message")
            ("scene-file", value<std::string>(), "Scene file")
            ("render-time", value<int>()->default_value(5), "Render time")
            ("version,v", "Display the version number");

    positional_options_description p;
    p.add("scene-file", -1);

    variables_map vm;
    store(
            command_line_parser(argc, argv)
                    .options(description)
                    .positional(p)
                    .run(),
            vm);
    notify(vm);

    if(vm.count("version")){
        std::cout << "ObjectRenderer Version 1.0" << std::endl;
        return 0;
    }

    else if(vm.count("scene-file")){
        std::string file = vm["scene-file"].as<std::string >();
        int renderTime = vm["render-time"].as<int>();
        Renderer renderer(file, renderTime);
        renderer.render();
    }
    else {
        std::cout << description << std::endl;
        deviceInfo();
        return 0;
    }
    return 0;
}


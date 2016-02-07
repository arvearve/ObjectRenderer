/***************************************************************************
 * Copyright 1998-2015 by authors (see AUTHORS.txt)                        *
 *                                                                         *
 *   This file is part of LuxRender.                                       *
 *                                                                         *
 * Licensed under the Apache License, Version 2.0 (the "License");         *
 * you may not use this file except in compliance with the License.        *
 * You may obtain a copy of the License at                                 *
 *                                                                         *
 *     http://www.apache.org/licenses/LICENSE-2.0                          *
 *                                                                         *
 * Unless required by applicable law or agreed to in writing, software     *
 * distributed under the License is distributed on an "AS IS" BASIS,       *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
 * See the License for the specific language governing permissions and     *
 * limitations under the License.                                          *
 ***************************************************************************/

#include <iostream>

#include <boost/foreach.hpp>
#include <boost/assign.hpp>
#include <boost/format.hpp>

#include <luxcore/luxcore.h>

using namespace std;
using namespace luxrays;
using namespace luxcore;

int main(int argc, char *argv[]) {
    try {
        luxcore::Init();

        cout << "LuxCore v" << LUXCORE_VERSION_MAJOR << "." << LUXCORE_VERSION_MINOR << "\n\n" ;


        //----------------------------------------------------------------------
        cout << "LuxRays device information example...\n";
        //----------------------------------------------------------------------

        {
            luxrays::Context ctx;
            const vector<luxrays::DeviceDescription *> &deviceDescriptions = ctx.GetAvailableDeviceDescriptions();

            // Print device info
            for (size_t i = 0; i < deviceDescriptions.size(); ++i) {
                luxrays::DeviceDescription *desc = deviceDescriptions[i];
                cout << "Device " << i << " name: " << desc->GetName() << "\n";
                cout << "Device " << i << " type: " << luxrays::DeviceDescription::GetDeviceType(desc->GetType()) << "\n";
                cout << "Device " << i << " compute units: " << desc->GetComputeUnits() << "\n";
                cout << "Device " << i << " preferred float vector width: " << desc->GetNativeVectorWidthFloat() << "\n";
                cout << "Device " << i << " max allocable memory: " << desc->GetMaxMemory() / (1024 * 1024) << "MBytes" << "\n";
                cout << "Device " << i << " max allocable memory block size: " << desc->GetMaxMemoryAllocSize() / (1024 * 1024) << "MBytes" << "\n";
            }
        }
        cout << "\n";

        //----------------------------------------------------------------------
        cout << "RenderConfig and RenderSession examples (requires scenes directory)...\n";
        //----------------------------------------------------------------------

        {
            //------------------------------------------------------------------
            cout << "A simple rendering...\n";
            //------------------------------------------------------------------

            // Load the configuration from filel
            Properties props("scenes/cornell/cornell-gpu.cfg");

            // Change the render engine to PATHCPU
            props.Set(Property("renderengine.type")("PATHOCL"));
            RenderConfig *config = new RenderConfig(props);
            RenderSession *session = new RenderSession(config);

            session->Start();

            const double startTime = WallClockTime();
            for (;;) {
                boost::this_thread::sleep(boost::posix_time::millisec(1000));

                const double elapsedTime = WallClockTime() - startTime;

                // Print some information about the rendering progress

                // Update statistics
                session->UpdateStats();

                // Print all statistics
                //cout << "[Elapsed time: " << (int)elapsedTime << "/5]\n";
                //cout << session->GetStats();

                const Properties &stats = session->GetStats();
                cout << boost::format("[Elapsed time: %3d/5sec][Samples %4d][Avg. samples/sec % 3.2fM on %.1fK tris]\n") %
                        (int)stats.Get("stats.renderengine.time").Get<double>() %
                        stats.Get("stats.renderengine.pass").Get<u_int>() %
                        (stats.Get("stats.renderengine.total.samplesec").Get<double>()  / 1000000.0) %
                        (stats.Get("stats.dataset.trianglecount").Get<double>() / 1000.0);

                if (elapsedTime > 5.0) {
                    // Time to stop the rendering
                    break;
                }
            }

            session->Stop();

            // Save the rendered image
            session->GetFilm().SaveOutputs();

            delete session;
            delete config;

            cout << "Done.\n";
        }
    } catch (runtime_error &err) {
        cerr << "RUNTIME ERROR: " << err.what() << "\n";
        return EXIT_FAILURE;
    } catch (exception &err) {
        cerr << "ERROR: " << err.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

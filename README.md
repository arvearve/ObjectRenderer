#Building and running

## Dependencies:
Make sure the following packages are installed on your computer

* cmake
* curl
* libboost-all-dev
* libglew-dev
* libopenexr-dev
* libopenimageio-dev
* libpng12-dev
* libtiff5-dev

```
sudo apt-get install cmake curl libboost-all-dev libglew-dev libopenexr-dev libopenimageio-dev libpng12-dev libtiff5-dev
```

In order to tell CMake where to find OpenCL files, export the path as an environment variable. Examples:
```
export ATISTREAMSDKROOT=/home/yourname/AMDAPPSDK-3.0
export AMDAPPSDKROOT=/path/to/AMDAPPSDKROOT
export CUDA_PATH=/path/to/NVIDIA/CUDA-7.0
export INTELOCLSDKROOT=/path/to/intel_ocl_sdk_root
```

## Building
After making sure all dependencies are installed and the path to your OpenCL sdk is exposed as an environment variable, go to the projects root folder and type
```
make
```
This will build the program, as well as download and convert all the models from the Princeton Shape Benchmark.

## Running
To start the program with the default config, and render each image for 10 seconds do:
```
./objectrenderer scenes/base.cfg --render-time 10
```



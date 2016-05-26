FROM ubuntu:15.10

RUN apt-get update && apt-get install -y cmake curl libboost-all-dev libglew-dev libopenexr-dev libopenimageio-dev libpng12-dev libtiff5-dev
COPY ./ objectrenderer/
COPY libOpenCL.so.1 /usr/lib/libOpenCL.so.1
ENV ATISTREAMSDKROOT /objectrenderer/AMDAPPSDK-3.0
# Change the NVIDIA driver version to the one in the
# the physical node
ENV NVIDIA_VER 352.93
ENV NVIDIA_DOWNLOAD http://us.download.nvidia.com/XFree86/Linux-x86_64/$NVIDIA_VER/NVIDIA-Linux-x86_64-$NVIDIA_VER.run
ENV NVIDIA_DRV NVIDIA-Linux-x86_64-$NVIDIA_VER.run



#RUN cd /tmp/setup && \
#    wget $NVIDIA_DOWNLOAD && \
#    chmod 700 $NVIDIA_DRV && \
#    ./$NVIDIA_DRV -s --no-kernel-module && \
#    cd /usr/include && \
#    wget http://www.lip.pt/~david/cl_include.tgz && \
#    tar zxvf cl_include.tgz && \

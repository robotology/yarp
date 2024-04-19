ARG base_img=ubuntu:22.04
FROM ${base_img}
LABEL maintainer="Marco Randazzo"

# Non-interactive installation mode
ENV DEBIAN_FRONTEND=noninteractive
ENV DOCKER_UPDATED_ON=23_11_2023

# Install essentials
RUN apt-get update && apt-get install -y apt-utils \
                          software-properties-common \
                          sudo \
                          psmisc \
                          lsb-release \
                          protobuf-compiler \
                          libatlas-base-dev \
                          tmux \
                          nano \
                          geany \
                          vim \
                          wget \
                          curl \
                          build-essential \
                          git gitk \
                          cmake \
                          cmake-curses-gui \
                          autoconf \
                          xserver-xorg-video-dummy \
                          xserver-xorg-legacy \
                          net-tools \
                          terminator \
                          apt-transport-https \
                          ca-certificates \
                          gnupg \
                          locales \
                          python3-setuptools \
                          python3-pip \
                          iproute2 \
                          python3-tornado \
                          lsof \
                          iftop \
                          iputils-ping \
                          gdb \
                          bash-completion \
                          btop \
                          mlocate

RUN pip3 install numpy bpytop

RUN sed -i 's/allowed_users=console/allowed_users=anybody/' /etc/X11/Xwrapper.config
COPY ./common/xorg.conf /etc/X11/xorg.conf

# Install yarp dependencies
RUN apt-get install -y     libace-dev \
                           libsqlite3-dev \
                           libtinyxml-dev \
                           libedit-dev \
                           qtbase5-dev \
                           qtdeclarative5-dev \
                           qtmultimedia5-dev \
                           libqt5opengl5-dev \
                           libqcustomplot-dev \
                           libopencv-dev \
                           libeigen3-dev \
                           libgraphviz-dev \
                           libpng-dev \
                           libv4l-dev \
                           libavcodec-dev \
                           libavdevice-dev \
                           libavformat-dev \
                           libavutil-dev \
                           portaudio19-dev \
                           libsdl1.2-dev \
                           libopenni2-dev \
                           libftdi-dev \
                           libi2c-dev \
                           libjpeg-dev \
                           libpcl-dev \
                           libsoxr-dev \
                           libgstreamer1.0-dev \
                           libgstreamer-plugins-base1.0-dev

# Install SWIG and bindings dependencies
RUN apt-get install -qq -y swig \
                           mono-mcs \
                           liblua5.3-dev \
                           lua5.3 \
                           tcl-dev \
                           tk-dev \
                           python3-dev \
                           liboctave-dev \
                           ruby-dev \
                           ruby \
                           perl

# Create user: user1
USER root
RUN useradd -l -u 33334 -G sudo -md /home/user1 -s /bin/bash -p user1 user1 && \
    # passwordless sudo for users in the 'sudo' group
    sed -i.bkp -e 's/%sudo\s\+ALL=(ALL\(:ALL\)\?)\s\+ALL/%sudo ALL=NOPASSWD:ALL/g' /etc/sudoers

# Set ENV
USER user1
RUN mkdir /home/user1/robotology
ENV robotology_install_user user1
ENV user1_home /home/$robotology_install_user
ENV robotology_install_folder $user1_home/robotology

# Build ycm
USER $robotology_install_user
WORKDIR $robotology_install_folder
RUN git clone https://github.com/robotology/ycm.git -b master
RUN cd ycm && mkdir build && cd build && \
    cmake .. \
    -DCMAKE_BUILD_TYPE=Release && \
    make -j11
ENV YCM_DIR=/home/user1/robotology/ycm/build

# Build YARP
ARG yarp_branch=master
USER $robotology_install_user
WORKDIR $robotology_install_folder
RUN git clone https://github.com/robotology/yarp.git -b ${yarp_branch}
RUN cd yarp && mkdir build && cd build && \
    cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DYARP_COMPILE_libYARP_math=ON \
    -DYARP_COMPILE_GUIS=ON \
    -DYARP_COMPILE_DEVICE_PLUGINS=ON \
    -DYARP_COMPILE_ALL_FAKE_DEVICES=ON \
    -DENABLE_yarpcar_mjpeg=ON \
    -DENABLE_yarpcar_segmentationimage=ON \
    -DENABLE_yarpcar_portmonitor=ON \
    -DENABLE_yarppm_bottle_compression_zlib=ON \
    -DENABLE_yarppm_depthimage_compression_zlib=ON \
    -DENABLE_yarppm_image_compression_ffmpeg=ON \
    -DENABLE_yarppm_depthimage_to_mono=ON \
    -DENABLE_yarppm_depthimage_to_rgb=ON && \
    make -j11
ENV YARP_ROOT=$robotology_install_folder/yarp
ENV YARP_DIR=$robotology_install_folder/yarp/build

# YARP bindings
USER $robotology_install_user
WORKDIR $robotology_install_folder
RUN cd yarp && cd bindings &&  mkdir build && cd build && cmake .. -DCREATE_PYTHON=ON && make -j11

# Install YARP completion
RUN sudo ln -s /usr/local/share/bash-completion/completions/yarp /usr/share/bash-completion/completions

# Set environmental variables
USER $robotology_install_user
RUN echo "PS1='\[\e]0;\u \w\a\]\[\033[01;32m\]\u\[\033[00m\] \[\033[01;34m\]\w\[\033[00m\] \$ '" >> /home/$robotology_install_user/.bashrc
ENV PATH=$PATH:$YARP_DIR/bin
ENV DISPLAY=:1
ENV YARP_DATA_DIRS=$YARP_DIR/share/yarp
ENV LD_LIBRARY_PATH=$robotology_install_folder/yarp/build/lib/yarp/
ENV YARP_COLORED_OUTPUT=1
ENV QT_X11_NO_MITSHM=1
ENV PYTHONPATH=$PYTHONPATH:/home/user1/robotology/yarp/bindings/build/lib/python3/

# Manage yarp port
EXPOSE 10000/tcp 10000/udp

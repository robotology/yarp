Installing YARP on Linux, macOS or Windows with conda        {#install_yarp_conda}
=====================================================

[TOC]

Installation on Linux, macOS or Windows with conda           {#install_on_conda}
=========================

These instructions for installing yarp on any Linux distributions, macOS or Windows,
if you are using the conda package manager.


Install from binaries                             {#install_conda_from_binaries}
---------------------

To create a new environment called `yarpenv` with the `yarp` C++ library, tools and Python bindings, run:

~~~{.sh}
conda create -n yarpenv -c conda-forge yarp
~~~

If you are experiencing problems with [conda-forge](https://conda-forge.org/) binaries of yarp, please open an issue at 
[conda-forge/yarp-feedstock](https://github.com/conda-forge/yarp-feedstock) GitHub repo.

You are now ready to check you installation, see \ref check_your_installation.


Install from sources                               {#install_conda_from_sources}
--------------------

### Required Dependencies                             {#install_required_conda}


To create an environment called `yarpsrcdev` in which to compile yarp from source, run the following command

~~~{.sh}
conda create -n yarpsrcdev -c conda-forge cmake compilers pkg-config make ninja ycm-cmake-modules ace tinyxml eigen sdl sqlite libjpeg-turbo robot-testing-framework libpng libzlib soxr libopencv portaudio qt-main ffmpeg
~~~

### Compiling YARP                                             {#compiling_yarp}

To get the source you can download the latest release at
https://github.com/robotology/yarp/releases/
or clone YARP git repository from one of the following addresses:

\li(git read-only) git://github.com/robotology/yarp.git
\li(ssh read+write) git@github.com:robotology/yarp.git
\li(https read+write) https://github.com/robotology/yarp.git

For example:

~~~{.sh}
  git clone https://github.com/robotology/yarp.git
~~~

By default the `master` (development) branch is cloned, if you need a stable
branches they are named `yarp-<version>`. For example

~~~{.sh}
  git clone -b yarp-3.3 https://github.com/robotology/yarp.git
~~~


Generate Ninja build files using CMake in the environment you created before, and specify that you want to install
yarp in the same environment, by executing the following commands on Linux or macOS:

~~~{.sh}
  conda activate yarpsrcdev
  cd yarp
  mkdir build
  cd build
  cmake -GNinja -DCMAKE_INSTALL_PREFIX=$CONDA_PREFIX ..
~~~

or on Windows:

~~~{.sh}
  conda activate yarpsrcdev
  cd yarp
  mkdir build
  cd build
  cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%CONDA_PREFIX% ..
~~~

Usually you want to check in the CMake output that "Compile GUIs" and
"Compile YARP_math library" are enabled, if they are not, you are probably
missing some dependency.


Compile:

~~~{.sh}
ninja
ninja install
~~~

You are now ready to check you installation, see \ref check_your_installation.

\remark Do not select other options unless you know what you are doing.

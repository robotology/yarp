Installing YARP on GNU/Linux                               {#install_yarp_linux}
============================

[TOC]

Installation on GNU/Linux                                    {#install_on_linux}
=========================

These instructions are for Debian and its derivatives (Ubuntu, etc) GNU/Linux
distributions. For other distributions the installation from sources is mostly
the same, but you will need to figure out how to install the dependencies on
your system.


Install from binaries                             {#install_linux_from_binaries}
---------------------

Add www.icub.org to your sources.list.

On Ubuntu:

~~~{.sh}
sudo sh -c 'echo "deb http://www.icub.org/ubuntu focal contrib/science" > /etc/apt/sources.list.d/icub.list'
~~~

(replace `focal` with the code name of the installed version).


On Debian:

~~~{.sh}
sudo sh -c 'echo "deb http://www.icub.org/debian buster contrib/science" > /etc/apt/sources.list.d/icub.list'
~~~

(replace `buster` with the code name of the installed version).


Import the repository public key:

~~~{.sh}
sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-keys 57A5ACB6110576A6
~~~

Update the list of packages and install YARP.

~~~{.sh}
sudo apt-get update
sudo apt-get install yarp
~~~

You are now ready to check you installation, see \ref check_your_installation.





Install from sources                               {#install_linux_from_sources}
--------------------

### Required Dependencies                             {#install_required_debian}


Install the following packages to prepare your development environment and get
basic dependencies:

~~~{.sh}
sudo apt-get install build-essential git cmake cmake-curses-gui
~~~

(Please note that you need some extra step to install CMake 3.12 on Debian 9 and
Ubuntu 18.40, see
[robotology/QA#364](https://github.com/robotology/QA/issues/364)


Install [YCM](https://github.com/robotology/ycm/) from sources or install it
from binaries by configuring the icub.org repository (Follow the instructions at
\ref install_linux_from_binaries), and install it with

~~~{.sh}
sudo apt-get install ycm-cmake-modules
~~~

Depending on what parts of YARP you want to enable, you will also have to
install some other packages


For convenience's sake, all the required and most optional packages can be
installed with this command line, if you want to go into further details about
what is installed and why, proceed to the following sections.

~~~{.sh}
sudo apt-get install build-essential git cmake cmake-curses-gui \
  ycm-cmake-modules \
  libeigen3-dev \
  libace-dev \
  libedit-dev \
  libsqlite3-dev \
  libtinyxml-dev \
  qtbase5-dev qtdeclarative5-dev qtmultimedia5-dev \
  qml-module-qtquick2 qml-module-qtquick-window2 \
  qml-module-qtmultimedia qml-module-qtquick-dialogs \
  qml-module-qtquick-controls qml-module-qt-labs-folderlistmodel \
  qml-module-qt-labs-settings \
  libqcustomplot-dev \
  libgraphviz-dev \
  libjpeg-dev \
  libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
  gstreamer1.0-plugins-base \
  gstreamer1.0-plugins-good \
  gstreamer1.0-plugins-bad \
  gstreamer1.0-libav
~~~


### Recommended Dependencies                       {#install_recommended_debian}

Installing the following packages is recommended:

~~~{.sh}
sudo apt-get install \
  libeigen3-dev \
  libace-dev \
  libedit-dev \
  libsqlite3-dev \
  libtinyxml-dev
~~~

### Suggested Dependencies                           {#install_suggested_debian}

#### Qt5                                                   {#install_qt5_debian}

On Debian/Ubuntu the following Qt5 packages are required to build and run YARP
GUIs:

~~~{.sh}
sudo apt-get install qtbase5-dev qtdeclarative5-dev qtmultimedia5-dev \
  qml-module-qtquick2 qml-module-qtquick-window2 \
  qml-module-qtmultimedia qml-module-qtquick-dialogs \
  qml-module-qtquick-controls qml-module-qt-labs-folderlistmodel \
  qml-module-qt-labs-settings
~~~


#### QCustomPlot                                   {#install_qcustomplot_debian}

QCustomPlot is also optionally required for `yarpscope`:

~~~{.sh}
sudo apt-get install libqcustomplot-dev
~~~


### Graphviz                                          {#install_graphviz_debian}

Graphviz is required for `yarpviz`

~~~{.sh}
sudo apt-get install libgraphviz-dev
~~~


### Jpeg Library                                          {#install_jpeg_debian}

The jpeg library is required to build the `mjpeg` carrier

~~~{.sh}
sudo apt-get install libjpeg-dev
~~~


### GStreamer                                        {#install_gstreamer_debian}

GStreamer is required to enable the h264 carrier

~~~{.sh}
sudo apt-get install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
  gstreamer1.0-plugins-base \
  gstreamer1.0-plugins-good \
  gstreamer1.0-plugins-bad \
  gstreamer1.0-libav
~~~

See also \ref how_to_install_gstreamer


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


Generate makefiles using CMake:

~~~{.sh}
  cd yarp
  mkdir build
  cd build
  cmake ..
~~~

Usually you want to check in the CMake output that "Compile GUIs" and
"Compile YARP_math library" are enabled, if they are not, you are probably
missing some dependency.


Compile:

~~~{.sh}
 make
 sudo make install
 sudo ldconfig
~~~

You are now ready to check you installation, see \ref check_your_installation.

\remark YARP is installed with the RPATH enabled. This allows to install YARP in
another location by changing the `CMAKE_INSTALL_PREFIX` option (default is
`/usr/local`). If you install YARP in a system directories, or if you use the
`LD_LIBRARY_PATH` environment variable, you might want to disable the RPATH.
In order to do this, you have to enable the `CMAKE_SKIP_INSTALL_RPATH` option
in cmake.

\remark If YARP is not installed in a system location, you you will have to set
the `YARP_DIR` and `YARP_DATA_DIRS` environment variables to point to the
installation directory, see \ref set_up_your_environment_linux.

\remark Do not select other options unless you know what you are doing.


### Setup your environment                      {#set_up_your_environment_linux}

If you have installed YARP in default location you can skip to
\ref check_your_installation.

If you are using YARP without installing it or you have installed it in a custom
location, or you have disabled RPATH, you have to tweak your environment (edit
your `~/.bashrc` or similar) to:

* Set environment variables `YARP_DIR` to the location where you installed YARP
  (or CMake's build directory if you did not perform the installation step) to
  allow cmake to find YARP easily for your own projects.
  Set `YARP_DATA_DIRS` to `$YARP_DIR/share/yarp`.
* Add `YARP_DIR/bin` to `PATH` to allow your shell to find YARP programs.
* Add `YARP_DIR/lib` to `LD_LIBRARY_PATH` to allow YARP programs to find YARP
  libraries, if you enabled `CMAKE_SKIP_INSTALL_RPATH` and did not install in
  a system directory.

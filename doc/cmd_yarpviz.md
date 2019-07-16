yarpviz                                           {#yarpviz}
=======

The `yarpviz` is a graphical tool for profiling and visualizing YARP network!

![yarpviz](/images/yarpviz.png)


To be compiled, `YARP` has to be configured with `YARP_COMPILE_GUIS=ON` and `YARP_COMPILE_yarpviz=ON`

Dependencies
------------
* [Qt 5](https://www.qt.io/download/)
* [Graphviz](http://www.graphviz.org/)

### Ubuntu or Debian-based distribution

* Install graphviz and qt5
```
$ sudo apt-get install libgraphviz-dev qtbase5-dev qtdeclarative5-dev \
          qtdeclarative5-qtquick2-plugin qtdeclarative5-window-plugin \
          qtdeclarative5-controls-plugin qtdeclarative5-dialogs-plugin
```

### macOs

* Update brew
```
$ brew update
```

* Install Qt5
```
$ brew install qt
$ brew link --force qt
```

* Add the following lines to your .bash_profile
```
$ export Qt5_DIR=/usr/local/opt/qt5/lib/cmake
$ export PATH=/usr/local/opt/qt5/bin:$PATH
```

* Install Graphviz
```
$ brew install graphviz
```

### Windows

Download and install the library from (http://www.graphviz.org/Download_windows.php).

Set `GRAPHVIZ_ROOT` to point the installed directory (usually `C:\Program Files\Graphviz2.38` or `C:\Program Files (x86)\Graphviz2.38`).

Append `%GRAPHVIZ_ROOT%\bin` to your PATH variable.

fix_yarpviz_gridOnExport {#yarp_3_4}
-------------------

### Tools + libraries

#### `yarpviz` + `YARP_priv_qgvcore`

* Since, due to how the `QGVScene::drawBackground` function has been reimplemented, everytime a scene is exported as an image from `yarpviz` a white grid is added on top of the background, whether the user likes it or not, I added to `QGVScene` a new **bool** property named `gridNeeded` in order to change this behavior. If the property is *true*, the grid will be drawn as it is now. Otherwise the grid won't be added to the resulting image.
* The initial value of the property can be passed to the class constructor as a parameter (the default value of this parameter is *true* for backward compatibility sake) and then it can be modified by using `QGVScen::enableBgGrid` and checked via `QGVScene::bgGridEnabled`.
* In `yarpviz`menu a new voice has been added under `View`-> `Render options`. This checkable option labbelled `Background grid` allows to enabel/disable the background grid menitioned in the previous points.

* **PS:** since the icons resource file for the `Mainwindow` is called `res.qrc` but in the `Mainwindow.ui` file there are multiple references to a non-existent `ress.qrc` file, I changed also those references.

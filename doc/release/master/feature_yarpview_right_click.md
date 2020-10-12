feature_yarpview_right_click {#master}
---------------

### GUIs

#### `yarpview`

* Added the possibility to intercept the mouse right button click event.
* As for the mouse left button click event, there are two possibilities:
   * Single click (sends the current cursor coordinates to the specified output port)
   * Press, drag, and release (sends the coordinates of the `press` point and of the `release` one to the specified output port)
* To enable this feature the user has to specify a name for the output port that will send the cursor coordinates to the YARP network using the `--rightout` option with the desired output port name, e.g.
     ```
     yarpview --rightout /outRight
     ```
* If the ouput port has been specified, menu bar, under the voice `Image`, will have a new checkable menu item, i.e. `Intercept right click` (checked by default) that will allow the user to enable/disable the right click interception, if he/she erver needs to (obviously this option is not available in `minimal` or `compact` mode since the menu is not accessible).
* The line drawn while dragging with the right button will be green in order to easily distinguish it from the one drawn with the left button.
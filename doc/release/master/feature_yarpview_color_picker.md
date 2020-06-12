fix_yarpview_color_picker {#master}
---------------

### GUIs

#### `yarpview`

* Added a checkable menu item that, if checked, shows an additional line in the `status bar` that displays the color value of the pixel pointed by the mouse cursor.
  
  The string has the following format 
  ```
  Pixel("x","y") = "hexstring"
  ```
  Where  `x` and `y` are the coordinates of the pixel and `hexstring` is the hexadecimal (in the ARGB format) string representing the pixel color
  
  The additional line on the `status bar` contains also a little rectangle that will turn the same color of the currently selected pixel.
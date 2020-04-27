MapGrid2DOrigin {#master}
-----------------

### Libraries

#### `dev`

* The format of .map file loaded by `MapGrid2D` has been changed. It can now include the parameters `resolution <double>` and 
  `orientation <bottle>` to adjust the map reference frame.
  
* `MapGrid2DOrigin` can now handle a generic orientation. Previously only the trasnslational part was handled by methods `world2Cell` and `cell2World`

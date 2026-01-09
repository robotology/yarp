feature_python_bindings_navigation {#yarp_3_12}
-----------------------

## Bindings

### `dev`

#### `INavigation2D`, `ILocalization2D`, `IMap2D`

* Added bindings for `yarp::dev::INavigation2D`, `yarp::dev::IMap2D`, `yarp::dev::ILocalization2D` interfaces
* Issue with bindings on and YARP_NO_MATH defined fixed (it was affecting the newly added bindings as ell as thealready present `yarp::dev::IFrameTransform` ones)

fakeLaser_context {#master}
-----------------------

### devices
* Device `fakeLaser` now supports the `map_context` parameter to load a test map from a specific context. e.g.
`yarpdev --device Rangefinder2DWrapper --subdevice fakeLaser --period 10 --name /fakeLaser:o --test use_mapfile --map_context context --map_file mymap.map`




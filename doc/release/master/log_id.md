log_id {#master}
------

## Libraries

### `os`

#### `Log`

* Added the new family of logging macros with "Id" (`yIDebug`).
  See [documentation](\ref yarp_logging).

### `dev`

#### `DeviceDriver`

* Added `id()` and `setId()` methods

#### `PolyDriver`

* The device id can be set passing the `id` option to the `open` method (or
  passing `--id` to `yarpdev`).

## Devices

### `portaudio*`,

* 'id' parameter was renamed to 'dev_id'.

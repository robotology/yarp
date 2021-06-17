fix_topIsLow {#master}
------------

# Important Changes

* Bottom to top images are no longer flipped when sent through the network. This
  means that the image needs to be manually flipped after it is received.

* Bottom to top images might cause issues when received by YARP 3.4.5 or
  earlier.


# Bugfix


## Libraries

### `sig`

#### `Image`

* The `topIsLow` flag is now properly handled, kept in sync with the IplImage,
  and forwarded through the network.

## Devices

### `grabberDual`

* The `topIsLow` flag is no longer changed.

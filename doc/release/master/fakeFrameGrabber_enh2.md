fakeFrameGrabber_enh2 {#master}
---------------------

## Devices

### `fakeFrameGrabber`

* The `getImage()` method no longer blocks the caller.
  The `--syncro` option was added to restore the old behavior.

* The `topIsLow` option and the `set_topIsLow` rpc command were added to produce
  images with bottom to top scanlines.

* The `nois` mode (introduced in fakeFrameGrabber_enh) was removed in favour of
  the `--noise` option and `set_noise` and `set_snr` commands that adds noise
  to the generated images.

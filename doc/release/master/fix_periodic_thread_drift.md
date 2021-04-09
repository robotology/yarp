fix_periodic_thread_drift {#master}
-----------

### Libraries

#### `os`

##### `PeriodicThread`

* The constructor now accepts a third (optional) parameter, `clockAccuracy`,
  which enables drift compensation using an absolute time reference to ensure
  all steps trigger at precise intervals, thus avoiding error accumulation over
  time. Set it to `PeriodicThreadClock::Absolute` to enable the new behavior,
  default is `PeriodicThreadClock::Relative` (old behavior). Beware that, in
  absolute mode, starvation may occur if two busy threads share a common
  resource. Another constructor has been added for ease of usage:
  `PeriodicThread(double period, PeriodicThreadClock clockAccuracy)`. (#2488)
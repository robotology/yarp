deprecate_Mutex {devel}
---------------

### os

* All `yarp::os::Mutex` related classes and methods are now deprecated in favour
  of `std::mutex`:
  * Classes:
    * `yarp::os::Mutex`
    * `yarp::os::RecursiveMutex`
    * `yarp::os::AbstractLockGuard`
    * `yarp::os::LockGuard`
    * `yarp::os::RecursiveLockGuard`
  * Methods:
    * `yarp::os::Contactable::setCallbackLock(yarp::os::Mutex*)`
    * `yarp::os::AbstractContactable::setCallbackLock(yarp::os::Mutex*)`
    * `yarp::os::Port::setCallbackLock(yarp::os::Mutex*)`
    * `yarp::os::Buffered::setCallbackLock(yarp::os::Mutex*)`
    * `yarp::os::Timer::Timer(..., yarp::os::Mutex*)`

yarploggerfix_ap5_ap6 {#master}
-----------

Important Changes
-----------------

### yarplogger

* added option '--unlimited_size' which removes all limits on the log size. To be used with caution!
* fixed some bugs related to how the limits are applied to new logs added to an on-going session
* saving/loading log routines moved YarpLoggerFiles.cpp
* fixed issue related to saving/loading files containing empty strings.
* reduced size of mutex-proteceted critical section in logger_thread::run()
* added a counter which return from the logger_thread::run() every xxx processed events.
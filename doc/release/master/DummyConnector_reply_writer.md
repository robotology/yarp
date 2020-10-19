DummyConnector_reply_writer {#master}
---------------------------


### Libraries

#### os

##### `yarp::os::DummyConnector`

* The `getReader` methods now accepts a `ConnectionWriter` as optional
  parameter. This writer is returned by `getWriter` is called on the
  `ConnectionReader` returned by the writer, and it is therefore used for the
  replies in `Portable`s and in a few other cases.

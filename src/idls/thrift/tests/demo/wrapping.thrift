
struct WrapValue
{
  1: list<double> content;
} 
(
  yarp.name = "yarp::os::Value"
  yarp.includefile="yarp/os/Value.h"
)

service Wrapping {
  i32 check(1:WrapValue param);	
}

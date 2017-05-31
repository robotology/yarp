
struct WrapValue
{
} 
(
  yarp.name = "yarp::os::Value"
  yarp.includefile="yarp/os/Value.h"
)

struct WrapBottle
{
} 
(
  yarp.name = "yarp::os::Bottle"
  yarp.includefile="yarp/os/Bottle.h"
)

service Wrapping {
  i32 check(1:WrapValue param);
  WrapBottle getBottle();
}

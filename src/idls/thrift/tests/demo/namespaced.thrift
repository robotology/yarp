
namespace yarp DemoWithNamespace


enum NSDemoEnum {
  ENUM1 = 1,
  ENUM2 = 2
}

struct NSDemoStruct {
  1: i32 x,
  2: i32 y
}

service NSDemo {
  i32 get_answer();
}

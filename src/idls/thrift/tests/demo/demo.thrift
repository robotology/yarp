#demo.thrift

enum DemoEnum {
  ENUM1 = 1,
  ENUM2 = 2,
  ENUM3 = 3
}

service Demo {
  i32 get_answer();
  bool set_answer(1:i32 rightAnswer)
  i32 add_one(1:i32 x);
  bool start();
  bool stop();
  bool is_running();

  void test_void(1:i32 x);
  oneway void test_1way(1:i32 x);

  DemoEnum test_enums(1:DemoEnum x);
  list<DemoEnum> test_enum_vector(1:list<DemoEnum> x);

  bool test_defaults(1:i32 x = 42);
}

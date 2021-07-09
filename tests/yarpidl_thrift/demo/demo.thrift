/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#demo.thrift

/**
*   Demo Thrift
*/

/// @cond


/**
 * Documentation for enumerator
 */
enum DemoEnum {
  /** thing 1 */
  ENUM1 = 1,
  /** thing 2 */
  ENUM2 = 2,
  ENUM3 = 3
}

/**
 * Documentation for structure
 */
struct DemoStruct {
  /** this is the x part */
  1: i32 x = 0,

  /** this is the y part */
  2: i32 y
}

struct DemoStructList {
  1: list<DemoStruct> lst
}

struct DemoStructMap {
  1: map<string, DemoStruct> mapValues;
}

struct DemoStructExt {
  /** this is the x part */
  1: i32 x = 0,

  /** this is the y part */
  2: i32 y = 0,

  /** this is a list of ints */
  3: list<i32> int_list,

  /** this is a list of structs */
  4: list<DemoStruct> ds_list
}

struct TestSomeMoreTypes {
  1: bool a_bool,
  2: i8 a_i8,
  3: i16 a_i16,
  4: i32 a_i32,
  5: i64 a_i64,
  6: double a_double,
  7: string a_string,
  8: binary a_binary
}

/**
 * Documentation for service
 */
service Demo {
  /** This function gets the answer */
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
  i32 add_pair(1:i32 x,2:i32 y);
  i32 test_partial(1:i32 x, 2:list<i32> lst, 3:i32 y);

  i32 test_tail_defaults(1:DemoEnum x = DemoEnum.ENUM1);

  /**
  * Test longer tail defaults
  */
  i32 test_longer_tail_defaults(1: i32 ignore, 2:DemoEnum _enum = DemoEnum.ENUM2, 3:i32 _int = 42, 4:string _string = "Space Monkey from the Planet: Space");

  oneway void do_start_a_service();
  bool do_check_for_service();
  void do_stop_a_service();

  DemoStruct get_demo_struct();

  DemoStructMap get_demo_map_struct();

  bool this_is_a_const_method() (yarp.qualifier = "const");
}

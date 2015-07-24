// This is an automatically-generated file.
// It could get re-generated if the ALLOW_IDL_GENERATION flag is on.

#ifndef YARP_THRIFT_GENERATOR_Demo
#define YARP_THRIFT_GENERATOR_Demo

#include <yarp/os/Wire.h>
#include <yarp/os/idl/WireTypes.h>
#include <DemoEnum.h>

class Demo;


/**
 * Documentation for service
 */
class Demo : public yarp::os::Wire {
public:
  Demo();
  /**
   * This function gets the answer
   */
  virtual int32_t get_answer();
  virtual bool set_answer(const int32_t rightAnswer);
  virtual int32_t add_one(const int32_t x);
  virtual bool start();
  virtual bool stop();
  virtual bool is_running();
  virtual void test_void(const int32_t x);
  virtual void test_1way(const int32_t x);
  virtual DemoEnum test_enums(const DemoEnum x);
  virtual std::vector<DemoEnum>  test_enum_vector(const std::vector<DemoEnum> & x);
  virtual bool test_defaults(const int32_t x = 42);
  virtual int32_t add_pair(const int32_t x, const int32_t y);
  virtual int32_t test_partial(const int32_t x, const std::vector<int32_t> & lst, const int32_t y);
  virtual int32_t test_tail_defaults(const DemoEnum x = ENUM1);
  /**
   * Test longer tail defaults
   */
  virtual int32_t test_longer_tail_defaults(const int32_t ignore, const DemoEnum _enum = ENUM2, const int32_t _int = 42, const std::string& _string = "Space Monkey from the Planet: Space");
  virtual void do_start_a_service();
  virtual bool do_check_for_service();
  virtual void do_stop_a_service();
  virtual bool read(yarp::os::ConnectionReader& connection);
  virtual std::vector<std::string> help(const std::string& functionName="--all");
};

#endif


#ifndef LOGIN_INC
#define LOGIN_INC

#include <stdio.h>
#include "Thing.h"

class Login {
public:
  Login();
  bool apply(const char *name, const char *key);
  void shutdown();
  ID getID() {
    return base_id;
  }
  Thing& getThing();

private:
  ID base_id;
};

#endif

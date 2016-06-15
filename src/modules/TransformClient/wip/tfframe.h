#include <string>
#include <iostream>
#include <iomanip>
#include <yarp/os/Log.h>
#include "tf.h"

class TfFrame
{
    std::string           name;
    std::string           tfToParent;
    TfFrame*              parent;
    std::vector<TfFrame*> children;
    bool                  valid;

public:
                          TfFrame();
                          ~TfFrame();
    std::string           getParentTf()                        { return     tfToParent; };
    TfFrame*              getParent()                          { return     parent;     };
    std::vector<TfFrame*> getChildren()                        { return     children;   };
    std::string           getName()                            { return     name;       };
    bool                  isValid()                            { return     valid;      };
    void                  setParentTf(const std::string& tf)   { tfToParent = tf;       };
    void                  setParent(TfFrame& frame)            { parent     = &frame;   };
    void                  setName(const std::string& newName)  { name       = newName;  };
    bool                  setName(char* newName);
    bool                  addChild(TfFrame& frame);

};
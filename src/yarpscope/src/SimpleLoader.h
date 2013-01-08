/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#ifndef YARPSCOPE_SIMPLELOADER_H
#define YARPSCOPE_SIMPLELOADER_H

namespace yarp {
namespace os {
class Property;
}
}

namespace YarpScope
{

class SimpleLoader
{
public:
    explicit SimpleLoader(/* FIXME const */ yarp::os::Property &options, bool *ok);
    virtual ~SimpleLoader();

}; // class SimpleLoader

} // namespace YarpScope

#endif // YARPSCOPE_SIMPLELOADER_H

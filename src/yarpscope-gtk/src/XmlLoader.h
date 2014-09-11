/*
 * Copyright (C) 2012  iCub Facility, Istituto Italiano di Tecnologia
 * Author: Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef YARPSCOPE_XMLLOADER_H
#define YARPSCOPE_XMLLOADER_H

namespace Glib {
class ustring;
}

namespace YarpScope
{

class XmlLoader
{
public:
    explicit XmlLoader(const Glib::ustring &filename);
    virtual ~XmlLoader();

}; // class XmlLoader

} // namespace YarpScope


#endif // YARPSCOPE_XMLLOADER_H

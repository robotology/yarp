/*
 * Copyright (C) 2006-2019 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include <yarp/os/Name.h>

using namespace yarp::os;

Name::Name(const std::string& txt)
{
    this->txt = txt;
}

bool Name::isRooted() const
{
    if (txt.length() >= 1) {
        if (txt[0] == '/') {
            return true;
        }
    }
    return false;
}


Contact Name::toAddress() const
{
    size_t mid = txt.find(":/");
    if (mid != std::string::npos && mid > 0) {
        std::string first = txt.substr(0, mid);
        std::string second = txt.substr(mid + 2);
        if (first.length() >= 2) {
            if (first[0] == '/') {
                first = first.substr(1);
            }
        }
        return Contact(second, first, "", -1);
    }
    return Contact(txt, "", "", -1);
}


std::string Name::getCarrierModifier(const char* mod, bool* hasModifier)
{
    bool ok = false;
    std::string work = txt;
    size_t mid = work.find(":/");
    if (mid != std::string::npos && mid > 0) {
        work = work.substr(0, mid);
        std::string target = std::string("+") + mod + ".";
        size_t modLoc = work.find(target);
        if (modLoc != std::string::npos) {
            work = work.substr(modLoc + target.length(), work.length());
            size_t endLoc = work.find('+');
            if (endLoc != std::string::npos) {
                work = work.substr(0, endLoc);
            }
            ok = true;
        }
    }
    if (hasModifier != nullptr) {
        *hasModifier = ok;
    }
    return ok ? work : "";
}

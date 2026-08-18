/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ControlBoardRemapper.h"
#include "ControlBoardRemapperHelpers.h"
#include "ControlBoardRemapperLogComponent.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <mutex>
#include <cassert>

using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;

bool ControlBoardRemapper::close()
{
    return detachAll();
}

bool ControlBoardRemapper::open(Searchable& config)
{
    // From ControlBoardRemapper_ParamsParser
    bool parseok = parseParams(config);

    this->setVerbose(m_verbose);

    if (m_axesNames.size() == 0)
    {
        yCError(CONTROLBOARDREMAPPER, "Invalid axes name");
        return false;
    }

    this->setAxesNames(m_axesNames);
    this->setNrOfControlledAxes(m_axesNames.size());

    return true;
}

bool ControlBoardRemapper::attachAll(const PolyDriverList &polylist)
{
    return ControlBoardRemappingEngine::attachAllDevices(polylist);
}

bool ControlBoardRemapper::detachAll()
{
    return ControlBoardRemappingEngine::detachAllDevices();
}

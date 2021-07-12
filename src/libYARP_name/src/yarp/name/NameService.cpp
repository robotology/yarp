/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/name/NameService.h>
#include <yarp/name/NameServerConnectionHandler.h>
#include <yarp/os/DummyConnector.h>

using namespace yarp::name;
using namespace yarp::os;

bool NameService::process(yarp::os::PortWriter& in,
                          yarp::os::PortReader& out,
                          const yarp::os::Contact& /*remote*/)
{
    DummyConnector din;
    DummyConnector dout;
    din.setTextMode(true);
    dout.setTextMode(true);
    in.write(din.getWriter());
    NameServerConnectionHandler handler(this);
    bool ok = handler.apply(din.getReader(), &dout.getWriter(), false);
    out.read(dout.getReader());
    return ok;
}

-- SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
-- SPDX-License-Identifier: BSD-3-Clause

-- loading lua-yarp binding library
require("yarp")

--
-- create is called when the port monitor is created
-- @return Boolean
--
PortMonitor.create = function()
    prev_rcv_time = nil
    prev_monitor_time = nil
    frame_jitter = 0
    frame_count = 0
    admin_port = yarp.Port()
    admin_port:setAdminMode(true)
    local ret = admin_port:open("...")
    ret = ret and yarp.NetworkBase_connect(admin_port:getName(), "/grabber")
    if ret == true then
        print("QOS monitor started!")
        return true
    else
        print("Failed to start QOS monitor!")
        return false
    end
end



--
-- destroy is called when port monitor is destroyed
--
PortMonitor.destroy = function()
    yarp.NetworkBase_disconnect(admin_port:getName(), "/grabber")
    admin_port:close()
    print("QOS monitor finished!")
end


--
-- accept is called when the port receives new data
-- @param thing The Things abstract data type
-- @return Boolean
-- if false is returned, the data will be ignored
-- and update() will never be called
PortMonitor.accept = function(thing)
    if prev_rcv_time == nil then
        prev_rcv_time = yarp.Time_now()
        prev_monitor_time = yarp.Time_now()
        return true
    end

    frame_jitter =  frame_jitter + (yarp.Time_now() - prev_rcv_time)
    frame_count = frame_count + 1;
    prev_rcv_time = yarp.Time_now()

    if (yarp.Time_now() - prev_monitor_time) > 5.0 then
        prev_monitor_time = yarp.Time_now()
        local frame_rate =  1.0 / (frame_jitter/frame_count)
        frame_jitter = 0
        frame_count = 0
        print("Frame rate:", frame_rate)
        if frame_rate < 28.0 then
            print(string.format("QOS low! frame rate = %d", frame_rate))
            local cmd = yarp.Bottle()
            cmd:fromString("prop set /view (qos ((tos 16)))")
            print(cmd:toString())
            admin_port:write(cmd)
            cmd:clear()
            cmd:fromString("prop set /view (sched ((priority 10) (policy 1)))")
            print(cmd:toString())
            admin_port:write(cmd)
        end
    end
    return true
end

-- SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
-- SPDX-License-Identifier: BSD-3-Clause

-- loading lua-yarp binding library
require("yarp")


--
-- Auxiliary functions
--

-- save the log data into a file
function save_log()
    if param_log_name == nil or param_log_name == '' then
        param_log_name = param_log_path.."/"..string.gsub(sourceName,"/","_").."."..string.gsub(destinationName,"/","_")..".log"
    end
    print("\n[Rate_Log] Saving to "..param_log_name.."\n")
    local file = io.open(param_log_name, "w")
    if file == nil then
        print("\n[Rate_Log] Cannot open", param_log_name.."\n")
    return
    end
    -- save all the loged data
    if param_log_raw == true then
        for i=1,#log_data do
            file:write(log_data[i].time," ",log_data[i].value,"\n")
        end
    else
        local avg = sum / count
        file:write("Source     : "..sourceName.."\n")
        file:write("Destination: "..destinationName.."\n")
        file:write("Samples    : "..count.."\n")
        file:write("Mean       : "..avg.."\n")
        file:write("Min        : "..min.."\n")
        file:write("Max        : "..max.."\n")
    end
    file:close()
end


--
-- create is called when the port monitor is created
-- @return Boolean
--
PortMonitor.create = function(options)
    sourceName = options:find("source"):asString()
    if sourceName == nil or sourceName == '' then
        sourceName = "port"
    end
    destinationName = options:find("destination"):asString()
--    local carrier = options:find("carrier"):asString()
--    print("carrier:", carrier)
--    print("opetions:", options:toString())
--    for attr in string.gmatch(carrier,'([^+]+)') do
--        k, v = attr:match("([^.]+).([^.]+)")
--        print(k,v)
--    end

    -- log parameters
    param_log_raw = true
    param_log_start = true
    param_log_path = os.getenv("HOME")
    param_log_name = ""

    -- log variables
    log_data = {}
    prev_time = 0
    sum, min, max, count = 0, 0, 0 ,0
    print("\n[Rate_Log] Ready!\n")
    if param_log_start == true then
        print("\n[Rate_Log] Start logging...\n")
    end

    return true;
end

--
-- destroy is called when port monitor is destroyed
--
PortMonitor.destroy = function()
    if param_log_start == false then return true end
    --save_log()
end


--
-- accept is called when the port receives new data
-- @param thing The Things abstract data type
-- @return Boolean
-- if false is returned, the data will be ignored
PortMonitor.accept = function(thing)
    if param_log_start == false then return true end

    if prev_time ~= 0 then
        t = yarp.Time_now() - prev_time
        count = count + 1
        sum = sum + t
        if max < t then max = t end
        if min == 0 or min > t then min = t end
        if param_log_raw == true then
            log_data[#log_data+1] =  {time=yarp.Time_now(), value=t}
        end
    end
    prev_time = yarp.Time_now()
    return true
end

--
-- getparan is called on getCarrierParams by the port administrator
-- @return property The Property
--
PortMonitor.setparam = function(property)
    if property:check("log_raw") then
        param_log_raw = (property:find("log_raw"):asInt32() == 1)
        if param_log_raw == true then
            print("\n[Rate_Log] Logging all samples!\n")
        else
            print("\n[Rate_Log] Logging only statistics!\n")
        end
    end

    if property:check("log_name") then
        param_log_name = property:find("log_name"):asString()
    end

    if property:check("log_path") then
        param_log_path = property:find("log_path"):asString()
        print('log_path', param_log_path)
    end

    if property:check("log_start") then
        param_log_start = (property:find("log_start"):asInt32() == 1)
        if param_log_start == true then
            print("\n[Rate_Log] Start logging...\n")
        else
            print("\n[Rate_Log] Stop logging...\n")
        end
    end
    if property:check("log_save") then
        param_log_start = false
        save_log()
    end
end

PortMonitor.getparam = function()
    prop = yarp.Property()
    if param_log_raw == true then
        prop:put("log_raw", 1)
    else
        prop:put("log_raw", 0)
    end
    prop:put("log_name", param_log_name)
    if param_log_start == true then
        prop:put("log_start", 1)
    else
        prop:put("log_start", 0)
    end
    bt = yarp.Bottle()
    data = bt:addList()
    for i=1,#log_data do
        v = data:addList()
        v:addDouble(log_data[i].time)
        v:addDouble(log_data[i].value)
    end
    prop:put("data", bt:get(0))
    return prop
end

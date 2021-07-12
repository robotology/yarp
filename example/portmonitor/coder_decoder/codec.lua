-- SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
-- SPDX-License-Identifier: BSD-3-Clause

-- Using Lua 5.1+ base64 v3.0 (c) 2009 by Alex Kloss <alexthkloss@web.de>

-- loading lua-yarp binding library
require("yarp")

--
-- PortMonitor table is used by portmonitor_carrier
-- to invoke the corresponding methods.The methods are
-- optional but must satisfy the following format:
--
--  PortMonitor.create = function(options) ... return true end,
--  PortMonitor.destroy = function() ... end,
--  PortMonitor.accept = function(thing) ... return true end,
--  PortMonitor.update = function(thing) ... return thing end,
--  PortMonitor.setparam = function(param) ... end,
--  PortMonitor.getparam = function() ... return param end
--  PortMonitor.trig = function() ... return end
--

--
-- create is called when the port monitor is created
-- @return Boolean
--
PortMonitor.create = function(options)
    isCoder = (options:find("sender_side"):asInt32() == 1)
    if isCoder == true then
        print("codec.lua: I will encode whatever i get!")
    else
        print("codec.lua: I will decode whatever i get!")
    end
    return true;
end


--
-- accept is called when the port receives new data
-- @param thing The Things abstract data type
-- @return Boolean
-- if false is returned, the data will be ignored
-- and update() will never be called
PortMonitor.accept = function(thing)
    if thing:asBottle() == nil then
        print("codec.lua: got wrong data type (expected type Bottle)")
        return false
    end
    return true
end



--
-- update is called when the port receives new data
-- @param thing The Things abstract data type
-- @return Things
PortMonitor.update = function(thing)
    bt = thing:asBottle()
    data = bt:toString()
    bt:clear()
    if isCoder == true then
        bt:fromString(enc(data))
    else
        bt:fromString(dec(data))
    end
    return thing
end


-- character table string
local b='ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/'

-- encoding
function enc(data)
    return ((data:gsub('.', function(x)
        local r,b='',x:byte()
        for i=8,1,-1 do r=r..(b%2^i-b%2^(i-1)>0 and '1' or '0') end
        return r;
    end)..'0000'):gsub('%d%d%d?%d?%d?%d?', function(x)
        if (#x < 6) then return '' end
        local c=0
        for i=1,6 do c=c+(x:sub(i,i)=='1' and 2^(6-i) or 0) end
        return b:sub(c+1,c+1)
    end)..({ '', '==', '=' })[#data%3+1])
end

-- decoding
function dec(data)
    data = string.gsub(data, '[^'..b..'=]', '')
    return (data:gsub('.', function(x)
        if (x == '=') then return '' end
        local r,f='',(b:find(x)-1)
        for i=6,1,-1 do r=r..(f%2^i-f%2^(i-1)>0 and '1' or '0') end
        return r;
    end):gsub('%d%d%d?%d?%d?%d?%d?%d?', function(x)
        if (#x ~= 8) then return '' end
        local c=0
        for i=1,8 do c=c+(x:sub(i,i)=='1' and 2^(8-i) or 0) end
        return string.char(c)
    end))
end

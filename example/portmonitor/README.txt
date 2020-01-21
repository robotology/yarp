
Port Monitor carrier
====================

- What is the port monitor?

  It is a run-time monitor object implemented as a new carrier which
  allows to modify incoming (in future also outgoing) data to a port
  using user script. Currently only Lua is supported by the port monitor.


- What do I need to use port monitor?

  You need to compile yarp-lua binding and set the 'LUA_CPATH' to find
  yarp-lua binding library.


- How can I use port monitor?

  You should connect two ports using yarp 'portmonitor' carrier modifier:

  $ yarp connect /out /in tcp+recv.portmonitor+script.lua+context.myapp+file.my_lua_script

  'script.lua' tells the carrier to create a port monitor object for Lua.
  'context.myapp' tells the resource finder to load the script from the 'myapp' context.
  'file.my_lua_script' indicates 'my_lua_script' should be loaded by monitor object.
  'my_lua_script' is located using standard yarp Resource Finder policy. The postfix
  (e.g., '.lua') is not necessary.

  When data arrive to an input port, the port monitor will call the corresponding
  callback function (i.e., PortMonitor.update) from lua script and passes an instance of
  Things object to the callback function. Using the Things object, user's script can
  access the data, modify it and return it to the port monitor object. Beside the
  'PortMonitor.update' The following callbacks can be  implemented within the global
  'PortMonitor' table:

   PortMonitor.create
   ------------------
   This is called when the script is loaded and the port monitor object is created.
   Returning 'false' will avoid calling other functions and stop the monitor object.
   The 'options' is of type yarp.Property object and it consists of a set of property/value
   provides some information about the current connection which the PortMonitor object is attached.
   such as "sender_side" or "receiver_side" keys in the options list respectively indicate whether
   the port monitor object is attached to the sender side or the receiver side of the connection.
   e.g., options:find("sender_side"):asBool()

   PortMonitor.create = function(options)
        ...
        return true     --default
   end


   PortMonitor.destroy
   -------------------
   This is called when the monitor object is destroying (e.g., on disconnect)

   PortMonitor.destroy = function()
        ...
   end


   PortMonitor.accept
   ------------------
   This is called when a new data arrives to the port. User can access the data and
   check whether it should accept or discard it. Returning 'false' will discard delivering
   data to the port and avoids calling PortMonitor.update().

   PortMonitor.accept = function(thing)
        ...
        return true     --default
   end


   PortMonitor.update
   ------------------
   This will be called if the data is accepted by PortMonitor.update(). User can modify and
   return it using 'thing' object.

   PortMonitor.update = function(thing)
        ...
        return thing
   end


   PortMonitor.setparam/getparam
   -----------------------------
   This will be called by the yarp port administrator when users try to reconfigure the monitor
   object using yarp port administrative commands (See example/image_modification/README.txt).
   The 'param' is of type yarp.Property object.

   PortMonitor.setparam = function(param)
        ...
   end

   PortMonitor.getparam = function()
        ...
        return param
   end

   PortMonitor.trig
   ----------------
   This will be called when one of the peer connections to the same input port receives data.

   PortMonitor.trig = function()
        ...
   end

  Beside the port monitor callbacks, there is a set of auxiliary functions which is offered by the
  PortMonitor. These auxiliary functions are used with the PortMonitor to arbitrator multiple
  connection to the same input port of a module. (See example/arbitration/README.txt)

    - PortMonitor.setEvent(event, lifetime) : set an event (with optional lifetime) into port event record
    - PortMonitor.unsetEvent(event)         : unset an event into port event record
    - PortMonitor.setConstraint(rule)       : set the selection rule
    - PortMonitor.getConstraint()           : get the selection rule


  Port monitor carrier looks for the global table name 'PortMonitor' in the user script and calls its
  corresponding functions if exist. Notice that the PortMonitor is a global variable and should not be
  altered or assigned to nil.

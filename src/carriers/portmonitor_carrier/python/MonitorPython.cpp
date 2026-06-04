/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

#include "MonitorPython.h"
#include "MonitorLogComponent.h"

#include "swigpythonrun.h"

#include <filesystem>

using namespace yarp::os;

namespace {
YARP_LOG_COMPONENT(PY_PORT_MONITOR, "yarp.portmonitor.MonitorPython")

constexpr const char* yarpThingsSwigType = "yarp::os::Things *";
constexpr const char* yarpPortWriterSwigType = "yarp::os::PortWriter *";
constexpr const char* yarpPropertySwigType = "yarp::os::Property *";

bool appendPythonPath(const std::string& path)
{
    if (path.empty()) {
        return true;
    }

    PyObject* sysPath = PySys_GetObject((char*)"path"); // Borrowed reference
    if (sysPath == nullptr || !PyList_Check(sysPath)) {
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        yCError(PY_PORT_MONITOR) << "Unable to access Python sys.path";
        return false;
    }

    PyObject* pyPath = PyUnicode_FromString(path.c_str());
    if (pyPath == nullptr) {
        PyErr_Print();
        return false;
    }

    const int contains = PySequence_Contains(sysPath, pyPath);
    if (contains < 0) {
        Py_DECREF(pyPath);
        PyErr_Print();
        return false;
    }

    if (contains == 0 && PyList_Append(sysPath, pyPath) != 0) {
        Py_DECREF(pyPath);
        PyErr_Print();
        return false;
    }

    Py_DECREF(pyPath);
    return true;
}
}


/**
 * Class MonitorPython
 */
MonitorPython::MonitorPython() : bHasAcceptCallback(false),
                           bHasUpdateCallback(false),
                           bHasUpdateReplyCallback(false),
                           trigger(nullptr)
{
    // Initialize the Python Interpreter
    if (!Py_IsInitialized())
    {
        yInfo() << "Calling Py_Initialize from open()";
        yInfo() << "Python version: " << Py_GetVersion();

        Py_Initialize();
    }
}

MonitorPython::~MonitorPython()
{
    Py_XDECREF(m_lastUpdateResult);
}

bool MonitorPython::load(const Property &options)
{
    std::lock_guard<std::recursive_mutex> guard(m_monitor_mutex);

    const std::filesystem::path scriptPath(options.find("filename").asString());
    if (scriptPath.empty()) {
        yCError(PY_PORT_MONITOR) << "Python monitor script filename is empty";
        return false;
    }

    m_path = scriptPath.has_parent_path() ? scriptPath.parent_path().string() : std::string();
    m_pythonScriptName = scriptPath.stem().string();
    if (m_pythonScriptName.empty()) {
        yCError(PY_PORT_MONITOR) << "Unable to determine Python module name from" << scriptPath.string();
        return false;
    }

    if (!appendPythonPath(m_path)) {
        return false;
    }

    if(!loadPythonModule(m_pythonScriptName, m_pModule)) {
        return false;
    }

    bHasAcceptCallback = hasPythonFunction("acceptData");
    bHasUpdateCallback = hasPythonFunction("updateData");
    bHasUpdateReplyCallback = hasPythonFunction("updateReply");
    if(hasPythonFunction("create"))
    {
        PyObject* pValue = nullptr;
        PyObject* pArgs = nullptr;
        if (!functionWrapper("create", pArgs, pValue))
        {
            yCError(PY_PORT_MONITOR) << "Unable to call the create function from python \n";
            return false;
        }
        Py_XDECREF(pValue);
    }

    return ensureYarpModuleLoaded();
}

bool MonitorPython::acceptData(Things &thing)
{
    std::lock_guard<std::recursive_mutex> guard(m_monitor_mutex);
    PyGILState_STATE gstate = PyGILState_Ensure();
    yDebug() << "acceptData called";
    if (!bHasAcceptCallback) {
        PyGILState_Release(gstate);
        return true;
    }
    if (!ensureYarpModuleLoaded()) {
        PyGILState_Release(gstate);
        return false;
    }
    swig_type_info* type = SWIG_TypeQuery(yarpThingsSwigType);
    if (type == nullptr) {
        yCError(PY_PORT_MONITOR) << "Swig type of Things is not found";
        PyGILState_Release(gstate);
        return false;
    }
    PyObject* pyThing = SWIG_NewPointerObj((void*)&thing, type, 0);
    if (pyThing == nullptr) {
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        PyGILState_Release(gstate);
        return false;
    }
    PyObject* pArgs = PyTuple_New(1);
    if (pArgs == nullptr) {
        Py_DECREF(pyThing);
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        PyGILState_Release(gstate);
        return false;
    }
    if (PyTuple_SetItem(pArgs, 0, pyThing) != 0) {
        Py_DECREF(pyThing);
        Py_DECREF(pArgs);
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        PyGILState_Release(gstate);
        return false;
    }
    PyObject* pValue = nullptr;
    if (!functionWrapper("acceptData", pArgs, pValue)) {
        yCError(PY_PORT_MONITOR) << "Unable to call the acceptData function from python";
        Py_DECREF(pArgs);
        PyGILState_Release(gstate);
        return false;
    }
    Py_DECREF(pArgs);
    if (pValue == nullptr) {
        PyGILState_Release(gstate);
        return false;
    }
    const int accepted = PyObject_IsTrue(pValue);
    Py_DECREF(pValue);
    if (accepted < 0) {
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        PyGILState_Release(gstate);
        return false;
    }
    return accepted != 0;
}


yarp::os::Things& MonitorPython::updateData(yarp::os::Things& thing)
{
    std::lock_guard<std::recursive_mutex> guard(m_monitor_mutex);
    PyGILState_STATE gstate = PyGILState_Ensure();

    if (!ensureYarpModuleLoaded()) {
        PyGILState_Release(gstate);
        return thing;
    }

    swig_type_info* type = SWIG_TypeQuery(yarpThingsSwigType);
    if (type == nullptr) {
        yCError(PY_PORT_MONITOR) << "Swig type of Things is not found";
        PyGILState_Release(gstate);
        return thing;
    }

    PyObject* pyThing = SWIG_NewPointerObj((void*)&thing, type, 0);
    if (pyThing == nullptr) {
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        PyGILState_Release(gstate);
        return thing;
    }

    PyObject* pArgs = PyTuple_Pack(1, pyThing);
    Py_DECREF(pyThing);
    if (pArgs == nullptr) {
        PyErr_Print();
        PyGILState_Release(gstate);
        return thing;
    }

    PyObject* pRetValue = nullptr;
    std::string func_name = "updateData";
    yDebug() << "Calling " << func_name << " function from python \n";
    if (!functionWrapper(func_name, pArgs, pRetValue))
    {
        yError() << "Unable to call the " << func_name << " function from python \n";
    }

    Py_DECREF(pArgs);

    if (pRetValue != nullptr) {
        yarp::os::Things* result = nullptr;
        if (SWIG_ConvertPtr(pRetValue, (void**)(&result), type, 0) == SWIG_OK && result != nullptr) {
            if (result != &thing) {
                Py_XDECREF(m_lastUpdateResult);
                m_lastUpdateResult = pRetValue;
                pRetValue = nullptr;
            }
            Py_XDECREF(pRetValue);
            PyGILState_Release(gstate);
            return *result;
        }
        PyErr_Clear();

        swig_type_info* writerType = SWIG_TypeQuery(yarpPortWriterSwigType);
        yarp::os::PortWriter* writer = nullptr;
        if (writerType != nullptr &&
            SWIG_ConvertPtr(pRetValue, (void**)(&writer), writerType, 0) == SWIG_OK &&
            writer != nullptr) {
            thing.setPortWriter(writer);
            Py_XDECREF(m_lastUpdateResult);
            m_lastUpdateResult = pRetValue;
            pRetValue = nullptr;
        } else {
            PyErr_Clear();
        }
    }

    Py_XDECREF(pRetValue);

    PyGILState_Release(gstate);
    return thing;
}

yarp::os::Things& MonitorPython::updateReply(yarp::os::Things& thing)
{
    std::lock_guard<std::recursive_mutex> guard(m_monitor_mutex);
    PyGILState_STATE gstate = PyGILState_Ensure();

    PyObject* pRetValue = nullptr;
    PyObject* pArgs = PyTuple_New(1);
    std::string func_name = "updateReply";
    if (pArgs == nullptr) {
        PyErr_Print();
        yCError(PY_PORT_MONITOR) << "Unable to allocate arguments for the " << func_name << " function";
        PyGILState_Release(gstate);
        return thing;
    }
    PyObject* pThing = SWIG_NewPointerObj(&thing, SWIG_TypeQuery(yarpThingsSwigType), 0);
    if (pThing == nullptr) {
        Py_DECREF(pArgs);
        PyErr_Print();
        yCError(PY_PORT_MONITOR) << "Unable to convert reply thing for the " << func_name << " function";
        PyGILState_Release(gstate);
        return thing;
    }
    PyTuple_SetItem(pArgs, 0, pThing);
    yCDebug(PY_PORT_MONITOR) << "Calling " << func_name << " function from python";
    if (!functionWrapper(func_name, pArgs, pRetValue))
    {
        yCError(PY_PORT_MONITOR) << "Unable to call the " << func_name << " function from python \n";
        Py_DECREF(pArgs);
        PyGILState_Release(gstate);
        return thing;
    }
    if (pRetValue != nullptr && pRetValue != Py_None) {
        yarp::os::Things* updatedThing = nullptr;
        if (SWIG_ConvertPtr(pRetValue,
                            reinterpret_cast<void**>(&updatedThing),
                            SWIG_TypeQuery(yarpThingsSwigType),
                            0) == SWIG_OK && updatedThing != nullptr && updatedThing != &thing) {
            thing = *updatedThing;
        }
    }
    Py_XDECREF(pRetValue);
    Py_DECREF(pArgs);
    PyGILState_Release(gstate);
    return thing;
}

bool MonitorPython::setParams(const yarp::os::Property& params)
{
    std::lock_guard<std::recursive_mutex> guard(m_monitor_mutex);
    PyGILState_STATE gstate = PyGILState_Ensure();

    if (!ensureYarpModuleLoaded()) {
        PyGILState_Release(gstate);
        return false;
    }

    swig_type_info* type = SWIG_TypeQuery(yarpPropertySwigType);
    if (type == nullptr) {
        yCError(PY_PORT_MONITOR) << "Swig type of Property is not found";
        PyGILState_Release(gstate);
        return false;
    }

    PyObject* pyParams = SWIG_NewPointerObj((void*)&params, type, 0);
    if (pyParams == nullptr) {
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        PyGILState_Release(gstate);
        return false;
    }

    PyObject* pArgs = PyTuple_Pack(1, pyParams);
    Py_DECREF(pyParams);
    if (pArgs == nullptr) {
        PyErr_Print();
        PyGILState_Release(gstate);
        return false;
    }

    PyObject* pRetValue = nullptr;
    std::string func_name = "setParams";
    if (!functionWrapper(func_name, pArgs, pRetValue))
    {
        yCError(PY_PORT_MONITOR) << "Unable to call the " << func_name << " function from python";
        Py_DECREF(pArgs);
        PyGILState_Release(gstate);
        return false;
    }
    Py_DECREF(pArgs);
    Py_XDECREF(pRetValue);
    PyGILState_Release(gstate);
    return true;
}

bool MonitorPython::getParams(yarp::os::Property& params)
{
    std::lock_guard<std::recursive_mutex> guard(m_monitor_mutex);
    PyGILState_STATE gstate = PyGILState_Ensure();

    if (!ensureYarpModuleLoaded()) {
        PyGILState_Release(gstate);
        return false;
    }

    swig_type_info* type = SWIG_TypeQuery(yarpPropertySwigType);
    if (type == nullptr) {
        yCError(PY_PORT_MONITOR) << "Swig type of Property is not found";
        PyGILState_Release(gstate);
        return false;
    }

    PyObject* pyParams = SWIG_NewPointerObj((void*)&params, type, 0);
    if (pyParams == nullptr) {
        if (PyErr_Occurred()) {
            PyErr_Print();
        }
        PyGILState_Release(gstate);
        return false;
    }

    PyObject* pArgs = PyTuple_Pack(1, pyParams);
    Py_DECREF(pyParams);
    if (pArgs == nullptr) {
        PyErr_Print();
        PyGILState_Release(gstate);
        return false;
    }

    PyObject* pRetValue = nullptr;
    std::string func_name = "getParams";
    if (!functionWrapper(func_name, pArgs, pRetValue))
    {
        yCError(PY_PORT_MONITOR) << "Unable to call the " << func_name << " function from python";
        Py_DECREF(pArgs);
        PyGILState_Release(gstate);
        return false;
    }
    Py_DECREF(pArgs);

    if (pRetValue != nullptr && pRetValue != Py_None) {
        yarp::os::Property* updatedParams = nullptr;
        if (SWIG_ConvertPtr(pRetValue,
                            reinterpret_cast<void**>(&updatedParams),
                            type,
                            0) == SWIG_OK && updatedParams != nullptr && updatedParams != &params) {
            params = *updatedParams;
        }
    }
    Py_XDECREF(pRetValue);
    PyGILState_Release(gstate);
    return true;
}

bool MonitorPython::peerTrigged()
{
    std::lock_guard<std::recursive_mutex> guard(m_monitor_mutex);
    PyGILState_STATE gstate = PyGILState_Ensure();
    PyObject* pRetValue = nullptr;
    PyObject* pArgs = nullptr;
    std::string func_name = "trig";
    if (!functionWrapper(func_name, pArgs, pRetValue))
    {
        yCError(PY_PORT_MONITOR) << "Unable to call the " << func_name << " function from python \n";
        PyGILState_Release(gstate);
        return false;
    }
    PyGILState_Release(gstate);
    return true;
}

bool MonitorPython::canAccept()
{
    if (constraint == "") {
        return true;
    }

    MonitorEventRecord& record = MonitorEventRecord::getInstance();

    /**
     * following piece of code replaces each event symbolic name
     * with a boolean value based on their existence in MonitorEventRecord
     */
    std::string strConstraint = constraint;
    std::string strDummy = strConstraint;
    searchReplace(strDummy, "(", " ");
    searchReplace(strDummy, ")", " ");
    // wrap it with some  guard space
    strDummy = " " + strDummy + " ";
    std::string delimiter = " ";
    size_t pos = 0;
    std::string token;
    while ((pos = strDummy.find(delimiter)) != std::string::npos)
    {
        token = strDummy.substr(0, pos);
        if(token.size() && !isKeyword(token.c_str()))
        {
            record.lock();
                         std::string value = (record.hasEvent(token.c_str())) ? "True" : "False";
            record.unlock();
            searchReplace(strConstraint, token, value);
        }
        strDummy.erase(0, pos + delimiter.length());
    }
    yCTrace(PY_PORT_MONITOR, "constraint = \'%s\'", strConstraint.c_str());
    /*
    * Evaluate the boolean expression using the embedded Python interpreter.
    * The expression is expected to contain only Python boolean operators and
    * the True/False literals injected above.
    */
    PyGILState_STATE gstate = PyGILState_Ensure();
    PyObject* globals = PyDict_New();
    PyObject* locals = PyDict_New();
    PyObject* result = nullptr;
    bool accepted = false;
    if ((globals == nullptr) || (locals == nullptr)) {
        yCError(PY_PORT_MONITOR, "Failed to allocate Python dictionaries to evaluate constraint '%s'", strConstraint.c_str());
        Py_XDECREF(globals);
        Py_XDECREF(locals);
        PyGILState_Release(gstate);
        return false;
    }
    return true;
    PyDict_SetItemString(globals, "__builtins__", PyEval_GetBuiltins());
    result = PyRun_String(strConstraint.c_str(), Py_eval_input, globals, locals);
    if (result == nullptr) {
        yCError(PY_PORT_MONITOR, "Failed to evaluate constraint '%s'", strConstraint.c_str());
        PyErr_Clear();
    } else {
        int isTrue = PyObject_IsTrue(result);
        if (isTrue < 0) {
            yCError(PY_PORT_MONITOR, "Constraint '%s' did not evaluate to a valid boolean value", strConstraint.c_str());
            PyErr_Clear();
        } else {
            accepted = (isTrue != 0);
        }
    }
    Py_XDECREF(result);
    Py_DECREF(globals);
    Py_DECREF(locals);
    PyGILState_Release(gstate);
    return accepted;
}


inline void MonitorPython::searchReplace(std::string& str, const std::string& oldStr, const std::string& newStr)
{
  size_t pos = 0;
  while((pos = str.find(oldStr, pos)) != std::string::npos)
  {
     str.replace(pos, oldStr.length(), newStr);
     pos += newStr.length();
  }
}

inline void MonitorPython::trimString(std::string& str)
{
  std::string::size_type pos = str.find_last_not_of(' ');
  if(pos != std::string::npos) {
    str.erase(pos + 1);
    pos = str.find_first_not_of(' ');
    if (pos != std::string::npos) {
        str.erase(0, pos);
    }
  } else {
      str.erase(str.begin(), str.end());
  }
}

inline bool MonitorPython::isKeyword(const char* str)
{
    if (!str) {
        return false;
    }

    std::string token = str;
    if ((token == "true") || (token == "false") || (token == "and") || (token == "or") || (token == "not")) {
        return true;
    }
    return false;
}


/**
 * static members
 */


bool MonitorPython::loadPythonModule(const std::string& moduleName, PyObject* &pModule)
{
    pModule = nullptr;
    PyGILState_STATE gstate = PyGILState_Ensure();
    if (!Py_IsInitialized())
    {
        yInfo()<<"Calling loadPythonModule Py_Initialize";
        Py_Initialize();
    }

    if (!appendPythonPath(m_path)) {
        PyGILState_Release(gstate);
        return false;
    }

    PyObject *pName = PyUnicode_DecodeFSDefault(moduleName.c_str());
    if (pName == nullptr) {
        PyErr_Print();
        yCError(PY_PORT_MONITOR) << "Unable to decode Python module name: " << moduleName;
        PyGILState_Release(gstate);
        return false;
    }

    pModule = PyImport_Import(pName);
    Py_DECREF(pName);

    if (pModule == nullptr) {
        PyErr_Print();
        yCError(PY_PORT_MONITOR) << "Failed to load Python module: " << moduleName;
        PyGILState_Release(gstate);
        return false;
    }
    PyGILState_Release(gstate);
    return true;
}


bool MonitorPython::functionWrapper(std::string functionName, PyObject* &pArgs, PyObject* &pValue)
{
    pValue = nullptr;
    if (!Py_IsInitialized())
    {
        yInfo()<<"Calling functionWrapper Py_Initialize";
        Py_Initialize();
    }

    PyObject *pFunc;    //Interpreted name of the function

    if (m_pModule != NULL) // Check if the Module has been found and loaded
    {
        // Get the function inside the module
        pFunc = PyObject_GetAttrString(m_pModule, functionName.c_str());

        if (pFunc && PyCallable_Check(pFunc))   // Check if the function has been found and is callable
        {
            // Call the function
            pValue = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pFunc);
            // Check the return value
            if (pValue != NULL)
            {
                // Clear memory
                return true;
            }
            else
            {   // Call Failed
                PyErr_Print();
                yCError(PY_PORT_MONITOR) << "Call failed";

                return false;
            }
        }
        else
        {   // Function not found in the py module
            if (PyErr_Occurred())
                PyErr_Print();
            yCError(PY_PORT_MONITOR) << "Cannot find function: " << functionName;
            Py_XDECREF(pFunc);

            return false;
        }
    }
    else
    {   // Module not found or loaded
        PyErr_Print();
        yCError(PY_PORT_MONITOR) << "Failed to load: " << m_pythonScriptName;
        return false;
    }
}

bool MonitorPython::hasPythonFunction(const std::string& functionName)
{
    PyObject* pFunc = PyObject_GetAttrString(m_pModule, functionName.c_str());
    if (pFunc == nullptr) {
        if (PyErr_ExceptionMatches(PyExc_AttributeError)) {
            PyErr_Clear(); // Function simply does not exist.
        } else {
            PyErr_Print(); // Real error during lookup.
        }

        return false;
    }

    const bool isCallable = PyCallable_Check(pFunc);

    Py_DECREF(pFunc);

    return isCallable;
}

bool MonitorPython::ensureYarpModuleLoaded()
{
    if (!Py_IsInitialized())
    {
        yInfo()<<"Calling ensureYarpModuleLoaded Py_Initialize";
        Py_Initialize();
    }

    if (SWIG_GetModule(nullptr) != nullptr) {
        return true;
    }

    PyObject* yarpModule = PyImport_ImportModule("yarp");
    if (yarpModule == nullptr) {
        PyErr_Print();
        yCError(PY_PORT_MONITOR) << "Unable to import the yarp Python module";
        return false;
    }
    Py_DECREF(yarpModule);

    if (SWIG_GetModule(nullptr) == nullptr) {
        yCError(PY_PORT_MONITOR) << "The yarp Python module did not register SWIG runtime data";
        return false;
    }

    return true;
}

bool MonitorPython::classWrapper(PyObject* &pClassInstance, std::string methodName, PyObject* &pClassMethodArgs, PyObject* &pValue)
{
    if (!Py_IsInitialized())
    {
        yInfo()<<"Calling classWrapper Py_Initialize";
        Py_Initialize();
    }
    PyObject *pMethod;  // Converted name of the class method to PyObject

    yCInfo(PY_PORT_MONITOR) << "[classWrapper] converting method from str";
    pMethod = PyUnicode_FromString(methodName.c_str());

    if (pMethod == NULL)
    {
        yCError(PY_PORT_MONITOR) << "[classWrapper] Unable to convert methodName to python";
        if (PyErr_Occurred())
            PyErr_Print();
        Py_DECREF(pMethod);
        return false;
    }

    yCInfo(PY_PORT_MONITOR) << "[classWrapper] class instance check";
    if (pClassInstance==NULL)
    {
        yCError(PY_PORT_MONITOR) << "[classWrapper] Class instance NULL";
        if (PyErr_Occurred())
            PyErr_Print();
        Py_DECREF(pMethod);
        return false;
    }

    yCInfo(PY_PORT_MONITOR) << "[classWrapper] calling method class";
    pValue = PyObject_CallMethodObjArgs(pClassInstance, pMethod, pClassMethodArgs, NULL);

    if (pValue==NULL)
    {
        yCError(PY_PORT_MONITOR) << "[classWrapper] Returned NULL Value from Class call";
        if (PyErr_Occurred())
            PyErr_Print();
        Py_DECREF(pMethod);
        return false;
    }
    Py_DECREF(pMethod);
    return true;
}

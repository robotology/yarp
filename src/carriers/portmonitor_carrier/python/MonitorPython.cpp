/*
 * SPDX-FileCopyrightText: 2026-2026 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Network.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>

#include "MonitorPython.h"
#include "MonitorLogComponent.h"

#include "python/src_gen/swigpythonrun.h"

using namespace yarp::os;

namespace {
YARP_LOG_COMPONENT(FAKE_SPEECHTR, "yarp.portmonitor.MonitorPython")
}


/**
 * Class MonitorLua
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
        Py_Initialize();
    }
}

MonitorPython::~MonitorPython()
{

}

bool MonitorPython::load(const Property &options)
{
    std::lock_guard<std::recursive_mutex> guard(m_monitor_mutex);
    return true;
}

bool MonitorPython::acceptData(Things &thing)
{
    std::lock_guard<std::recursive_mutex> guard(m_monitor_mutex);
    return true;
}


yarp::os::Things& MonitorPython::updateData(yarp::os::Things& thing)
{
    std::lock_guard<std::recursive_mutex> guard(m_monitor_mutex);

    swig_type_info* type = SWIG_TypeQuery("p_yarp__os__Thing");
    PyObject* pyThing = SWIG_NewPointerObj( (void*)&thing,type, 0 );

    PyObject* pRetValue = nullptr;
    std::string func_name = "updateData";
    if (!functionWrapper(m_pythonScriptName, func_name, pyThing, pRetValue))
    {
        yCError(FAKE_SPEECHTR) << "Unable to call the " << func_name << " function from python \n";
    }

    return thing;
}

yarp::os::Things& MonitorPython::updateReply(yarp::os::Things& thing)
{
    std::lock_guard<std::recursive_mutex> guard(m_monitor_mutex);

    PyObject* pRetValue = nullptr;
    PyObject* pArgs = nullptr;
    std::string func_name = "updateReply";
    if (!functionWrapper(m_pythonScriptName, func_name, pArgs, pRetValue))
    {
        yCError(FAKE_SPEECHTR) << "Unable to call the " << func_name << " function from python \n";
    }

    return thing;
}

bool MonitorPython::setParams(const yarp::os::Property& params)
{
    std::lock_guard<std::recursive_mutex> guard(m_monitor_mutex);

    PyObject* pRetValue = nullptr;
    PyObject* pArgs = nullptr;
    std::string func_name = "setParams";
    if (!functionWrapper(m_pythonScriptName, func_name, pArgs, pRetValue))
    {
        yCError(FAKE_SPEECHTR) << "Unable to call the " << func_name << " function from python \n";
        return false;
    }
    return true;
}

bool MonitorPython::getParams(yarp::os::Property& params)
{
    std::lock_guard<std::recursive_mutex> guard(m_monitor_mutex);

    PyObject* pRetValue = nullptr;
    PyObject* pArgs = nullptr;
    std::string func_name = "getParams";
    if (!functionWrapper(m_pythonScriptName, func_name, pArgs, pRetValue))
    {
        yCError(FAKE_SPEECHTR) << "Unable to call the " << func_name << " function from python \n";
        return false;
    }
    return true;
}

bool MonitorPython::peerTrigged()
{
    std::lock_guard<std::recursive_mutex> guard(m_monitor_mutex);
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
            std::string value = (record.hasEvent(token.c_str())) ? "true" : "false";
            record.unlock();
            searchReplace(strConstraint, token, value);
        }
        strDummy.erase(0, pos + delimiter.length());
    }
    yCTrace(PORTMONITORCARRIER, "constraint = \'%s\'", strConstraint.c_str());

    /*
     *  Using lua to evaluate the boolean expression
     *  Note: this can be replaced by a homebrew boolean
     *  expression validator (e.g., BinNodeType from libyarpmanager)
     */
    strConstraint = "return " + strConstraint;


    return true;
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

bool MonitorPython::functionWrapper(std::string moduleName, std::string functionName, PyObject* &pArgs, PyObject* &pValue)
{
    if (!Py_IsInitialized())
    {
        yInfo()<<"Calling functionWrapper Py_Initialize";
        Py_Initialize();
    }

    PyObject *pName,    //Interpreted name of the module
             *pModule,  //Imported py Module loaded in this object
             *pFunc;    //Interpreted name of the function

    // Build the name object (of the module)
    PyObject* sysPath = PySys_GetObject((char*)"path");     // BORROWED REFERENCE
    PyList_Append(sysPath, (PyUnicode_FromString(m_path.c_str()))); // sets where to look for the module
    pName = PyUnicode_DecodeFSDefault(m_pythonScriptName.c_str());    //The string "Module" should be the name of the python file: i.e. Module.py

    // Load the module object
    pModule = PyImport_Import(pName);
    // Destroy the pName object: not needed anymore
    Py_DECREF(pName);

    if (pModule != NULL) // Check if the Module has been found and loaded
    {
        // Get the function inside the module
        pFunc = PyObject_GetAttrString(pModule, functionName.c_str());

        if (pFunc && PyCallable_Check(pFunc))   // Check if the function has been found and is callable
        {
            // Call the function
            pValue = PyObject_CallObject(pFunc, pArgs);
            // Check the return value
            if (pValue != NULL)
            {
                yCInfo(FAKE_SPEECHTR) << "Returning object " << " \n";
                // Clear memory
                Py_DECREF(pModule);
                return true;
            }
            else
            {   // Call Failed
                Py_DECREF(pModule);
                PyErr_Print();
                yCError(FAKE_SPEECHTR) << "Call failed";

                return false;
            }
        }
        else
        {   // Function not found in the py module
            if (PyErr_Occurred())
                PyErr_Print();
            yCError(FAKE_SPEECHTR) << "Cannot find function: " << functionName;
            //Py_XDECREF(pFunc);
            Py_XDECREF(pModule);

            return false;
        }
    }
    else
    {   // Module not found or loaded
        PyErr_Print();
        yCError(FAKE_SPEECHTR) << "Failed to load: " << m_pythonScriptName;
        return false;
    }
}

bool MonitorPython::classWrapper(PyObject* &pClassInstance, std::string methodName, PyObject* &pClassMethodArgs, PyObject* &pValue)
{
    if (!Py_IsInitialized())
    {
        yInfo()<<"Calling classWrapper Py_Initialize";
        Py_Initialize();
    }
    PyObject *pMethod;  // Converted name of the class method to PyObject

    yCInfo(FAKE_SPEECHTR) << "[classWrapper] converting method from str";
    pMethod = PyUnicode_FromString(methodName.c_str());

    if (pMethod == NULL)
    {
        yCError(FAKE_SPEECHTR) << "[classWrapper] Unable to convert methodName to python";
        if (PyErr_Occurred())
            PyErr_Print();
        Py_DECREF(pMethod);
        return false;
    }

    yCInfo(FAKE_SPEECHTR) << "[classWrapper] class instance check";
    if (pClassInstance==NULL)
    {
        yCError(FAKE_SPEECHTR) << "[classWrapper] Class instance NULL";
        if (PyErr_Occurred())
            PyErr_Print();
        Py_DECREF(pMethod);
        return false;
    }

    yCInfo(FAKE_SPEECHTR) << "[classWrapper] calling method class";
    pValue = PyObject_CallMethodObjArgs(pClassInstance, pMethod, pClassMethodArgs, NULL);

    if (pValue==NULL)
    {
        yCError(FAKE_SPEECHTR) << "[classWrapper] Returned NULL Value from Class call";
        if (PyErr_Occurred())
            PyErr_Print();
        Py_DECREF(pMethod);
        return false;
    }
    Py_DECREF(pMethod);
    return true;
}

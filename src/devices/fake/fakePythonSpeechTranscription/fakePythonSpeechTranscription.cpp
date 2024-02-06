/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fakePythonSpeechTranscription.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/LogComponent.h>

#include <cstdio>
#include <cstdlib>

using namespace yarp::os;
using namespace yarp::dev;

namespace {
YARP_LOG_COMPONENT(FAKE_SPEECHTR, "yarp.device.fakePythonSpeechTranscription")
}

FakePythonSpeechTranscription::FakePythonSpeechTranscription()
{
    m_classInstance=NULL;
}

FakePythonSpeechTranscription::~FakePythonSpeechTranscription()
{
    // Clear references for each PyObject
    if (m_classInstance!=NULL)
    {
        for (long int i = 0; i < m_classInstance->ob_refcnt; ++i)
        {
            Py_XDECREF(m_classInstance);
        }
    }

    Py_FinalizeEx();
}

bool FakePythonSpeechTranscription::open(yarp::os::Searchable& config)
{
    std::string func_name = "open";

    m_moduleName = config.check("moduleName", yarp::os::Value("Module")).asString();
    m_path = config.find("modulePath").asString();
    m_className = config.check("className", yarp::os::Value("SpeechTranscriptor")).asString();

    //Simple Function Calling without passing any parameters

    PyObject *pArgs = NULL,    //Interpreted name of the Arguments
             *pValue;   //Interpreted return value from function call

    // Initialize the Python Interpreter
    if (!Py_IsInitialized())
    {
        yInfo() << "Calling Py_Initialize from open()";
        Py_Initialize();
    }

    if(!functionWrapper(m_moduleName, func_name, pArgs, pValue))
    {
        yCError(FAKE_SPEECHTR) << "[open] Unable to call the " << func_name << " function from python \n";
        return false;
    }

    if (pValue == NULL)
    {
        yCError(FAKE_SPEECHTR) << "[open] Returned null pointer from function call \n";
        Py_DECREF(pValue);
        return false;
    }

    bool ret;   //result from the function call on python side
    if (!boolWrapper(pValue, ret))
    {
        yCError(FAKE_SPEECHTR) << "[open] Unable to convert returned PyObject to bool \n";
        return false;
    }
    Py_DECREF(pValue);

    /*------------------CLASS CREATION*------------------*/
    // This line instantiate the class in Module.py with two parameters: lang=auto and verbose=1
    PyObject* pClassArgs = Py_BuildValue("(si)", m_language.c_str(), 1);

    if (! classInstanceCreator(m_moduleName, m_className, pClassArgs, m_classInstance))
    {
        yCError(FAKE_SPEECHTR) << "Failed to instantiate py class \n";
        Py_XDECREF(pClassArgs);
        return false;
    }
    // Clear not needed PyObjects
    Py_XDECREF(pClassArgs);

    return ret;
}

bool FakePythonSpeechTranscription::close()
{
    std::string func_name = "close";

    //Simple Function Calling without passing any parameters

    PyObject *pArgs = NULL,    //Interpreted name of the Arguments
             *pValue;   //Interpreted return value from function call

    // Initialize the Python Interpreter
    if (!Py_IsInitialized())
    {
        yInfo() << "Calling Close Py_Initialize";
        Py_Initialize();
    }

    if(!functionWrapper(m_moduleName, func_name, pArgs, pValue))
    {
        yCError(FAKE_SPEECHTR) << "[close] Unable to call the close() function from python \n";
        return false;
    }

    if (pValue == NULL)
    {
        yCError(FAKE_SPEECHTR) << "[close] Returned null pointer from function call \n";
        Py_DECREF(pValue);
        return false;
    }

    bool ret;   //result from the function call on python side
    if (!boolWrapper(pValue, ret))
    {
        yCError(FAKE_SPEECHTR) << "[close] Unable to convert returned PyObject to bool \n";
        return false;
    }
    Py_DECREF(pValue);

    return ret;
}

bool FakePythonSpeechTranscription::setLanguage(const std::string& language)
{
   if (!Py_IsInitialized())
    {
        yInfo()<<"Calling test Py_Initialize";
        Py_Initialize();
    }

    PyObject* pRetVal; // Return Value from the class method
    std::string methodName = "set_language";
    PyObject* pInput = PyUnicode_FromString(language.c_str());    // string to pass to the method

    if(! classWrapper(m_classInstance, methodName, pInput, pRetVal))
    {
        yCError(FAKE_SPEECHTR) << "[setLanguage] Returned False at classWrapper \n";
        return false;
    }
    bool result;

    if (!boolWrapper(pRetVal, result))
    {
        yCError(FAKE_SPEECHTR) << "[setLanguage] Unable to convert returned PyObject to bool \n";
        return false;
    }

    if (pInput!=NULL)
    {
        for (long int i = 0; i < pInput->ob_refcnt; ++i)
        {
            Py_XDECREF(pInput);
        }
    }
    Py_XDECREF(pRetVal);
    yInfo() << "Returning from setLanguage: " << result;

    return true;
}

bool FakePythonSpeechTranscription::getLanguage(std::string& language)
{
    if (!Py_IsInitialized())
    {
        yInfo()<<"Calling test Py_Initialize";
        Py_Initialize();
    }

    PyObject* pRetVal; // Return Value from the class method
    std::string methodName = "get_language";
    PyObject* pClassMethodArgs = NULL;  // no arguments passed

    if(! classWrapper(m_classInstance, methodName, pClassMethodArgs, pRetVal))
    {
        yCError(FAKE_SPEECHTR) << "[getLanguage] Returned False at classWrapper \n";
        return false;
    }

    std::string ret;
    if (!stringWrapper(pRetVal, ret))
    {
        yCError(FAKE_SPEECHTR) << "[getLanguage] Unable to covert PyObject to string, Null value \n";
        return false;
    }

    yInfo() << "Returning from getLanguage: " << ret;
    language = ret;
    Py_XDECREF(pRetVal);

    return true;
}

bool FakePythonSpeechTranscription::transcribe(const yarp::sig::Sound& sound, std::string& transcription, double& score)
{
    if (sound.getSamples() == 0 ||
        sound.getChannels() == 0)
    {
        yCError(FAKE_SPEECHTR) << "Invalid Sound sample received";
        transcription = "";
        score = 0.0;
        return false;
    }

    transcription = "hello world";
    score = 1.0;
    return true;
}

bool FakePythonSpeechTranscription::functionWrapper(std::string moduleName, std::string functionName, PyObject* &pArgs, PyObject* &pValue)
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
    PyObject* sysPath = PySys_GetObject((char*)"path");
    PyList_Append(sysPath, (PyUnicode_FromString(m_path.c_str())));
    pName = PyUnicode_DecodeFSDefault(m_moduleName.c_str());    //The string "Module" should be the name of the python file: i.e. Module.py
    yInfo() << "Path of the Module: " << PyUnicode_AsUTF8(pName);
    // Load the module object
    pModule = PyImport_Import(pName);
    // Destroy the pName object: not needed anymore
    Py_DECREF(pName);
    Py_XDECREF(sysPath);

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
        yCError(FAKE_SPEECHTR) << "Failed to load: " << m_moduleName;
        return false;
    }
}

bool FakePythonSpeechTranscription::classWrapper(PyObject* &pClassInstance, std::string methodName, PyObject* &pClassMethodArgs, PyObject* &pValue)
{
    if (!Py_IsInitialized())
    {
        yInfo()<<"Calling classWrapper Py_Initialize";
        Py_Initialize();
    }
    PyObject *pMethod;  // Converted name of the class method to PyObject

    pMethod = PyUnicode_FromString(methodName.c_str());

    if (pMethod == NULL)
    {
        if (PyErr_Occurred())
            PyErr_Print();
        yCError(FAKE_SPEECHTR) << "[classWrapper] Unable to convert methodName to python\n";
        return false;
    }

    if (pClassInstance==NULL)
    {
        if (PyErr_Occurred())
            PyErr_Print();
        yCError(FAKE_SPEECHTR) << "[classWrapper] Class instance NULL \n";
        return false;
    }

    pValue = PyObject_CallMethodObjArgs(pClassInstance, pMethod, pClassMethodArgs, NULL);

    if (pValue==NULL)
    {
        if (PyErr_Occurred())
            PyErr_Print();
        yCError(FAKE_SPEECHTR) << "[classWrapper] Returned NULL Value from Class call \n";
        return false;
    }
    Py_DECREF(pMethod);
    return true;
}

bool FakePythonSpeechTranscription::classInstanceCreator(std::string moduleName, std::string className, PyObject* &pClassArgs, PyObject* &pReturn)
{
    if (!Py_IsInitialized())
    {
        yInfo()<<"Calling classWrapper Py_Initialize";
        Py_Initialize();
    }
    PyObject *pName,    // Interpreted name of the module
             *pModule,  // Imported py Module loaded in this object
             *pClass,   // Interpreted name of the class
             *pDict;    // Dictionary of the interpreter

    // Build the name object (of the module)
    PyObject* sysPath = PySys_GetObject((char*)"path");
    PyList_Append(sysPath, (PyUnicode_FromString(m_path.c_str())));
    pName = PyUnicode_DecodeFSDefault(m_moduleName.c_str());    //The string "Module" should be the name of the python file: i.e. Module.py

    // Load the module object
    pModule = PyImport_Import(pName);
    // Destroy the pName object: not needed anymore
    Py_DECREF(pName);
    Py_DECREF(sysPath);

    if (pModule==NULL)
    {
        if (PyErr_Occurred())
            PyErr_Print();
        yCError(FAKE_SPEECHTR) << "[classWrapper] Unable to create module instance \n";
        return false;
    }
    Py_DECREF(pModule);

    // pDict is a borrowed reference
    pDict = PyModule_GetDict(pModule);
    // Build the name of a callable class -> borrowed reference
    pClass = PyDict_GetItemString(pDict, className.c_str());

    if (PyCallable_Check(pClass))
    {
        //Create instance of the class
        pReturn = PyObject_CallObject(pClass, pClassArgs);

        if (pReturn==NULL)
        {
            if (PyErr_Occurred())
                PyErr_Print();
            yCError(FAKE_SPEECHTR) << "[classWrapper] Unable to instantiate Class with given class arguments \n";

            if (pReturn->ob_refcnt>0)
            {
                Py_XDECREF(pReturn);
            }
            return false;
        }

        return true;
    }
    else
    {
        if (PyErr_Occurred())
            PyErr_Print();
        yCError(FAKE_SPEECHTR) << "[classWrapper] Unable to find class in py module \n";
        return false;
    }
}

bool FakePythonSpeechTranscription::stringWrapper(PyObject* &pValue, std::string &ret)
{
    if (pValue!=NULL)
    {
        ret = std::string(PyUnicode_AsUTF8(pValue));
        return true;
    }
    else
    {
        yCError(FAKE_SPEECHTR) << "Null pValue passed to FakePythonSpeechTranscription::stringWrapper \n";
        return false;
    }
}

bool FakePythonSpeechTranscription::intWrapper(PyObject* &pValue, long &ret)
{
    if (pValue!=NULL)
    {
        ret = PyLong_AsLong(pValue);
        return true;
    }
    else
    {
        yCError(FAKE_SPEECHTR) << "Null pValue passed to FakePythonSpeechTranscription::intWrapper \n";
        return false;
    }
}

bool FakePythonSpeechTranscription::doubleWrapper(PyObject* &pValue, double &ret)
{
    if (pValue!=NULL)
    {
        ret = PyFloat_AsDouble(pValue);
        return true;
    }
    else
    {
        yCError(FAKE_SPEECHTR) << "Null pValue passed to FakePythonSpeechTranscription::doubleWrapper \n";
        return false;
    }
}

bool FakePythonSpeechTranscription::boolWrapper(PyObject* &pValue, bool &ret)
{
    if (pValue!=NULL)
    {
        ret = (bool)PyLong_AsLong(pValue);
        return true;
    }
    else
    {
        yCError(FAKE_SPEECHTR) << "Null pValue passed to FakePythonSpeechTranscription::boolWrapper \n";
        return false;
    }
}

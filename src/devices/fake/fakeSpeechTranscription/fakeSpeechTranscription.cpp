/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fakeSpeechTranscription.h"

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/LogComponent.h>

#include <cstdio>
#include <cstdlib>

using namespace yarp::os;
using namespace yarp::dev;

namespace {
YARP_LOG_COMPONENT(FAKE_SPEECHTR, "yarp.device.FakeSpeechTranscription")
}

FakeSpeechTranscription::FakeSpeechTranscription()
{
}

FakeSpeechTranscription::~FakeSpeechTranscription()
{
    close();
    Py_FinalizeEx();
    // Hardcoding finalizer???
    yInfo()<<"Finalizing destroying";
    //PyRun_SimpleString("import sys\n"
    //                    "sys.exit()");
}

bool FakeSpeechTranscription::open(yarp::os::Searchable& config)
{
    const char * func_name = "open";
    //Simple Function Calling without passing any parameters

    PyObject *pName,    //Interpreted name of the module
             *pModule,  //Imported py Module loaded in this object
             *pFunc,    //Interpreted name of the function
             *pValue;   //Value returned by the function

    // Initialize the Python Interpreter
    if (!Py_IsInitialized())
    {
        yInfo()<<"Calling Open Py_Initialize";
        Py_Initialize();
    }

    // Build the name object (of the module)
    PyObject* sysPath = PySys_GetObject((char*)"path");
    PyList_Append(sysPath, (PyUnicode_FromString(m_path)));
    pName = PyUnicode_DecodeFSDefault(m_moduleName);    //The string "Module" should be the name of the python file: i.e. Module.py

    // Load the module object
    pModule = PyImport_Import(pName);
    // Destroy the pName object: not needed anymore
    Py_DECREF(pName);
    Py_XDECREF(sysPath);

    if (pModule != NULL) // Check if the Module has been found and loaded
    {
        // Get the function inside the module
        pFunc = PyObject_GetAttrString(pModule, func_name);

        if (pFunc && PyCallable_Check(pFunc))   // Check if the function has been found and is callable
        {
            // Call the function
            pValue = PyObject_CallObject(pFunc, NULL);
            // Check the return value
            if (pValue != NULL) 
            {
                printf("Result of call: %ld\n", PyLong_AsLong(pValue)); // 0 or 1
                Py_DECREF(pValue);
            }
            else 
            {   // Call Failed
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                fprintf(stderr,"Call failed\n");
                return false;
            }
        }
        else 
        {   // Function not found in the py module
            if (PyErr_Occurred())
                PyErr_Print();
            fprintf(stderr, "Cannot find function \"%s\"\n", func_name);
            Py_XDECREF(pFunc);
            Py_DECREF(pModule);
            return false;
        }
        // Clear memory
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    }
    else 
    {   // Module not found or loaded
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n", m_moduleName);
        return false;
    }
    //test seq
    std::string lang = "ita";
    setLanguage(lang);
    std::string out = "test";
    getLanguage(out);
    printf(out.c_str());

    return true;
}

bool FakeSpeechTranscription::close()
{
    const char * func_name = "close";
    //Simple Function Calling without passing any parameters

    PyObject *pName,    //Interpreted name of the module
             *pModule,  //Imported py Module loaded in this object
             *pFunc,    //Interpreted name of the module
             *pValue;   //Interpreted name of the module

    // Initialize the Python Interpreter
    if (!Py_IsInitialized())
    {
        yInfo()<<"Calling Close Py_Initialize";
        Py_Initialize();
    }

    // Build the name object (of the module)
    PyObject* sysPath = PySys_GetObject((char*)"path");
    PyList_Append(sysPath, (PyUnicode_FromString(m_path)));
    pName = PyUnicode_DecodeFSDefault(m_moduleName);    //The string "Module" should be the name of the python file: i.e. Module.py

    // Load the module object
    pModule = PyImport_Import(pName);
    // Destroy the pName object: not needed anymore
    Py_DECREF(pName);
    Py_XDECREF(sysPath);

    if (pModule != NULL) // Check if the Module has been found and loaded
    {
        // Get the function inside the module
        pFunc = PyObject_GetAttrString(pModule, func_name);

        if (pFunc && PyCallable_Check(pFunc))   // Check if the function has been found and is callable
        {
            // Call the function
            pValue = PyObject_CallObject(pFunc, NULL);
            // Check the return value
            if (pValue != NULL) 
            {
                printf("Result of call: %ld\n", PyLong_AsLong(pValue)); // 0 or 1
                Py_DECREF(pValue);
            }
            else 
            {   // Call Failed
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                fprintf(stderr,"Call failed\n");
                return false;
            }
        }
        else 
        {   // Function not found in the py module
            if (PyErr_Occurred())
                PyErr_Print();
            fprintf(stderr, "Cannot find function \"%s\"\n", func_name);
            Py_XDECREF(pFunc);
            Py_DECREF(pModule);
            return false;
        }
        // Clear memory
        Py_XDECREF(pFunc);
        Py_XDECREF(pModule);
    }
    else 
    {   // Module not found or loaded
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n", m_moduleName);
        return false;
    }
    return true;
}

bool FakeSpeechTranscription::setLanguage(const std::string& language)
{
    m_language=language;
    yCInfo(FAKE_SPEECHTR) << "Language set to" << language;

    const char * func_name = "set_language";
    //Simple Function Calling without passing any parameters

    // Initialize the Python Interpreter
    if (!Py_IsInitialized())
    {
        yInfo()<<"Calling setLanguage Py_Initialize";
        Py_Initialize();
    }

    PyObject *pName,    //Interpreted name of the module
             *pModule,  //Imported py Module loaded in this object
             *pFunc,    //Interpreted name of the module
             *pValue,   //Interpreted name of the module
             *pArgs;    //Interpreted argument list

    

    // Build the name object (of the module)
    PyObject* sysPath = PySys_GetObject((char*)"path");
    PyList_Append(sysPath, (PyUnicode_FromString(m_path)));
    pName = PyUnicode_DecodeFSDefault(m_moduleName);    //The string "Module" should be the name of the python file: i.e. Module.py

    // Load the module object
    pModule = PyImport_Import(pName);
    // Destroy the pName object: not needed anymore
    Py_DECREF(pName);
    Py_XDECREF(sysPath);

    if (pModule != NULL) // Check if the Module has been found and loaded
    {
        // Get the function inside the module
        pFunc = PyObject_GetAttrString(pModule, func_name);

        if (pFunc && PyCallable_Check(pFunc))   // Check if the function has been found and is callable
        {
            pArgs = PyTuple_New(1); // Set the size to 1, since the number of the arguments is 1
            pValue = PyUnicode_FromString(language.c_str());
            if (!pValue)
            {
                Py_DECREF(pArgs);
                Py_DECREF(pModule);
                Py_DECREF(pValue);
                fprintf(stderr, "Cannot convert argument\n");
                return false;
            }
            PyTuple_SetItem(pArgs, 0, pValue);

            // Call the function
            pValue = PyObject_CallObject(pFunc, pArgs);
            // Check the return value
            if (pValue != NULL) 
            {
                const char* returnString = PyUnicode_AsUTF8(pValue);
                if (returnString==NULL)
                {
                    yCInfo(FAKE_SPEECHTR) << "Unable to Convert PyUnicode_AsUTF8 String, are you sure that you are getting back a string? \n";
                }
                
                yCInfo(FAKE_SPEECHTR) << "Returning " << returnString << " \n";
                
                Py_DECREF(pValue);
            }
            else 
            {   // Call Failed
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                fprintf(stderr,"Call failed\n");
                return false;
            }
        }
        else 
        {   // Function not found in the py module
            if (PyErr_Occurred())
                PyErr_Print();
            fprintf(stderr, "Cannot find function \"%s\"\n", func_name);
            Py_XDECREF(pFunc);
            Py_DECREF(pModule);
            return false;
        }
        // Clear memory
        Py_XDECREF(pArgs);
        Py_XDECREF(pFunc);
        Py_XDECREF(pModule);
    }
    else 
    {   // Module not found or loaded
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n", m_moduleName);
        return false;
    }
    return true;
}

bool FakeSpeechTranscription::getLanguage(std::string& language)
{
    if (!Py_IsInitialized())
    {
        yInfo()<<"Calling getLanguage Py_Initialize";
        Py_Initialize();
    }

    //language = m_language;
    PyObject* pArgs = PyTuple_New(1);
    PyObject* pString = PyUnicode_FromString(language.c_str());
    PyTuple_SetItem(pArgs, 0, pString);
    PyObject* pRet;
    if(! functionWrapper(m_moduleName, "get_language", pArgs, pRet))
    {
        yCError(FAKE_SPEECHTR) << "[getLanguage] Returned False at functionWrapper \n";
        Py_XDECREF(pArgs);
        Py_XDECREF(pString);
        return false;
    }
    yInfo() << "functionWrapper OK \n";
    std::string ret = stringWrapper(pRet);

    if (ret=="")
    {
        yCError(FAKE_SPEECHTR) << "[getLanguage] Unable to convert value in stringWrapper \n";
    }
    language = ret;
    yInfo() << "Returning converted string:" << language << "\n";
    yInfo() << "stringWrapper OK \n";
    Py_XDECREF(pArgs);
    yInfo() << "1 OK \n";
    Py_XDECREF(pString);
    yInfo() << "2 OK \n";
    Py_XDECREF(pRet);
    return true;
}


bool FakeSpeechTranscription::transcribe(const yarp::sig::Sound& sound, std::string& transcription, double& score)
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

bool FakeSpeechTranscription::functionWrapper(std::string moduleName, std::string functionName, PyObject* &pArgs, PyObject* &pValue)
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
    PyList_Append(sysPath, (PyUnicode_FromString(m_path)));
    pName = PyUnicode_DecodeFSDefault(m_moduleName);    //The string "Module" should be the name of the python file: i.e. Module.py

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
                Py_XDECREF(pFunc);
                Py_DECREF(pModule);

                return true;
            }
            else 
            {   // Call Failed
                Py_XDECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                fprintf(stderr,"Call failed\n");

                return false;
            }
        }
        else 
        {   // Function not found in the py module
            if (PyErr_Occurred())
                PyErr_Print();
            fprintf(stderr, "Cannot find function \"%s\"\n", functionName);
            Py_XDECREF(pFunc);
            Py_DECREF(pModule);

            return false;
        }
    }
    else 
    {   // Module not found or loaded
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n", m_moduleName);
        Py_DECREF(pModule);

        return false;
    }
    
    Py_XDECREF(pFunc);
    Py_XDECREF(pModule);

}

bool FakeSpeechTranscription::classWrapper(std::string moduleName, std::string className, std::string functionName, PyObject* &pClassArgs, PyObject* &pClassMethodArgs, PyObject* &pValue)
{
    if (!Py_IsInitialized())
    {
        yInfo()<<"Calling classWrapper Py_Initialize";
        Py_Initialize();
    }
    PyObject *pName,    // Interpreted name of the module
             *pModule,  // Imported py Module loaded in this object
             *pFunc,    // Interpreted name of the function
             *pClass,   // Interpreted name of the class
             *pDict,    // Dictionary of the interpreter
             *pInstance;// Instance of the Class on the interpreter
    
    // Build the name object (of the module)
    PyObject* sysPath = PySys_GetObject((char*)"path");
    PyList_Append(sysPath, (PyUnicode_FromString(m_path)));
    pName = PyUnicode_DecodeFSDefault(m_moduleName);    //The string "Module" should be the name of the python file: i.e. Module.py

    // Load the module object
    pModule = PyImport_Import(pName);
    // Destroy the pName object: not needed anymore
    Py_DECREF(pName);
    Py_XDECREF(sysPath);

    if (pModule != NULL)
    {
        // pDict is a borrowed reference 
        pDict = PyModule_GetDict(pModule);
        // Build the name of a callable class -> borrowed reference
        pClass = PyDict_GetItemString(pDict, className.c_str());

        if (pClass && PyCallable_Check(pClass))
        {
            // pFunc is also a borrowed reference 
            pFunc = PyDict_GetItemString(pDict, functionName.c_str());
            pInstance = PyObject_CallObject(pClass, pClassArgs);

            if (pFunc != NULL && pInstance!= NULL)
            {
                pValue = PyObject_CallMethodObjArgs(pInstance, pFunc, pClassMethodArgs);
                if (pValue==NULL)
                {
                    Py_XDECREF(pModule);
                    //Py_XDECREF(pFunc);
                    //Py_XDECREF(pClass);
                    //Py_XDECREF(pDict);
                    Py_XDECREF(pInstance);
                    return false;
                }
            }
            else
            {
                if (PyErr_Occurred())
                    PyErr_Print();
                yCError(FAKE_SPEECHTR) << "[classWrapper] Returned False at classWrapper \n";
                Py_XDECREF(pModule);
                //Py_XDECREF(pFunc);
                //Py_XDECREF(pClass);
                //Py_XDECREF(pDict);
                Py_XDECREF(pInstance);
                return false;
            }
        }
    }
    Py_XDECREF(pModule);
    //Py_XDECREF(pFunc);
    //Py_XDECREF(pClass);
    //Py_XDECREF(pDict);
    Py_XDECREF(pInstance);
    return true;
}

std::string FakeSpeechTranscription::stringWrapper(PyObject* &pValue)
{
    if (pValue!=NULL)
    {
        return std::string(PyUnicode_AsUTF8(pValue));
    }
    else
    {
        yCError(FAKE_SPEECHTR) << "Null pValue passed to FakeSpeechTranscription::stringWrapper \n"; 
        return NULL;
    }
}

long FakeSpeechTranscription::intWrapper(PyObject* &pValue)
{
    if (pValue!=NULL)
    {
        return PyLong_AsLong(pValue);
    }
    else
    {
        yCError(FAKE_SPEECHTR) << "Null pValue passed to FakeSpeechTranscription::intWrapper \n"; 
        return NULL;
    }
}

double FakeSpeechTranscription::doubleWrapper(PyObject* &pValue)
{
    if (pValue!=NULL)
    {
        return PyFloat_AsDouble(pValue);
    }
    else
    {
        yCError(FAKE_SPEECHTR) << "Null pValue passed to FakeSpeechTranscription::doubleWrapper \n"; 
        return NULL;
    }
}

bool FakeSpeechTranscription::boolWrapper(PyObject* &pValue)
{
    if (pValue!=NULL)
    {
        return ((bool)PyLong_AsLong(pValue));
    }
    else
    {
        yCError(FAKE_SPEECHTR) << "Null pValue passed to FakeSpeechTranscription::boolWrapper \n"; 
        return NULL;
    }
}
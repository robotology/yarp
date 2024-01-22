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
    m_classInstance=NULL;
}

FakeSpeechTranscription::~FakeSpeechTranscription()
{
    close();
    // Clear references for each PyObject
    if (m_classInstance!=NULL)
    {
        for (size_t i = 0; i < m_classInstance->ob_refcnt; ++i)
        {
            Py_DECREF(m_classInstance);
        }
    }
    
    Py_FinalizeEx();
    yInfo()<<"Finalizing and destroying";
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
            Py_DECREF(pModule);
            return false;
        }
        // Clear memory
        Py_DECREF(pModule);
    }
    else 
    {   // Module not found or loaded
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n", m_moduleName);
        return false;
    }

    /*------------------CLASS CREATION*------------------*/

    PyObject* pClassArgs = Py_BuildValue("(si)", "french", 1);
    // PyObject* pString = PyUnicode_FromString(m_className.c_str());

    if (! classInstanceCreator(m_moduleName, m_className, pClassArgs, m_classInstance))
    {
        yCError(FAKE_SPEECHTR) << "[test] Failed to instanciate py class \n";
        //Py_XDECREF(pString);
        Py_XDECREF(pClassArgs);
        return false;
    }
    // Clear not needed PyObjects
    //Py_XDECREF(pString);
    Py_XDECREF(pClassArgs);


    yInfo() << "-----------------------------TEST--------------------------- " ;
    //test seq
    std::string lang = "ita";
    yInfo() << "SetLanguage " << lang;
    setLanguage(lang);

    std::string out = "polish";
    yInfo() << "getLanguage with " << out;
    getLanguage(out);
    yInfo() << out;

    yInfo() << "Testing with class wrapper ";
    if (! test("testing please"))
    {
        yCError(FAKE_SPEECHTR) << "test Failed";  
    }
    
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
            Py_DECREF(pModule);
            return false;
        }
        // Clear memory
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
    /*
    m_language=language;
    yCInfo(FAKE_SPEECHTR) << "Setting Language with functionWrapper" << language;

   if (!Py_IsInitialized())
    {
        yInfo()<<"Calling setLanguage Py_Initialize";
        Py_Initialize();
    }

    //language = m_language;
    PyObject* pArgs = PyTuple_New(1);
    PyObject* pString = PyUnicode_FromString(language.c_str());
    PyTuple_SetItem(pArgs, 0, pString);
    PyObject* pRet;
    if(! functionWrapper(m_moduleName, "set_language", pArgs, pRet))
    {
        yCError(FAKE_SPEECHTR) << "[setLanguage] Returned False at functionWrapper \n";
        Py_XDECREF(pArgs);
        Py_XDECREF(pString);
        return false;
    }
    std::string ret = stringWrapper(pRet);

    if (ret=="")
    {
        yCError(FAKE_SPEECHTR) << "[setLanguage] Unable to convert value in stringWrapper \n";
    }
    yInfo() << "Returning converted string:" << ret << "\n";
    Py_XDECREF(pArgs);
    Py_XDECREF(pString);
    
    return true;
    */

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
    bool result = boolWrapper(pRetVal);

    yInfo() << "Returning from setLanguage: " << result;

    return true;
}

bool FakeSpeechTranscription::getLanguage(std::string& language)
{
    /*if (!Py_IsInitialized())
    {
        yInfo()<<"Calling getLanguage Py_Initialize";
        Py_Initialize();
    }
    std::string className = "SpeecTranscriptor";
    std::string functionName = "set_language";

    PyObject *pName,    // Interpreted name of the module
             *pModule,  // Imported py Module loaded in this object
             *pClass,   // Interpreted name of the class
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
        // Build the name of a callable class -> borrowed reference
        pClass = PyObject_GetAttrString(pModule, className.c_str());
        if (pClass && PyCallable_Check(pClass))
        {
            PyObject* pClassArgs = Py_BuildValue("(si)", "french", 1 );

            pInstance = PyObject_CallObject(pClass, pClassArgs);

            Py_DECREF(pClassArgs);

            if (pInstance!= NULL)
            {


                PyObject* pValue = PyObject_CallMethod(pInstance, functionName.c_str(), "(s)", language.c_str());

                if (pValue==NULL)
                {
                    yCError(FAKE_SPEECHTR) << "Returned NULL Value from Class call \n";
                    Py_XDECREF(pModule);
                    Py_XDECREF(pInstance);
                    return false;
                }
                char* tmp;
                PyArg_Parse(pValue, "s", &tmp); 
                yInfo() << "Converted Value: " << std::string(tmp) << " From wrapper: " << stringWrapper(pValue);
                language = std::string(tmp);
                Py_XDECREF(pValue);
            }
            else
            {
                if (PyErr_Occurred())
                    PyErr_Print();
                yCError(FAKE_SPEECHTR) << "[classWrapper] Returned False at classWrapper \n";
                Py_XDECREF(pModule);
                Py_XDECREF(pInstance);
                
                return false;
            }
        }
        else
        {
            if (PyErr_Occurred())
                PyErr_Print();
            yCError(FAKE_SPEECHTR) << "[classWrapper] Unable to create class instance \n";
            Py_XDECREF(pModule);
            return false;
        }
    }
    Py_XDECREF(pModule);
    Py_XDECREF(pInstance);
    return true;    */

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
    
    std::string ret = stringWrapper(pRetVal);

    yInfo() << "Returning from getLanguage: " << ret;
    language = ret;
    Py_XDECREF(pRetVal);

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
                Py_DECREF(pModule);
                return true;
            }
            else 
            {   // Call Failed
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
            //Py_XDECREF(pFunc);
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
}

bool FakeSpeechTranscription::classWrapper(PyObject* &pClassInstance, std::string methodName, PyObject* &pClassMethodArgs, PyObject* &pValue)
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
    return true;
}


bool FakeSpeechTranscription::classInstanceCreator(std::string moduleName, std::string className, PyObject* &pClassArgs, PyObject* &pReturn)
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
    PyList_Append(sysPath, (PyUnicode_FromString(m_path)));
    pName = PyUnicode_DecodeFSDefault(m_moduleName);    //The string "Module" should be the name of the python file: i.e. Module.py

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
            yCError(FAKE_SPEECHTR) << "[classWrapper] Unable to instantiate Class with given class aguments \n";

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



bool FakeSpeechTranscription::test(std::string dummy) 
{
    /*if (!Py_IsInitialized())
    {
        yInfo()<<"Calling test Py_Initialize";
        Py_Initialize();
    }

    PyObject* pClassArgs = Py_BuildValue("(sis)", "french", 11, "happy" );
    PyObject* pString = PyUnicode_FromString(dummy.c_str());
    PyObject* pRet;

    if(! classWrapper(m_moduleName, "Dummy" , "set_languag", pClassArgs, pString, pRet))
    {
        yCError(FAKE_SPEECHTR) << "[test] Returned False at classWrapper \n";
        Py_XDECREF(pString);
        Py_XDECREF(pClassArgs);
        return false;
    }
    std::string ret = stringWrapper(pRet);

    yInfo() << "Returning from test routine: " << ret;

    Py_XDECREF(pString);
    Py_XDECREF(pClassArgs);
    */

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
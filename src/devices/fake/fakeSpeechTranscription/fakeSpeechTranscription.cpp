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

#include "Python.h"

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
}

bool FakeSpeechTranscription::open(yarp::os::Searchable& config)
{
    const char * func_name = "open";
    //Simple Function Calling without passing any parameters

    PyObject *pName,    //Interpreted name of the module
             *pModule,  //Imported py Module loaded in this object
             *pDict,    //Interpreted name of the module
             *pFunc,    //Interpreted name of the module
             *pValue;   //Interpreted name of the module

    // Initialize the Python Interpreter
    Py_Initialize();

    // Build the name object (of the module)
    PyObject* sysPath = PySys_GetObject((char*)"path");
    PyList_Append(sysPath, (PyUnicode_FromString(m_path)));
    pName = PyUnicode_DecodeFSDefault(m_moduleName);    //The string "Module" should be the name of the python file: i.e. Module.py

    // Load the module object
    pModule = PyImport_Import(pName);
    // Destroy the pName object: not needed anymore
    Py_DECREF(pName);

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

    //setLanguage("asd");
    return true;
}

bool FakeSpeechTranscription::close()
{
    const char * func_name = "close";
    //Simple Function Calling without passing any parameters

    PyObject *pName,    //Interpreted name of the module
             *pModule,  //Imported py Module loaded in this object
             *pDict,    //Interpreted name of the module
             *pFunc,    //Interpreted name of the module
             *pValue;   //Interpreted name of the module

    // Initialize the Python Interpreter
    Py_Initialize();

    // Build the name object (of the module)
    PyObject* sysPath = PySys_GetObject((char*)"path");
    PyList_Append(sysPath, (PyUnicode_FromString(m_path)));
    pName = PyUnicode_DecodeFSDefault(m_moduleName);    //The string "Module" should be the name of the python file: i.e. Module.py

    // Load the module object
    pModule = PyImport_Import(pName);
    // Destroy the pName object: not needed anymore
    Py_DECREF(pName);

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
    return true;
}

bool FakeSpeechTranscription::setLanguage(const std::string& language)
{
    m_language=language;
    yCInfo(FAKE_SPEECHTR) << "Language set to" << language;

    const char * func_name = "set_language";
    //Simple Function Calling without passing any parameters

    PyObject *pName,    //Interpreted name of the module
             *pModule,  //Imported py Module loaded in this object
             *pDict,    //Interpreted name of the module
             *pFunc,    //Interpreted name of the module
             *pValue,   //Interpreted name of the module
             *pArgs;    //Interpreted argument list

    // Initialize the Python Interpreter
    Py_Initialize();

    // Build the name object (of the module)
    PyObject* sysPath = PySys_GetObject((char*)"path");
    PyList_Append(sysPath, (PyUnicode_FromString(m_path)));
    pName = PyUnicode_DecodeFSDefault(m_moduleName);    //The string "Module" should be the name of the python file: i.e. Module.py

    // Load the module object
    pModule = PyImport_Import(pName);
    // Destroy the pName object: not needed anymore
    Py_DECREF(pName);

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
        Py_DECREF(pArgs);
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
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
    language = m_language;
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

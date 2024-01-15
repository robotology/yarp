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
    //Simple Function Calling without passing any parameters

    PyObject *pName,    //Interpreted name of the module
             *pModule,  //Imported py Module loaded in this object
             *pDict,    //Interpreted name of the module
             *pFunc,    //Interpreted name of the module
             *pValue;   //Interpreted name of the module

    // Initialize the Python Interpreter
    Py_Initialize();

    // Build the name object (of the module)
    pName = PyUnicode_DecodeFSDefault("Module");    //The string "Module" should be the name of the python file: i.e. Module.py

    // Load the module object
    pModule = PyImport_Import(pName);
    // Destroy the pName object: not needed anymore
    Py_DECREF(pName);

    if (pModule != NULL) // Check if the Module has been found and loaded
    {
        // Get the function inside the module
        pFunc = PyObject_GetAttrString(pModule, "open");

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
            fprintf(stderr, "Cannot find function \"%s\"\n", "open");
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
        fprintf(stderr, "Failed to load \"%s\"\n", "Module");
        return false;
    }
    return true;
}

bool FakeSpeechTranscription::close()
{
    return true;
}

bool FakeSpeechTranscription::setLanguage(const std::string& language)
{
    m_language=language;
    yCInfo(FAKE_SPEECHTR) << "Language set to" << language;
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

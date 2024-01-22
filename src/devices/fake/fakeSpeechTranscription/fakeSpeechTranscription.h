/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef FAKESPEECHTRANSCRIPTION_H
#define FAKESPEECHTRANSCRIPTION_H

#include <yarp/dev/DeviceDriver.h>
#include <yarp/dev/ISpeechTranscription.h>
#include <yarp/os/Bottle.h>
#include <stdio.h>
#include <list>

#include "Python.h"

using namespace yarp::os;

/**
 * @ingroup dev_impl_other
 *
 * \brief `FakeSpeechTranscription`: A fake implementation of a speech transcriber plugin.
 */
class FakeSpeechTranscription :
        public yarp::dev::DeviceDriver,
        public yarp::dev::ISpeechTranscription
{
private:
    bool m_verbose = true;
    std::string m_language="auto";

    const char * m_moduleName = "Module";   // Name of the python file containing the functions/classes
    const char * m_path = "/home/ecub_docker/yarp/src/devices/fake/fakeSpeechTranscription";    // Path where the file is located
    std::string m_className = "SpeecTranscriptor";  // Name of the class contained in @m_moduleName at @m_path

    PyObject * m_classInstance;     // Python object of the created class

    /**
     * @brief Calls a function present in a .py file and returns its values as python objects.
     * @param moduleName The name of the .py file
     * @param functionName The name of the function contained in the .py file
     * @param pArgs PyObject Arguments needed for creating the class (pass NULL if no argument is needed)
     * @param pReturn The returned reference to the returned PyObject
     * @return True in case of success, false otherwise.
     */
    bool functionWrapper(std::string moduleName, std::string functionName, PyObject* &pArgs, PyObject* &pValue);

    /**
     * @brief Wraps the call to a class method.
     * @param pClassInstance The configuration parameters
     * @param methodName The name of the method present in the class instance
     * @param pClassMethodArgs PyObject Arguments needed for creating the class (pass NULL if no argument is needed)
     * @param pValue PyObject returned by the method
     * @return True in case of success, false otherwise.
     */
    bool classWrapper(PyObject* &pClassInstance, std::string methodName, PyObject* &pClassMethodArgs, PyObject* &pValue);

    /**
     * @brief Initialize python class described in the python file as an interpreted object. 
     * @param moduleName The name of the .py file
     * @param className The name of the class contained in the .py file
     * @param pClassArgs PyObject Arguments needed for creating the class (pass NULL if no argument is needed)
     * @param pReturn The returned reference to the instance object
     * @return True in case of success, false otherwise.
     */
    bool classInstanceCreator(std::string moduleName, std::string className, PyObject* &pClassArgs, PyObject* &pReturn);

    /**
     * @brief Converts a python object to string, returns NULL otherwise
     * @param pValue PyObject to be converted
     * @return The converted string
     */
    std::string stringWrapper(PyObject* &pValue);

    /**
     * @brief Converts a python object to long, returns NULL otherwise
     * @param pValue PyObject to be converted
     * @return The converted long
     */
    long intWrapper(PyObject* &pValue);

    /**
     * @brief Converts a python object to double, returns NULL otherwise
     * @param pValue PyObject to be converted
     * @return The converted double
     */
    double doubleWrapper(PyObject* &pValue);

    /**
     * @brief Converts a python object to bool, returns NULL otherwise
     * @param pValue PyObject to be converted
     * @return The converted bool
     */
    bool boolWrapper(PyObject* &pValue);

public:
    FakeSpeechTranscription();
    virtual ~FakeSpeechTranscription();
    FakeSpeechTranscription(const FakeSpeechTranscription&) = delete;
    FakeSpeechTranscription(FakeSpeechTranscription&&) = delete;
    FakeSpeechTranscription& operator=(const FakeSpeechTranscription&) = delete;
    FakeSpeechTranscription& operator=(FakeSpeechTranscription&&) = delete;

    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    virtual bool setLanguage(const std::string& language) override;
    virtual bool getLanguage(std::string& language) override;
    virtual bool transcribe(const yarp::sig::Sound& sound, std::string& transcription, double& score) override;
};

#endif

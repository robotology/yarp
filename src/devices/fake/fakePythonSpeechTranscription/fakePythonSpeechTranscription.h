/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef FAKEPYTHONSPEECHTRANSCRIPTION_H
#define FAKEPYTHONSPEECHTRANSCRIPTION_H

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
 * \brief `FakePythonSpeechTranscription`: A fake implementation of a speech transcriber plugin using python embedding.
 */
class FakePythonSpeechTranscription :
        public yarp::dev::DeviceDriver,
        public yarp::dev::ISpeechTranscription
{
private:
    bool m_verbose = true;
    std::string m_language="auto";

    std::string m_moduleName = "a";   // Name of the python file containing the functions/classes
    std::string m_path = "b";    // Path where the file is located
    std::string m_className = "c";  // Name of the class contained in @m_moduleName at @m_path

    PyObject * m_classInstance;     // Python object of the created class

    /**
     * @brief Calls a function present in a .py file and returns its values as python object.
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
     * @param ret The converted string
     * @return True in case of success, false otherwise.
     */
    bool stringWrapper(PyObject* &pValue, std::string &ret);

    /**
     * @brief Converts a python object to long, returns NULL otherwise
     * @param pValue PyObject to be converted
     * @param ret The converted long
     * @return True in case of success, false otherwise.
     */
    bool intWrapper(PyObject* &pValue, long &ret);

    /**
     * @brief Converts a python object to double, returns NULL otherwise
     * @param pValue PyObject to be converted
     * @param ret The converted double
     * @return True in case of success, false otherwise.
     */
    bool doubleWrapper(PyObject* &pValue, double &ret);

    /**
     * @brief Converts a python object to bool, returns NULL otherwise
     * @param pValue PyObject to be converted
     * @param ret The converted boolean
     * @return True in case of success, false otherwise.
     */
    bool boolWrapper(PyObject* &pValue, bool &ret);

public:
    FakePythonSpeechTranscription();
    virtual ~FakePythonSpeechTranscription();
    FakePythonSpeechTranscription(const FakePythonSpeechTranscription&) = delete;
    FakePythonSpeechTranscription(FakePythonSpeechTranscription&&) = delete;
    FakePythonSpeechTranscription& operator=(const FakePythonSpeechTranscription&) = delete;
    FakePythonSpeechTranscription& operator=(FakePythonSpeechTranscription&&) = delete;

    bool open(yarp::os::Searchable& config) override;
    bool close() override;

    virtual bool setLanguage(const std::string& language) override;
    virtual bool getLanguage(std::string& language) override;
    virtual bool transcribe(const yarp::sig::Sound& sound, std::string& transcription, double& score) override;
};

#endif

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

    const char * m_moduleName = "Module";
    const char * m_path = "/home/ecub_docker/yarp/src/devices/fake/fakeSpeechTranscription";
    std::string m_className = "SpeecTranscriptor";
    PyObject * m_classInstance;

    bool functionWrapper(std::string moduleName, std::string functionName, PyObject* &pArgs, PyObject* &pValue);
    bool classWrapper(PyObject* &pClassInstance, std::string methodName, PyObject* &pClassMethodArgs, PyObject* &pValue);
    bool classInstanceCreator(std::string moduleName, std::string className, PyObject* &pClassArgs, PyObject* &pReturn);
    
    bool test(std::string dummy);
    /* Wrappers for converting a returned pValue to common data types*/
    std::string stringWrapper(PyObject* &pValue);
    long intWrapper(PyObject* &pValue);
    double doubleWrapper(PyObject* &pValue);
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

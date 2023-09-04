/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef __CONVERSATION_CALLBACK_H_
#define __CONVERSATION_CALLBACK_H_

#include <yarp/os/Bottle.h>
#include <yarp/os/TypedReaderCallback.h>
#include <QObject>

class ConversationCallback : public QObject, public yarp::os::TypedReaderCallback<yarp::os::Bottle>
{
private:
    Q_OBJECT;

public:
    ConversationCallback(QObject* parent = nullptr) :
            QObject(parent)
    {
    }
    void onRead(yarp::os::Bottle&) override;

signals:
    void conversationChanged();
};

#endif

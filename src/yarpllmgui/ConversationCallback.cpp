/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ConversationCallback.h"

void ConversationCallback::onRead(yarp::os::Bottle& input)
{
    emit conversationChanged();
}

/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef CONVERSATIONMODEL_H
#define CONVERSATIONMODEL_H

#include <yarp/os/Bottle.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Property.h>

#include <yarp/dev/ILLM.h>
#include <yarp/dev/PolyDriver.h>

#include <ConversationCallback.h>
#include <QAbstractListModel>

/**
 * Inner class for message representation in ConversationModel.
 */
class Message
{
public:
    Message(const QString& type, const QString& content) :
            m_type {type}, m_content {content}
    {
    }

    QString type() const
    {
        return m_type;
    }
    QString content() const
    {
        return m_content;
    }

private:
    QString m_type;
    QString m_content;
};

/**
 * The model of the conversation data in the ui.

 * It calls the llm_nwc functions in a way that allows the user to modify
 * the conversation.

 * It allows the user to modify the configuration from the UI.

 * It has an inner callback mechanism that refreshes the UI if the conversation
 * is changed outside of it.
 *
*/
class ConversationModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ConversationRoles
    {
        TypeRole = Qt::UserRole + 1,
        ContentRole
    };

    explicit ConversationModel(QObject* parent = nullptr) :
            QAbstractListModel(parent)
    {
        m_prop.put("device", "LLM_nwc_yarp");
        m_prop.put("local", m_local_rpc_port);
    }

    ~ConversationModel()
    {
        m_conversation_port.close();
        m_poly.close();
    }

    /**
     * Configures the remote rpc and streaming port.
     * @param remoteRpc name of the rpc port opened by the llm_nws
     * @param remote_streaming_port_name name of the streaming port opened by the llm nws.
     * It is used by the model callback.
     * @return True if connection to the port and/or device opening is successful, false otherwise.
     */
    bool configure(const std::string& remote_rpc, const std::string& streaming_port_name);

    // QAbstractListModel overrides
    QVariant data(const QModelIndex& index, int role) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * Deletes the conversation. Invokable from the ui.
     */
    Q_INVOKABLE void deleteConversation();
    Q_INVOKABLE void eraseMessage(const int& index);

    /**
     * Sends a message. Invokable from the ui.
     * @param message. The message to send
     */
    Q_INVOKABLE void sendMessage(const QString& message);

    /**
     * Configures the rpc and streaming port. Invokable from the ui.
     * @param remoteRpc name of the rpc port opened by the llm_nws
     * @param remote_streaming_port_name name of the streaming port opened by the llm nws.
     */
    Q_INVOKABLE void configure(const QString& remote_rpc, const QString& streaming_port_name);

public slots:

    /**
     * Deletes and reuploads the conversation. This way the data that we keep locally is
     * always fully synchronized with the nws.
     */
    void refreshConversation();

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    yarp::dev::PolyDriver m_poly;
    yarp::os::Property m_prop;

    yarp::dev::ILLM* m_iLlm = nullptr;
    ConversationCallback m_callback;
    QList<Message> m_conversation;
    yarp::os::BufferedPort<yarp::os::Bottle> m_conversation_port;
    const std::string m_conversation_port_name = "/llmgui/conv:i";
    const std::string m_local_rpc_port = "/yarpllmgui/rpc";
    const std::string m_no_device_message = "yarpLLMGui: no LLM_nwc was found. Operation is not allowed.";

    void setRemoteRpc(const std::string& remoteRpc);
    bool setRemoteStreamingPort(const std::string& remote_streaming_port_name);
    bool setInterface();
    void addMessage(const Message& message);
    void eraseConversation();
};

#endif // CONVERSATIONMODEL_H

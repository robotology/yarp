/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <ConversationModel.h>
#include <iostream>

void ConversationModel::setRemoteRpc(const std::string& remoteRpc)
{
    m_prop.put("remote", remoteRpc);
}

bool ConversationModel::setRemoteStreamingPort(const std::string& remote_streaming_port_name)
{

    bool ret = false;

    m_conversation_port.useCallback(m_callback);

    if (!m_conversation_port.open(m_conversation_port_name)) {
        yWarning() << "Cannot open local streaming port " + m_conversation_port_name;
        return ret;
    }
    ret = yarp::os::Network::connect(remote_streaming_port_name, m_conversation_port_name);
    QObject::connect(&m_callback, &ConversationCallback::conversationChanged, this, &ConversationModel::refreshConversation);

    return ret;
}

bool ConversationModel::setInterface()
{
    if (!m_poly.open(m_prop)) {
        yWarning() << "Cannot open LLM_nwc_yarp";
        return false;
    }

    if (!m_poly.view(m_iLlm)) {
        yWarning() << "Cannot open interface from driver";
        return false;
    }

    return true;
}

bool ConversationModel::configure(const std::string& remote_rpc, const std::string& streaming_port_name)
{
    bool ret = false;

    if (!remote_rpc.empty()) {
        setRemoteRpc(remote_rpc);
        ret = setInterface();
    }

    if (!streaming_port_name.empty()) {
        if (!m_conversation_port.isClosed()) {
            m_conversation_port.close();
        }
        ret = setRemoteStreamingPort(streaming_port_name);
    }

    // We set at least the remote_rpc or streaming_port_name so we try to refresh.
    // This is consistent with what the user expects when making a succesfull configuration.
    if (ret) {
        refreshConversation();
    }

    return ret;
}

void ConversationModel::configure(const QString& remote_rpc, const QString& streaming_port_name)
{
    configure(remote_rpc.toStdString(), streaming_port_name.toStdString());
}

QVariant ConversationModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() >= m_conversation.count()) {
        return QVariant();
    }

    const Message& message = m_conversation[index.row()];
    if (role == TypeRole) {
        return message.type();
    }
    else if (role == ContentRole) {
        return message.content();
    }

    return QVariant();
}

int ConversationModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_conversation.count();
}

QHash<int, QByteArray> ConversationModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TypeRole] = "type";
    roles[ContentRole] = "content";
    return roles;
}

void ConversationModel::eraseConversation()
{
    beginRemoveRows(QModelIndex(), 0, rowCount());
    m_conversation.clear();
    endRemoveRows();
}

void ConversationModel::deleteConversation()
{
    if (!m_iLlm) {
        yError() << m_no_device_message;
        return;
    }

    m_iLlm->deleteConversation();
    refreshConversation();
}

void ConversationModel::refreshConversation()
{

    this->eraseConversation();

    if (!m_iLlm) {
        yError() << m_no_device_message;
        return;
    }

    std::vector<yarp::dev::LLM_Message> conversation;

    if (!m_iLlm->getConversation(conversation)) {
        yError() << "Unable to get conversation";
        return;
    }

    for (const auto& message : conversation) {
        addMessage(Message(QString::fromStdString(message.type), QString::fromStdString(message.content)));
    }
}

void ConversationModel::addMessage(const Message& message)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_conversation << message;
    endInsertRows();
}

void ConversationModel::sendMessage(const QString& message)
{

    yarp::dev::LLM_Message answer;

    if (!m_iLlm) {
        yError() << m_no_device_message;
        addMessage(Message(QString::fromStdString("User"), QString::fromStdString(m_no_device_message)));
        addMessage(Message(QString::fromStdString("User"), QString::fromStdString("Please use the menu from the gui to add a valid connection to an existing LLM_nws")));
        return;
    }

    if (rowCount() == 0) {
        m_iLlm->setPrompt(message.toStdString());
    } else {
        m_iLlm->ask(message.toStdString(), answer);
    }

    this->refreshConversation();
}

void ConversationModel::eraseMessage(const int& index)
{
    beginRemoveRows(QModelIndex(), index, index);
    m_conversation.removeAt(index);
    endRemoveRows();
}

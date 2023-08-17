#ifndef CONVERSATIONMODEL_H
#define CONVERSATIONMODEL_H

#include <yarp/os/Log.h>
#include <yarp/os/LogStream.h>
#include <yarp/os/Property.h>

#include <yarp/dev/ILLM.h>
#include <yarp/dev/PolyDriver.h>

#include <QAbstractListModel>

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
    }

    ~ConversationModel() {  m_poly.close(); }

    // Configure rpc port names and configure the polydriver 
    void setLocalRpc(const std::string& localRpc);
    void setRemoteRpc(const std::string& remoteRpc);
    void configure();

    QVariant data(const QModelIndex& index, int role) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    void addMessage(const Message& message);
    void refreshConversation(); // Added so that we don't rely on keeping local data in sync.
    void eraseConversation();

    // bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    // Qt::ItemFlags flags(const QModelIndex &index) const override;

    Q_INVOKABLE void deleteConversation(); // TODO: We have to come up with better names for this one
    Q_INVOKABLE void eraseMessage(const int& index);
    Q_INVOKABLE void sendMessage(const QString& message);

protected:
    QHash<int, QByteArray> roleNames() const;

private:
    yarp::dev::PolyDriver m_poly;
    yarp::os::Property m_prop;

    yarp::dev::ILLM* m_iLlm;
    QList<Message> m_conversation;
    const std::string m_no_device_message = "yarpLLMGui: no LLM_nwc was found. Operation is not allowed.";
    
};

#endif // CONVERSATIONMODEL_H
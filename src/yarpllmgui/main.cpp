#include "ConversationModel.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickView>
#include <QQmlContext>
#include <yarp/os/ResourceFinder.h>

/**
 * Main file for yarpllmgui. The GUI should be invoked with the following parameters
 * @remote_rpc name of the rpc port to attach to.
 * @local_rpc name of the rpc port that the gui exposes. 
 * Define it only if the name "/yarpllmgui/rpc" conflicts with an existing name
 * To define the parameters use the following utilities provided by the ResourceFinder.
*/

int main(int argc, char* argv[])
{

    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    qmlRegisterType<ConversationModel>("conversationmodel", 1, 0, "ConversationModel");
    ConversationModel aConversationModel;

    // Load configuration
    yarp::os::ResourceFinder rf;
    rf.setDefaultConfigFile("config.ini");
    rf.setDefaultContext("yarpllmgui");
    rf.configure(argc,argv);
    std::string local_rpc = rf.check("local_rpc",yarp::os::Value("/yarpllmgui/rpc")).asString();
    std::string remote_rpc = rf.check("remote_rpc",yarp::os::Value("/yarpgpt/rpc")).asString();

    aConversationModel.setLocalRpc(local_rpc);
    aConversationModel.setRemoteRpc(remote_rpc);
    aConversationModel.configure();
    aConversationModel.refreshConversation();

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("aConversationModel", &aConversationModel);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    int ret;
    try {
        ret = app.exec();
    }
    catch(const std::bad_alloc &){
        yError() << "Failure during application";
        return EXIT_FAILURE;
    }

    return ret;
}
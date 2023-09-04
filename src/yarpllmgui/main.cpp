/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/ResourceFinder.h>

#include <ConversationModel.h>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickView>

/**
 * Main file for yarpllmgui. The GUI can be invoked with the following parameters
 * @remote_rpc. Mandatory. Name of the rpc port to attach to.
 * @streaming_port Optional. Name of the streaming port opened by the nws (by default /llm/conv:o).
 * Allows to autorefresh the conversation in case it gets updated from outside the gui.
 *
 * These can also be specified within the gui after the opening. The gui will not work
 * until a valid remote_rpc has been configured.
 *
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
    rf.configure(argc, argv);
    std::string remote_rpc = rf.check("remote_rpc", yarp::os::Value("/yarpgpt/rpc")).asString();
    std::string streaming_port = rf.check("streaming_port", yarp::os::Value("/yarpllm/conv:o")).asString();

    // Configuration based on user's input from resource finder. Can be changed from ui.
    aConversationModel.configure(remote_rpc, streaming_port);
    aConversationModel.refreshConversation();

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("aConversationModel", &aConversationModel);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    int ret;
    try {
        ret = app.exec();
    } catch (const std::bad_alloc&) {
        yError() << "Failure during application";
        return EXIT_FAILURE;
    }

    return ret;
}

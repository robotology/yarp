/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

import QtQuick 2.15
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.15
import conversationmodel 1.0
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.3

ApplicationWindow {
    id: mainWindow
    width: 540
    height: 960
    visible: true

    ColumnLayout {
        anchors.fill: parent

        MenuBar {
            width: mainWindow.width // Make the menu bar span the width

            Menu {
                title: "Options"
                MenuItem {
                    text: "Configure"
                    onTriggered: {
                        var popupComponent = Qt.createComponent("ConfigurePopup.qml");
                        if (popupComponent.status === Component.Ready)
                        {
                            var popup = popupComponent.createObject(mainWindow);
                            popup.open();
                        } else {
                        console.log("Error loading component:", popupComponent.errorString());
                    }
                }
            }
        }
    }

    ListView {
        id: listView
        Layout.fillWidth: true
        Layout.fillHeight: true
        Layout.margins: pane.leftPadding + messageField.leftPadding
        displayMarginBeginning: 40
        displayMarginEnd: 40
        verticalLayoutDirection: ListView.TopToBottom
        spacing: 12
        model: aConversationModel
        delegate: Row {
            readonly property bool sentByUser: model.type == "user"
                readonly property bool isPrompt: model.type == "system"

                    id: messageRow
                    anchors.right: (sentByUser || isPrompt) ? listView.contentItem.right : undefined
                    spacing: 6

                    Rectangle {
                        id: messageBox
                        width: Math.min(messageText.implicitWidth + 24,
                        listView.width - (!sentByUser ? messageRow.spacing + listView.width/8 : 0))
                        height: messageText.implicitHeight + 24
                        color: isPrompt ? "green" : (sentByUser ? "lightgrey" : "steelblue")
                        opacity : 0.8
                        Label {
                            id: messageText
                            anchors.centerIn: parent
                            text: model.content
                            color: sentByUser ? "black" : "white"
                            anchors.fill: parent
                            anchors.margins: 12
                            wrapMode: Label.Wrap
                        }
                    }
                }

                ScrollBar.vertical: ScrollBar {}
            }

            Button {
                background: Rectangle {
                    implicitWidth: 60
                    implicitHeight: 60
                    opacity: 0.3
                    radius: 2
                }
                id: eraseConversation
                Layout.alignment: Qt.AlignRight | Qt.AlignBottom
                icon.source: "https://cdn3.iconfinder.com/data/icons/objects/512/Bin-512.png"
                opacity: hovered ? 0.6 : 0.3
                // MouseArea to detect hover
                MouseArea {
                    id: hoverArea
                    anchors.fill: parent
                    hoverEnabled: true
                    // Triggered when mouse enters the button
                    onEntered: {
                        popupText.text = "Erase conversation";
                        popup.x = eraseConversation.x - popup.width;
                        popup.y = eraseConversation.y - popup.height;
                        listView.opacity = 0.4;
                        popup.open();
                    }

                    // Triggered when mouse leaves the button
                    onExited: {
                        popup.close();
                        listView.opacity = 1;
                    }

                    onClicked: {
                        listView.model.deleteConversation();
                    }
                }
            }

            Pane {
                id: pane
                Layout.fillWidth: true

                RowLayout {
                    width: parent.width

                    TextArea {
                        id: messageField
                        Layout.fillWidth: true
                        placeholderText: qsTr("Compose message")
                        wrapMode: TextArea.Wrap
                        Keys.onReturnPressed: {
                            listView.model.sendMessage(messageField.text);
                            messageField.text = "";
                        }

                    }

                    Button {
                        id: sendButton
                        text: qsTr("Send")
                        enabled: messageField.length > 0
                        onClicked: {
                            listView.model.sendMessage(messageField.text);
                            messageField.text = "";
                        }
                    }
                }
            }

            Popup {
                id: popup
                modal: false
                width: 200
                height: 50
                focus: true

                Rectangle {
                    anchors.fill: parent
                    color: "lightgray"

                    Text {
                        id: popupText
                        anchors.centerIn: parent
                    }
                }
            }
        }

    }

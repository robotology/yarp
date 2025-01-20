/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.12
import QtQuick 2.15

Popup {
    id: configurePopup
    width: 500
    height: 300

    x: (mainWindow.width - width) / 2  // Center horizontally
    y: (mainWindow.height - height) / 2  // Center vertically

    ColumnLayout {

        width: configurePopup.width
        Layout.alignment: Qt.AlignHCenter

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            Text {
                text: "Remote rpc"
            }
            TextField {
                id: remote_rpc
                placeholderText: "/yarpllm/rpc"
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            Text {
                text: "Remote streaming port"
            }
            TextField {
                id: streaming_port
                placeholderText: "/yarpllm/conv:o"
            }
        }



    }

    Button {
        anchors.bottom : parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        text: "Confirm"
        onClicked: {
            // Handle the confirmation here
            listView.model.configure(remote_rpc.text, streaming_port.text)
            configurePopup.close();
        }
    }
}

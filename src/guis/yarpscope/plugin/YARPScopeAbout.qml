/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Window 2.1

Window {
    id: aboutDlg
    maximumHeight: 200
    minimumHeight: 200
    maximumWidth: 400
    minimumWidth: 400

    width: 400
    height: 200
    flags: Qt.Dialog
    modality: Qt.WindowModal
    title: "About YARP Port Scope"

    property string name: ""



    Rectangle{
        id: container
        anchors.fill: parent

        Label {
            id: lblName
            text: "YARP Port Scope"
            anchors.top: parent.top
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter
            font.bold: true
            font.pointSize: 29
        }

        Label {
            id: lblInfo
            height: 37
            wrapMode: TextEdit.WordWrap
            text: "A simple graphical user interface for visualizing the numerical content of a yarp port."
            horizontalAlignment: Text.AlignHCenter
            anchors.top: lblName.bottom
            anchors.topMargin: 10
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 40
            anchors.rightMargin: 40


        }



        Button {
            id: creditsBtn

            text: "Credits"
            anchors.top: lblInfo.bottom
            anchors.topMargin: 30
            anchors.leftMargin: 30
            anchors.left: container.left
            onClicked: {
                creditsDlg.visibility = Window.Windowed
            }
        }

        Button {
            id: licenseBtn

            text: "License"
            anchors.top: lblInfo.bottom
            anchors.topMargin: 30
            anchors.leftMargin: 10
            anchors.left: creditsBtn.right
            onClicked: {
                licenseDlg.visibility = Window.Windowed
            }
        }

        Button {
            id: closeBtn

            text: "Close"
            anchors.top: lblInfo.bottom
            anchors.topMargin: 30
            anchors.rightMargin: 30
            anchors.right: container.right
            onClicked: {
                aboutDlg.visibility = Window.Hidden
                licenseDlg.visibility = Window.Hidden
                creditsDlg.visibility = Window.Hidden
            }
        }


    }

    Window {
        id: licenseDlg
        maximumHeight: 400
        minimumHeight: 400
        maximumWidth: 500
        minimumWidth: 500

        width: 500
        height: 400
        flags: Qt.Dialog
        modality: Qt.NonModal
        title: "License"

        Rectangle{
            anchors.fill: parent

            TextArea {
                id: textArea1

                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: licenseCloseBtn.top
                anchors.topMargin: 10
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                anchors.bottomMargin: 10
                textFormat: Text.RichText

                text: "Copyright (C) 2012 Istituto Italiano di Tecnologia (IIT) "+
                "<br><br> " +
                "This program is free software: you can redistribute it and/or modify "+
                "it under the terms of the GNU General Public License as published by "+
                "the Free Software Foundation, either version 2 of the License, or "+
                "(at your option) any later version. "+
                "<br><br> " +
                "This program is distributed in the hope that it will be useful, "+
                "but WITHOUT ANY WARRANTY; without even the implied warranty of "+
                "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "+
                "GNU General Public License for more details. "+
                "<br><br> " +
                "You should have received a copy of the GNU General Public License "+
                "along with this program.  If not, see <html><style type='text/css'></style><a href='http://www.gnu.org/licenses/'>http://www.gnu.org/licenses/</a></html>."
                readOnly: true
            }

            Button{
                id: licenseCloseBtn

                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 10
                anchors.rightMargin: 10

                text: "Close"

                onClicked: {
                    licenseDlg.visibility = Window.Hidden
                }

            }

        }

    }

    Window {
        id: creditsDlg
        maximumHeight: 200
        minimumHeight: 200
        maximumWidth: 300
        minimumWidth: 300
        width: 300
        height: 200
        flags: Qt.Dialog
        modality: Qt.NonModal
        title: "Credits"

        Rectangle{
            id: rect
            anchors.fill: parent

            TabView{
                id: tab
                parent: rect
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: creditsCloseBtn.top
                anchors.topMargin: 10
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                anchors.bottomMargin: 10

                Component.onCompleted: {
                    //addTab("Written By", Qt.createComponent("qrc:/YARPView/Credits.qml"))
                }
            }




            Button{
                id: creditsCloseBtn

                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 10
                anchors.rightMargin: 10

                text: "Close"

                onClicked: {
                    creditsDlg.visibility = Window.Hidden
                }

            }

        }

    }


}

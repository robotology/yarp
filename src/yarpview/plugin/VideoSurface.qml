/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

import robotology.yarp.view 1.0
import QtQuick 2.2
import QtMultimedia 5.0
import QtQuick.Dialogs 1.1
import QtQuick.Controls 1.1
import QtQuick.Window 2.1


Rectangle {
    id: maincontainer
    color: "black"
    property string moduleTitle: "YARP Qt Image Viewer"

    property int posX: yarpViewCore.posX
    property int posY: yarpViewCore.posY
    property int windowWidth: yarpViewCore.windowWidth
    property int windowHeight: yarpViewCore.windowHeight
    property YARPViewStatusBar dataArea: undefined
    property int menuHeight: 0
    property string name: "yarpview"
    property string version: "2.0"
    property bool showPixelCol: false
    property bool rightClickOn: false

    signal changeWindowSize(int w, int h)
    signal synchRate(bool check);
    signal autosize(bool check);
    signal setName(string name);
    signal saveSetClosed(bool check);
    signal saveSingleClosed(bool check);
    signal rightClickEnabled(bool enabled);

    /*********Connections*********/
    Connections{
        target: yarpViewCore
        onSendPortFps:{
            if(dataArea != undefined){
                dataArea.avgFps = avg
                dataArea.minFps = min
                dataArea.maxFps = max
                dataArea.portSizeX = yarpViewCore.videoProducer.frameWidth
                dataArea.portSizeY = yarpViewCore.videoProducer.frameHeight
            }
        }
        onSendDisplayFps:{
            if(dataArea != undefined){
                dataArea.displayAvgFps = avg
                dataArea.displayMinFps = min
                dataArea.displayMaxFps= max
                dataArea.displaySizeX = width
                dataArea.displaySizeY = height
            }
        }

        onOptionsSet:{
            maincontainer.synchronizeRightEnabled();
        }

        onSynchRate:{
            synchRate(check);
        }

        onAutosize:{
            autosize(check);
        }

        onSetName:{
            setName(name)
        }

        onWidthChanged:{
            //console.log("onWidthChanged")
        }

        onHeightChanged:{
            //console.log("onHeightChanged")
        }

        onSizeChanged:{
            //console.log("onSizeChanged")
            setOriginalSize()
        }
    }

    Connections{
        target: setIntervalDlg
        onOk:{
            yarpViewCore.changeRefreshInterval(value)
            setIntervalDlg.visible = false
        }
        onCancel:{
            setIntervalDlg.visible = false
        }
    }

    Connections{
        target: saveSingleImageDlg

        onSetFileName:{
            yarpViewCore.setFileName(fileName)
        }

        onSaveFrame:{
            yarpViewCore.saveFrame()
        }
    }

    Connections{
        target: saveSetImageDlg

        onSaveFrameSet:{
            yarpViewCore.startDumpFrames()
        }

        onStopSavingFrameSet:{
            yarpViewCore.stopDumpFrames()
        }

        onSetFileName: {
            yarpViewCore.setFilesName(fileName)
        }
    }
    /******************************/
    /******************************/
    /******************************/

    /********Functions************/
    function enableColorPicking(flag){
        coordsMouseArea.hoverEnabled = flag;
        maincontainer.showPixelCol = flag;
    }

    function enableRightClick(flag){
        maincontainer.rightClickOn = flag;
    }

    function synchronizeRightEnabled(){
        rightClickOn = yarpViewCore.rightClickEnabled();
        rightClickEnabled(yarpViewCore.rightClickEnabled());
    }

    function parseParameters(params){
        var ret = yarpViewCore.parseParameters(params)
        return ret
    }

    function setMenuHeight(h){
        menuHeight = h;
    }


    function freeze(check){
        yarpViewCore.freeze(check)
    }

    function setOriginalSize(){
        if(yarpViewCore.videoProducer.frameWidth === 0 ||
                yarpViewCore.videoProducer.frameHeight === 0){
            return;
        }
        windowWidth = yarpViewCore.videoProducer.frameWidth
        windowHeight = yarpViewCore.videoProducer.frameHeight + (dataArea !== undefined ? dataArea.height:0) + menuHeight
        changeWindowSize(windowWidth,windowHeight)
    }

    function setOriginalAspectRatio(){
        if(yarpViewCore.videoProducer.frameWidth === 0 ||
                yarpViewCore.videoProducer.frameHeight === 0){
            return;
        }

        var originalRatio = yarpViewCore.videoProducer.frameWidth/yarpViewCore.videoProducer.frameHeight
        if(width > height){
            windowWidth = width

           /* if(windowHeight == (width / originalRatio) + (dataArea !== undefined ? dataArea.height:0) + menuHeight){
                windowHeight = (width / originalRatio) + 81
            }*/

            windowHeight = (width / originalRatio) + (dataArea !== undefined ? dataArea.height:0) + menuHeight
        }else{
           /* if(windowHeight == height + (dataArea !== undefined ? dataArea.height:0) + menuHeight){
                windowHeight = height + 81
            }*/
            windowHeight = height + (dataArea !== undefined ? dataArea.height:0) + menuHeight

            if(windowWidth == height * originalRatio){
                windowWidth = (height * originalRatio)+1
            }
            windowWidth = height * originalRatio
        }

        changeWindowSize(windowWidth,windowHeight)

    }

    function changeRefreshInterval(){
        setIntervalDlg.visibility = Window.Windowed
    }

    function synchDisplayPeriod(checked){
        yarpViewCore.synchDisplayPeriod(checked)
    }

    function synchDisplaySize(checked){
        yarpViewCore.synchDisplaySize(checked)
    }

    function saveSingleImage(checked){
        if(checked)
            saveSingleImageDlg.visibility = Window.Windowed
        else
            saveSingleImageDlg.visibility = Window.Hidden
    }

    function saveSetImages(checked){
        if(checked)
            saveSetImageDlg.visibility = Window.Windowed
        else
            saveSetImageDlg.visibility = Window.Hidden
    }

    function about(){
        aboutDlg.visibility = Window.Windowed
    }

    /******************************/
    /******************************/
    /******************************/

    QtYARPViewPlugin{
        id: yarpViewCore
    }

    VideoOutput{
        anchors.fill: parent
        id: mainVideoOutput
        source: yarpViewCore.videoProducer
        fillMode: VideoOutput.Stretch

        MouseArea{
            anchors.fill: parent
            id: coordsMouseArea
            acceptedButtons: maincontainer.rightClickOn ? Qt.LeftButton | Qt.RightButton : Qt.LeftButton
            property var clickX
            property var clickY
            property var startclickX
            property var startclickY
            property var lastclickX
            property var lastclickY
            property var currX
            property var currY
            property bool pressing: false

            /*! \brief Converts a pair of mouse XY coordinates to video frame coordinates
             *  \param x Integer: The mouse X coordinate
             *  \param y Integer: The mouse Y coordinate
             *  \return toReturn Dictionary: It contains the X and Y values of the newly computed video frame coordinates
             */
            function coordsConverter(x,y){

                var frameW = yarpViewCore.videoProducer.frameWidth;
                var frameH = yarpViewCore.videoProducer.frameHeight;

                var w = mainVideoOutput.width;
                var h = mainVideoOutput.height;

                var ratioW = w/frameW;
                var ratioH = h/frameH;

                var toReturn = {"x":0,"y":0};
                toReturn["x"] = Math.round(x/ratioW);
                toReturn["y"] = Math.round(y/ratioH);

                return toReturn;
            }

            onClicked: {
                startclickX = mouse.x
                startclickY = mouse.y
                var clickCoords = coordsConverter(mouse.x,mouse.y);

                clickX = clickCoords["x"];
                clickY = clickCoords["y"];
                if(mouse.button === Qt.LeftButton){
                    yarpViewCore.clickCoords_2(clickX,clickY);
                }
                if(mouse.button === Qt.RightButton){
                    yarpViewCore.rightClickCoords_2(clickX,clickY);
                }
            }

            onPressed: {
                startclickX = mouse.x
                startclickY = mouse.y
                startclickX = mouse.x
                startclickY = mouse.y
                var clickCoords = coordsConverter(mouse.x,mouse.y);

                clickX = clickCoords["x"];
                clickY = clickCoords["y"];

                if(mouse.button === Qt.LeftButton){
                    canvasOverlay.lineColor = "red";
                }
                if(mouse.button === Qt.RightButton){
                    canvasOverlay.lineColor = "green";
                }
            }

            onPressAndHold: {
                pressing = true;
                canvasOverlay.requestPaint()
            }

            onReleased: {
                if (pressing)
                {
                    pressing = false;
                    var clickCoords = coordsConverter(mouse.x,mouse.y);

                    lastclickX = clickCoords["x"];
                    lastclickY = clickCoords["y"];

                    if(mouse.button === Qt.LeftButton){
                        yarpViewCore.clickCoords_4(clickX,clickY,lastclickX,lastclickY);
                    }
                    if(mouse.button === Qt.RightButton){
                        yarpViewCore.rightClickCoords_4(clickX,clickY,lastclickX,lastclickY);
                    }
                }
                canvasOverlay.requestPaint();
            }

            onPositionChanged:
            {
                if(maincontainer.showPixelCol){
                    var coords = coordsConverter(mouse.x,mouse.y);
                    dataArea.pixelXStr = ""+coords["x"];
                    dataArea.pixelYStr = ""+coords["y"];
                    dataArea.setPixelVal(yarpViewCore.getPixelAsStr(coords["x"],coords["y"]));
                }
                if(pressed){
                    currX=mouse.x;
                    currY=mouse.y;
                    canvasOverlay.requestPaint()
                }
            }

            Canvas
            {
                id:canvasOverlay
                width: parent.width
                height: parent.height
                anchors.fill: parent
                visible: true
                property string lineColor: "red"

                onPaint:
                {
                    var ctx = canvasOverlay.getContext("2d")
                    ctx.clearRect(0, 0, canvasOverlay.width, canvasOverlay.height);
                    if (coordsMouseArea.pressing)
                    {
                        ctx.lineWidth = 1
                        ctx.strokeStyle = lineColor
                        ctx.beginPath()
                        ctx.moveTo(coordsMouseArea.startclickX,coordsMouseArea.startclickY)
                        ctx.lineTo(coordsMouseArea.currX,coordsMouseArea.currY)
                        ctx.closePath()
                        ctx.stroke()
                    }
                }
            }
        }
    }



    /************ Dialogs **************/
    Window {
        id: saveSetImageDlg
        width: 275
        height: 100
        flags: Qt.Dialog
        modality: Qt.NonModal
        title: "Save single Frame"

        signal saveFrameSet()
        signal stopSavingFrameSet()
        signal setFileName(url fileName)

        onClosing: {
            saveSetClosed(false);
        }

        function save(){
            saveFrameSet()
        }

        function stop(){
            stopSavingFrameSet()
        }

        function path(fileName){
            setFileName(fileName)
        }

        Button {
            id: recordSetBtn
            text: qsTr("Record")
            iconSource: "qrc:/YARPView/record.svg"
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 5
            anchors.top: parent.top
            anchors.topMargin: 5
            anchors.left: parent.left
            anchors.leftMargin: 5
            onClicked: {
                saveSetImageDlg.save()
            }
        }

        Button {
            id: stopSetBtn
            text: qsTr("Stop")
            iconSource: "qrc:/YARPView/stop.svg"
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 5
            anchors.top: parent.top
            anchors.topMargin: 5
            anchors.left: recordSetBtn.right
            anchors.leftMargin: 5
            anchors.right: browseSet.left
            anchors.rightMargin: 5
            onClicked: {
                saveSetImageDlg.stop()
            }
        }


        Button {
            id: browseSet
            text: qsTr("...")
            anchors.right: parent.right
            anchors.rightMargin: 5
            anchors.top: parent.top
            anchors.topMargin: 5
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 5
            onClicked: {
                saveSetFileDlg.open()
            }
        }

        FileDialog{
            id: saveSetFileDlg
            selectExisting: false
            title: "File Set Name"

            onAccepted: {
                saveSetImageDlg.path(saveSetFileDlg.fileUrl)
                saveSetFileDlg.close()
            }
            onRejected: {
                saveSetFileDlg.close()
            }
        }

    }

    Window {
        id: saveSingleImageDlg
        width: 200
        height: 100
        flags: Qt.Dialog
        modality: Qt.NonModal
        title: "Save single Frame"

        signal saveFrame()
        signal setFileName(url fileName)

        onClosing: {
            saveSingleClosed(false);
        }

        function save(){
            saveFrame()
        }

        function path(fileName){
            setFileName(fileName)
        }

        Button {
            id: button1
            text: qsTr("Save")
            iconSource: "qrc:/YARPView/stop.svg"
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 5
            anchors.top: parent.top
            anchors.topMargin: 5
            anchors.left: parent.left
            anchors.leftMargin: 5
            onClicked: {
                saveSingleImageDlg.save()
            }
        }

        Button {
            id: button2
            text: qsTr("...")
            anchors.right: parent.right
            anchors.rightMargin: 5
            anchors.top: parent.top
            anchors.topMargin: 5
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 5
            onClicked: {
                saveFileDlg.open()
            }
        }

        FileDialog{
            id: saveFileDlg
            selectExisting: false
            title: "File Name"

            onAccepted: {
                saveSingleImageDlg.path(saveFileDlg.fileUrl)
                saveFileDlg.close()
            }
            onRejected: {
                saveFileDlg.close()
            }
        }

    }



    Window {
        id: setIntervalDlg
        width: 400
        height: 120
        flags: Qt.Dialog


        property int firstValue: 0

        signal ok(int value)
        signal cancel()

        function confirm(val)
        {
            ok(val)
        }
        function reject()
        {
            cancel()
        }

        SpinBox {
            id: spinBox1
            x: 300
            y: 26
            width: 92
            height: 26
            value: yarpViewCore.refreshInterval
            maximumValue: 1000
            minimumValue: 10
            focus: true
            Connections{
                target: spinBox1
                onEditingFinished:{
                    setIntervalDlg.confirm(spinBox1.value)
                }
            }
        }

        Label {
            id: label1
            x: 26
            y: 26
            width: 237
            height: 26
            color: "#000000"
            text: qsTr("Insert new Refresh Time (in msec)")
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }

        Button {
            id: okBtn
            x: 216
            y: 79
            text: "Ok"
            onClicked: {
                setIntervalDlg.confirm(spinBox1.value)
            }
        }

        Button {
            id: cancelBtn
            x: 307
            y: 79
            text: "Cancel"
            onClicked: {
                setIntervalDlg.reject()
                spinBox1.value = setIntervalDlg.firstValue
            }
        }

        onVisibleChanged: {
            if(visible){
                modality:  Qt.WindowModal
                firstValue = spinBox1.value
            }else{
                modality:  Qt.NonModal
            }
        }
    }


    Window {
        id: aboutDlg
        width: 400
        height: 200
        flags: Qt.Dialog
        modality: Qt.NonModal
        title: "About " + maincontainer.name



        Rectangle{
            id: container
            anchors.fill: parent

            Label {
                id: lblName
                text: maincontainer.name + " " + maincontainer.version
                anchors.top: parent.top
                anchors.topMargin: 20
                anchors.horizontalCenter: parent.horizontalCenter
                font.bold: true
                font.pointSize: 29
            }

            Label {
                id: lblInfo
                text: "Program to display images received on a port."
                anchors.top: lblName.bottom
                anchors.topMargin: 10
                anchors.horizontalCenterOffset: 0
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Text {
                id: link
                text: "<html><style type='text/css'></style><a href='http://www.yarp.it'>http://www.yarp.it</a></html>"
                anchors.top: lblInfo.bottom
                anchors.topMargin: 10
                anchors.horizontalCenterOffset: 0
                anchors.horizontalCenter: parent.horizontalCenter
                onLinkActivated: Qt.openUrlExternally(link)
            }

            Button {
                id: licenseBtn

                text: "License"
                anchors.top: link.bottom
                anchors.topMargin: 30
                anchors.leftMargin: 30
                anchors.left: container.left
                onClicked: {
                    licenseDlg.visibility = Window.Windowed
                }
            }

            Button {
                id: closeBtn

                text: "Close"
                anchors.top: link.bottom
                anchors.topMargin: 30
                anchors.rightMargin: 30
                anchors.right: container.right
                onClicked: {
                    aboutDlg.visibility = Window.Hidden
                    licenseDlg.visibility = Window.Hidden
                }
            }


        }


    }

    Window {
        id: licenseDlg
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

                text: "Released under the terms of the LGPLv2.1 or later, see LICENSE " +
                "The complete license description is contained in the " +
                "COPYING file included in this distribution." +
                "Please refer to this file for complete " +
                "information about the licensing of YARP.

                DISCLAIMERS: LICENSOR WARRANTS THAT THE COPYRIGHT IN AND TO THE " +
                "SOFTWARE IS OWNED BY THE LICENSOR OR THAT THE SOFTWARE IS " +
                "DISTRIBUTED BY LICENSOR UNDER A VALID CURRENT LICENSE. EXCEPT AS "+
                "EXPRESSLY STATED IN THE IMMEDIATELY PRECEDING SENTENCE, THE " +
                "SOFTWARE IS PROVIDED BY THE LICENSOR, CONTRIBUTORS AND COPYRIGHT "+
                "OWNERS AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED "+
                "INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, "+
                "FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO "+
                "EVENT SHALL THE LICENSOR, CONTRIBUTORS OR COPYRIGHT OWNERS BE "+
                "LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN "+
                "ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN"
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

    /******************************/
    /******************************/
    /******************************/
}

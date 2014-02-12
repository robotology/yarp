import QtQuick 2.0
import QtQuick.Controls 1.1

StatusBar {
    id: bar
    height: 60
    property string avgFps: "0.0"
    property string minFps: "0.0"
    property string maxFps: "0.0"

    property string displayAvgFps: "0.0"
    property string displayMinFps: "0.0"
    property string displayMaxFps: "0.0"

    property string name: "/name"

    function setName(name){
        bar.name = name
    }

    Label{
        id: fps
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: displayFps

        text: "Port: " + bar.avgFps +
              " (min:" + bar.minFps +
              " max:" + bar.maxFps +") fps"
    }

    Label{
        id: displayFps
        anchors.left: parent.left
        anchors.top: fps.bottom
        anchors.right: parent.right
        anchors.bottom: name

        text: "Display: " + bar.displayAvgFps +
              " (min:" + bar.displayMinFps +
              " max:" + bar.displayMaxFps +") fps"



    }

    Label{
        id: name
        anchors.left: parent.left
        anchors.top: displayFps.bottom
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        text: bar.name



    }
}

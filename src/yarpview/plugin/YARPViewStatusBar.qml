/*
 * Copyright (C) 2006-2020 Istituto Italiano di Tecnologia (IIT)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

import QtQuick 2.0
import QtQuick.Controls 1.1

StatusBar {
    id: bar
    height: 60
    property string avgFps: "0.0"
    property string minFps: "0.0"
    property string maxFps: "0.0"
    property string portSizeX: "0"
    property string portSizeY: "0"

    property string displayAvgFps: "0.0"
    property string displayMinFps: "0.0"
    property string displayMaxFps: "0.0"
    property string displaySizeX: "0"
    property string displaySizeY: "0"

    property string name: "/name"

    function setName(name){
        bar.name = name
    }

    Column{
        anchors.fill: parent
        spacing: 2

        Label{
            id: fps

            //anchors.bottom: displayFps.top

            text: "Port: " + bar.avgFps +
                  " (min:" + bar.minFps +
                  " max:" + bar.maxFps +") fps; " +
                  "(size: " + bar.portSizeX + "x" + bar.portSizeY + ")"
            fontSizeMode: Text.VerticalFit
            height: parent.height/3 - 2
        }

        Label{
            id: displayFps

            //anchors.bottom: name.top


            text: "Display: " + bar.displayAvgFps +
                  " (min:" + bar.displayMinFps +
                  " max:" + bar.displayMaxFps +") fps; "+
                  "(size: " + bar.displaySizeX + "x" + bar.displaySizeY + ")"
            fontSizeMode: Text.VerticalFit
            height: parent.height/3 - 2


        }

        Label{
            id: name


            text: bar.name
            fontSizeMode: Text.VerticalFit
            height: parent.height/3 - 2


        }
    }


}

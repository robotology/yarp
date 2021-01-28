/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

import QtQuick 2.0
import QtQuick.Controls 1.1


MenuBar {

    signal quit()
    signal about()
    signal clear()
    signal rescale()
    signal playPressed(bool pressed)




    Menu {

        title: "File"
        MenuItem { text: "Quit"
            onTriggered: {
                quit()
            }}
    }

    Menu {
        title: "Actions"
        MenuItem { id: startStop
            property bool started: true
                   text: started == false ? "Start" : "Stop"
                   //checkable: true
                   //checked: false
                   onTriggered: {
                       started = !started;
                       playPressed(started)
                   }}
        MenuItem { text: "Clear"
                    onTriggered: {
                        clear()
                    }}

        MenuItem { text: "AutoRescale"
                   onTriggered: {
                        rescale()
                   }}
    }
    Menu {
        title: "Help"
        MenuItem { text: "About..."
            onTriggered: {
                about()
                   }
        }
    }

}

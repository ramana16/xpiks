/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU Lesser General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.1
import QtQuick.Controls.Styles 1.1
import QtGraphicalEffects 1.0
import "../Constants"
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"
import "../Constants/UIConfig.js" as UIConfig

Item {
    id: donateComponent
    anchors.fill: parent

    z: 10000

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    function closePopup() {
        donateComponent.destroy()
    }

    Component.onCompleted: focus = true
    Keys.onEscapePressed: closePopup()

    PropertyAnimation { target: donateComponent; property: "opacity";
        duration: 400; from: 0; to: 1;
        easing.type: Easing.InOutQuad ; running: true }

    // This rectange is the a overlay to partially show the parent through it
    // and clicking outside of the 'dialog' popup will do 'nothing'
    Rectangle {
        anchors.fill: parent
        id: overlay
        color: "#000000"
        opacity: 0.6
        // add a mouse area so that clicks outside
        // the dialog window will not do anything
        MouseArea {
            anchors.fill: parent
        }
    }

    FocusScope {
        anchors.fill: parent

        MouseArea {
            anchors.fill: parent
            onWheel: wheel.accepted = true
            onClicked: mouse.accepted = true
            onDoubleClicked: mouse.accepted = true
        }

        RectangularGlow {
            anchors.fill: dialogWindow
            anchors.topMargin: glowRadius/2
            anchors.bottomMargin: -glowRadius/2
            glowRadius: 4
            spread: 0.0
            color: Colors.defaultControlColor
            cornerRadius: glowRadius
        }

        // This rectangle is the actual popup
        Rectangle {
            id: dialogWindow
            width: 400
            height: 400
            color: Colors.popupBackgroundColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            ColumnLayout {
                anchors.centerIn: parent
                spacing: 0
                signal donateLinkClicked()

                Image {
                    source: Colors.t + helpersWrapper.getSvgForTheme("qrc:/Graphics/Icon_donate_", settingsModel.selectedThemeIndex)
                    cache: false
                    width: 123
                    height: 115
                    //fillMode: Image.PreserveAspectFit
                    asynchronous: true
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Item {
                    height: 30
                }

                StyledText {
                    text: i18.n + qsTr("We hope you enjoy using Xpiks.")
                    anchors.horizontalCenter: parent.horizontalCenter
                    isActive: true
                    font.bold: true
                }

                Item {
                    height: 20
                }

                StyledText {
                    text: i18.n + qsTr("Today Xpiks needs Your help to become even better.")
                    anchors.horizontalCenter: parent.horizontalCenter
                    isActive: true
                    font.italic: true
                    font.pixelSize: UIConfig.fontPixelSize - 2
                }

                Item {
                    height: 50
                }

                StyledBlackButton {
                    implicitHeight: 30
                    height: 30
                    width: 150
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: i18.n + qsTr("Support Xpiks")
                    defaultForeground: Colors.goldColor
                    hoverForeground: Colors.buttonDefaultForeground
                    visible: true
                    onClicked: {
                        Qt.openUrlExternally(switcher.donateCampaign1Link)
                        switcher.setDonateCampaign1LinkClicked()
                    }
                }

                Item {
                    height: 20
                }

                StyledText {
                    text: i18.n + qsTr("Maybe later")
                    anchors.horizontalCenter: parent.horizontalCenter
                    isActive: dismissMA.pressed

                    MouseArea {
                        id: dismissMA
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        onClicked: closePopup()
                    }
                }
            }
        }
    }
}

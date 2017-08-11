/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

import QtQuick 2.2
import QtQuick.Dialogs 1.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import "../Constants"
import "../Components"
import "../StyledControls"

ApplicationWindow {
    id: aboutWindow
    modality: "ApplicationModal"
    width: 250
    height: Qt.platform.os === "linux" ? 200 : 190
    minimumWidth: width
    maximumWidth: width
    minimumHeight: height
    maximumHeight: height
    flags: Qt.Dialog
    title: i18.n + qsTr("About")

    signal dialogDestruction();
    onClosing: dialogDestruction();
    Component.onDestruction: dialogDestruction();

    function closeAbout() {
        aboutWindow.close();
    }

    Rectangle {
        color: Colors.popupBackgroundColor
        anchors.fill: parent

        Component.onCompleted: focus = true
        Keys.onEscapePressed: closeAbout()

        MouseArea {
            anchors.fill: parent
            onClicked: closeAbout()
        }

        ColumnLayout {
            anchors.centerIn: parent

            StyledText {
                color: Colors.inputForegroundColor
                anchors.horizontalCenter: parent.horizontalCenter
                text: i18.n + qsTr("Version: %1").arg(settingsModel.appVersion)
            }

            StyledText {
                focus: true
                anchors.horizontalCenter: parent.horizontalCenter
                text: i18.n + qsTr("<u>Xpiks webpage</u>")
                color: aboutMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor

                MouseArea {
                    id: aboutMA
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    hoverEnabled: true
                    onClicked: {
                        Qt.openUrlExternally("https://ribtoks.github.io/xpiks/")
                    }
                }
            }

            Item {
                height: 10
            }

            Item {
                height: Qt.platform.os === "linux" ? 100 : 80
                width: 220
                anchors.margins: 10

                StyledText {
                    color: Colors.inputForegroundColor
                    wrapMode: TextEdit.Wrap
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    text: i18.n + qsTr("The program is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE")
                }
            }
        }
    }
}

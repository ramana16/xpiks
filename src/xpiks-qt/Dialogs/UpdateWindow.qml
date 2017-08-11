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
    id: updateWindow
    modality: "ApplicationModal"
    width: 250
    height: 150
    minimumWidth: width
    maximumWidth: width
    minimumHeight: height
    maximumHeight: height
    flags: Qt.Dialog
    title: i18.n + qsTr("Update")

    property string updateUrl

    signal dialogDestruction();
    onClosing: dialogDestruction();

    function closeUpdateWindow() {
        updateWindow.close()
    }

    Rectangle {
        color: Colors.popupBackgroundColor
        anchors.fill: parent

        Component.onCompleted: focus = true
        Keys.onEscapePressed: closeUpdateWindow()

        MouseArea {
            anchors.fill: parent
            onClicked: closeUpdateWindow()
        }

        ColumnLayout {
            anchors.centerIn: parent

            StyledText {
                anchors.horizontalCenter: parent.horizontalCenter
                text: i18.n + qsTr("Xpiks update is available!")
            }

            StyledText {
                anchors.horizontalCenter: parent.horizontalCenter
                text: i18.n + qsTr("<u>Download Xpiks update</u>")
                color: downloadMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor

                MouseArea {
                    id: downloadMA
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        Qt.openUrlExternally(updateUrl)
                        closeUpdateWindow()
                    }
                }
            }
        }
    }
}

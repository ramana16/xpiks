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
import "../Constants"
import "../StyledControls"

Item {
    id: container
    width: childrenRect.width
    height: 24

    property bool isListLayout: true
    signal layoutChanged()
    property color currentColor: {
        var resultColor = Colors.selectedArtworkBackground
        if (enabled) {
            if (layoutMA.pressed) {
                resultColor = Colors.linkClickedColor
            } else if (layoutMA.containsMouse) {
                resultColor = Colors.labelActiveForeground
            } else {
                resultColor = Colors.selectedArtworkBackground
            }
        } else {
            resultColor = Colors.selectedArtworkBackground
        }

        return resultColor
    }

    Column {
        width: height
        height: parent.height
        anchors.left: parent.left
        anchors.top: parent.top
        visible: !isListLayout
        spacing: 5

        Repeater {
            model: 3
            delegate: Item {
                id: row
                width: parent.width
                height: parent.height / 3 - 3

                Rectangle {
                    anchors.left: parent.left
                    anchors.top: parent.top
                    height: parent.height
                    width: height
                    color: container.currentColor
                }

                Rectangle {
                    anchors.right: parent.right
                    anchors.top: parent.top
                    height: parent.height
                    width: row.width * 0.6
                    color: container.currentColor
                }
            }
        }
    }

    Grid {
        columns: 3
        rows: 3
        width: height
        height: parent.height
        visible: isListLayout
        columnSpacing: 5
        rowSpacing: 5

        Repeater {
            model: 9

            Rectangle {
                width: parent.width / 3 - 3
                height: parent.height / 3 - 3
                color: container.currentColor
            }
        }
    }

    StyledText {
        anchors.left: parent.left
        anchors.leftMargin: parent.height + 10
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: 2
        text: i18.n + (isListLayout ? qsTr("Grid") : qsTr("List"))
        color: container.currentColor
    }

    MouseArea {
        enabled: container.enabled
        id: layoutMA
        anchors.fill: parent
        hoverEnabled: true
        onClicked: layoutChanged()
    }
}

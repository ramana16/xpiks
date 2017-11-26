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
    height: 20

    property bool isListLayout: true
    signal layoutChanged()
    property color currentColor: {
        var resultColor = uiColors.selectedArtworkBackground
        if (enabled) {
            if (layoutMA.pressed) {
                resultColor = uiColors.linkClickedColor
            } else if (layoutMA.containsMouse) {
                resultColor = uiColors.labelActiveForeground
            } else {
                resultColor = uiColors.labelInactiveForeground
            }
        }

        return resultColor
    }

    Column {
        width: height
        height: parent.height
        anchors.left: parent.left
        anchors.top: parent.top
        visible: !isListLayout
        spacing: 4

        Repeater {
            model: 3
            delegate: Item {
                id: row
                width: parent.width
                height: 4

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
        columnSpacing: 4
        rowSpacing: 4

        Repeater {
            model: 9

            Rectangle {
                width: 4
                height: 4
                color: container.currentColor
            }
        }
    }

    CustomTooltip {
        id: customTooltip
        tooltipText: i18.n + (isListLayout ? qsTr("Grid") : qsTr("List"))
        withOwnMouseArea: false
    }

    MouseArea {
        enabled: container.enabled
        id: layoutMA
        anchors.fill: parent
        hoverEnabled: true
        onClicked: layoutChanged()
        onEntered: { customTooltip.trigger() }
        onExited: { customTooltip.dismiss() }
    }
}

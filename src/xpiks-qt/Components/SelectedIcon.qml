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

Item {
    id: item
    signal itemClicked();
    property bool isVisible: true
    property color linesColor: uiColors.whiteColor

    Item {
        anchors.fill: parent

        Rectangle {
            id: leftLine
            visible: isVisible
            color: linesColor
            width: parent.width * 0.55
            height: parent.width*0.1
            radius: height / 2
            transformOrigin: Item.TopLeft
            rotation: 235
            x: parent.width * 0.45 + radius
            y: parent.height * 0.8 + radius
        }

        Rectangle {
            visible: isVisible
            color: linesColor
            width: parent.width * 0.7
            radius: height / 2
            height: parent.width*0.1
            transformOrigin: Item.TopLeft
            rotation: 300
            x: parent.width * 0.45
            y: parent.height * 0.8
        }

        scale: mouseArea.pressed ? 0.8 : 1

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true
            preventStealing: true
            onClicked: {
                itemClicked()
            }
        }
    }
}


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
    property bool isActive

    signal itemClicked();

    Item {
        anchors.fill: parent
        opacity: mouseArea.containsMouse ? 1 : 0.8

        Rectangle {
            color: mouseArea.pressed ? Colors.artworkModifiedColor : (isActive ? Colors.defaultControlColor : Colors.inputForegroundColor)
            width: parent.width - 4
            height: 3
            border.width: 1
            border.color: color
            radius: 2
            transformOrigin: Item.Center
            rotation: 90
            anchors.centerIn: parent
        }

        Rectangle {
            color: mouseArea.pressed ? Colors.artworkModifiedColor : (isActive ? Colors.defaultControlColor : Colors.inputForegroundColor)
            width: parent.width - 4
            height: 3
            radius: 2
            border.width: 1
            border.color: color
            anchors.centerIn: parent
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


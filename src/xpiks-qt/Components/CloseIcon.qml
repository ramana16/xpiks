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
    property bool isActive: false
    property bool isPlus: false
    property int thickness: 3
    property alias crossOpacity: wrapperRect.opacity
    property color disabledColor: Colors.closeIconDisabledColor
    signal itemClicked();

    Item {
        id: wrapperRect
        anchors.fill: parent
        opacity: closeIconMouseArea.containsMouse ? 1 : 0.9

        property color pressColor: {
            if (closeIconMouseArea.containsMouse) {
                return Colors.artworkModifiedColor;
            } else {
                return item.isActive ? Colors.closeIconActiveColor : Colors.closeIconInactiveColor;
            }
        }

        Rectangle {
            color: enabled ? wrapperRect.pressColor : item.disabledColor
            width: isPlus ? parent.width - 2 : parent.width
            height: item.thickness
            radius: item.thickness/2
            transformOrigin: Item.Center
            rotation: isPlus ? 90 : 45
            transform: Translate { y: (width - thickness)/2 }
        }

        Rectangle {
            color: enabled ? wrapperRect.pressColor : item.disabledColor
            width: isPlus ? parent.width - 2 : parent.width
            height: item.thickness
            radius: item.thickness/2
            transformOrigin: Item.Center
            rotation: isPlus ? 0 : 135
            transform: Translate { y: (width - thickness)/2 }
        }

        scale: closeIconMouseArea.pressed ? 0.8 : 1

        MouseArea {
            id: closeIconMouseArea
            anchors.fill: parent
            hoverEnabled: true
            preventStealing: true
            onClicked: {
                itemClicked()
            }
        }
    }
}


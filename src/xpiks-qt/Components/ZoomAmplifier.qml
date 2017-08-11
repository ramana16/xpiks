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
    property bool isPlus: true
    property int thickness: 3
    property color lineColor: Colors.artworkActiveColor
    property real circleWidth: item.width*0.75
    property real plusOffset: 0.5

    Rectangle {
        id: circle
        width: item.circleWidth
        height: item.circleWidth
        radius: width/2
        border.width: item.thickness
        border.color: item.lineColor
        anchors.left: parent.left
        anchors.top: parent.top
        color: "transparent"

        Rectangle {
            id: plusHorizontal
            width: circle.width/2
            height: item.thickness
            radius: item.thickness
            color: item.lineColor
            anchors.centerIn: parent
            anchors.verticalCenterOffset: item.plusOffset
            anchors.horizontalCenterOffset: item.plusOffset
        }

        Rectangle {
            id: plusVertical
            visible: item.isPlus
            height: circle.width/2
            width: item.thickness
            radius: item.thickness
            color: item.lineColor
            anchors.centerIn: parent
            anchors.verticalCenterOffset: item.plusOffset
            anchors.horizontalCenterOffset: item.plusOffset
        }
    }

    Rectangle {
        width: circle.width*0.6
        height: item.thickness
        radius: item.thickness
        color: item.lineColor
        transformOrigin: Item.BottomRight
        rotation: 43
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }
}

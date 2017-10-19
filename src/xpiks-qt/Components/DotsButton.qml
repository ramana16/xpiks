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
    id: root
    property real dimension: 28
    width: dimension
    height: dimension
    property color dotsColor: dotsMA.pressed ? uiColors.whiteColor : (dotsMA.containsMouse ? uiColors.artworkActiveColor : uiColors.labelInactiveForeground)
    property color highlighColor: uiColors.inputBackgroundColor
    property real dotDimension: 4

    signal dotsClicked()

    Rectangle {
        width: root.dimension
        height: width
        anchors.centerIn: parent
        radius: parent.width/2
        color: root.highlighColor
        visible: dotsMA.containsMouse
        opacity: dotsMA.pressed ? 0.5 : 0.3
    }

    Item {
        id: innerRect
        width: root.width - 2*root.dotDimension
        height: root.height - 2*root.dotDimension
        anchors.centerIn: parent

        Rectangle {
            width: dotDimension
            height: dotDimension
            radius: width/2
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            color: root.dotsColor
        }

        Rectangle {
            width: dotDimension
            height: dotDimension
            radius: width/2
            anchors.centerIn: parent
            color: root.dotsColor
        }

        Rectangle {
            width: dotDimension
            height: dotDimension
            radius: width/2
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            color: root.dotsColor
        }
    }

    MouseArea {
        id: dotsMA
        hoverEnabled: true
        anchors.fill: parent
        onClicked: dotsClicked()
    }
}

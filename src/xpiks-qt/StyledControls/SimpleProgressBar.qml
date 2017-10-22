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
    id: progressbar

    property int minimum: 0
    property int maximum: 100
    property int value: 0
    property color color: "#77B753"
    property bool isRounded: true

    width: 250
    height: 23
    clip: true

    Rectangle {
        id: border
        anchors.fill: parent
        color: uiColors.defaultDarkColor
        border.width: 0
        border.color: parent.color
    }

    Rectangle {
        id: highlight
        property int widthDest: ( ( (progressbar.width + 20) * ( value - minimum ) ) / ( maximum - minimum ) - 4 )
        width: highlight.widthDest
        radius: progressbar.isRounded ? height / 2 : 0

        Behavior on width {
            SmoothedAnimation {
                velocity: 1200
            }
        }

        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            leftMargin: -10
            //margins: 1
        }

        color: parent.color
    }
}


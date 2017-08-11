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
    property bool isGreen

    Rectangle {
        border.width: 2
        border.color: Colors.inputForegroundColor
        radius: parent.width / 2
        anchors.fill: parent
        color: enabled ? (isGreen ? Colors.greenColor : Colors.destructiveColor) : Colors.selectedArtworkBackground

        Rectangle {
            visible: item.isGreen
            color: Colors.inputBackgroundColor
            width: parent.width * 0.7
            height: 2
            radius: 1
            transformOrigin: Item.Center
            rotation: 120
            transform: Translate { x: parent.width * 0.3; y: parent.height * 0.45 }
        }

        Rectangle {
            visible: item.isGreen
            color: Colors.inputBackgroundColor
            width: parent.width / 2
            radius: 1
            height: 2
            transformOrigin: Item.Center
            rotation: 65
            transform: Translate { x: parent.width * 0.15; y: parent.height * 0.55 }
        }
    }
}


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
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import "../Constants"

Slider {
    style: SliderStyle {
        groove: Rectangle {
            implicitWidth: 200
            implicitHeight: 4
            color: Colors.defaultControlColor
            radius: 4

            Rectangle {
                height: parent.height
                width: styleData.handlePosition
                implicitHeight: 4
                implicitWidth: 100
                radius: height/2
                color: Colors.artworkActiveColor
            }
        }
    }
}

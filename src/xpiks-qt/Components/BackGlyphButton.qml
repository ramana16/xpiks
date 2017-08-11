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
import xpiks 1.0
import "../Constants"
import "../StyledControls"

GlyphButton {
    id: buttonBack
    width: 100
    glyphVisible: true
    leftShift: -5
    glyphMargin: 12
    glyph: Item {
        width: childrenRect.width
        height: 14

        TriangleElement {
            id: triangle
            isVertical: true
            isFlipped: true
            width: 7
            height: 14
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            color: buttonBack.textColor
        }

        Rectangle {
            anchors.left: triangle.right
            anchors.verticalCenter: parent.verticalCenter
            color: buttonBack.textColor
            width: 6
            height: 6
        }
    }
}

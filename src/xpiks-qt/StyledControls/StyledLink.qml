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
import "../Constants/UIConfig.js" as UIConfig

StyledText {
    id: plainText
    property bool isActive: true
    property color normalLinkColor: uiColors.artworkActiveColor
    property color inactiveLinkColor: uiColors.labelInactiveForeground
    color: {
        var resultColor = normalLinkColor;

        if (!enabled) {
            resultColor = inactiveLinkColor
        } else if (isPressed) {
            resultColor = uiColors.linkClickedColor
        } else if (!isActive) {
            resultColor = uiColors.labelActiveForeground
        }

        return resultColor
    }
    property bool isPressed: linkMA.pressed

    signal clicked()

    MouseArea {
        id: linkMA
        anchors.fill: parent
        hoverEnabled: true
        preventStealing: true
        cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
        onClicked: {
            // strange bug with clicking on the keywords field
            if (!containsMouse) { return; }

            plainText.clicked()
        }
    }
}

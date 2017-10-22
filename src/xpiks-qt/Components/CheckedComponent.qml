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
    property color backgroundColor: uiColors.selectedArtworkBackground
    property bool isAnimating: false

    function startAnimation() {
        animateColor.start()
    }

    function stopAnimation() {
        animateColor.stop()
        host.border.color = uiColors.defaultControlColor
    }

    Rectangle {
        id: host
        border.width: 4
        border.color: uiColors.defaultControlColor
        radius: parent.width / 2
        anchors.fill: parent
        color: enabled ? (isGreen ? uiColors.greenColor : uiColors.destructiveColor) : item.backgroundColor

        SequentialAnimation {
            id: animateColor
            running: false
            loops: Animation.Infinite

            PauseAnimation { duration: 500 }

            PropertyAnimation {
                target: host
                properties: "border.color"
                from: uiColors.defaultControlColor
                to: uiColors.panelSelectedColor
                duration: 1000
                easing.type: Easing.InOutQuint
            }

            PauseAnimation { duration: 100 }

            PropertyAnimation {
                target: host
                properties: "border.color"
                from: uiColors.panelSelectedColor
                to: uiColors.defaultControlColor
                duration: 1000
                easing.type: Easing.InOutQuint
            }
        }
    }
}


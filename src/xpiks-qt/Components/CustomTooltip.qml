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

Item {
    id: tooltipComponent
    anchors.fill: parent
    property string tooltipText
    property bool showToolTip: false
    property bool withOwnMouseArea: true

    function trigger() {
        showTimer.start()
    }

    function dismiss() {
        showToolTip = false
        showTimer.stop();
    }

    Rectangle {
        id: toolTipRectangle
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.bottom
        anchors.topMargin: 10
        width: toolTipText.width + 30
        height: toolTipText.height + 12
        radius: 4
        opacity: tooltipText != "" && showToolTip ? 1 : 0
        color: uiColors.defaultDarkerColor

        Text {
            id: toolTipText
            text: tooltipText
            color: uiColors.labelInactiveForeground
            anchors.centerIn: parent
        }

        Behavior on opacity {
            PropertyAnimation {
                easing.type: Easing.InOutQuad
                duration: 250
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        enabled: withOwnMouseArea
        onEntered: { trigger() }
        onExited: { dismiss() }
        hoverEnabled: true
    }

    Timer {
        id: showTimer
        interval: 500
        onTriggered: showToolTip = true;
    }
}

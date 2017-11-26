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
import "../StyledControls"

Rectangle {
    id: control
    clip: false
    height: 35
    width: 35

    property real iconWidth: 27
    property real iconHeight: 27

    property alias hovered: toolButtonMA.containsMouse
    property alias pressed: toolButtonMA.pressed

    property string normalIcon
    property string disabledIcon
    property string hoveredIcon
    property string clickedIcon

    property alias tooltip: customTooltip.tooltipText

    signal clicked()

    color: {
        var result = "transparent";

        if (control.enabled) {
            if (control.hovered) {
                result = uiColors.defaultControlColor
            }
        }

        return result
    }

    Image {
        id: iconWrapper
        anchors.centerIn: parent
        source: {
            var result = control.normalIcon
            if (control.enabled) {
                if (toolButtonMA.containsMouse) {
                   if (toolButtonMA.pressed) {
                       result = control.clickedIcon
                   } else {
                       result = control.hoveredIcon
                   }
                }
            } else {
                result = control.disabledIcon
            }

            return result
        }

        width: control.iconWidth
        height: control.iconHeight
        fillMode: Image.PreserveAspectFit
        sourceSize.width: control.iconWidth
        sourceSize.height: control.iconHeight
        cache: true
    }

    CustomTooltip {
        id: customTooltip
        withOwnMouseArea: false
    }

    MouseArea {
        id: toolButtonMA
        anchors.fill: parent
        hoverEnabled: true
        onClicked: control.clicked()
        onEntered: { customTooltip.trigger() }
        onExited: { customTooltip.dismiss() }
    }
}

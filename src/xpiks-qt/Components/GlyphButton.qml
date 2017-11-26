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
    property bool isDefault: false
    clip: true
    height: 24
    property alias hovered: glyphButtonMA.containsMouse
    property alias pressed: glyphButtonMA.pressed
    property alias glyph: glyphWrapper.data
    property int leftShift: 0
    property bool glyphVisible: false
    property string text: ''
    property int glyphMargin: 10
    signal clicked()

    color: {
        var result = uiColors.buttonDefaultBackground;

        if (control.enabled &&
                (control.isDefault || control.hovered)) {
            result = uiColors.buttonHoverBackground;
        }

        return result
    }

    property color textColor: {
        var result = uiColors.defaultControlColor;

        if (control.enabled) {
            if (control.pressed) {
                result = uiColors.buttonPressedForeground
            } else {
                result = control.hovered ? uiColors.buttonHoverForeground : uiColors.buttonDefaultForeground;
            }
        } else {
            result = uiColors.buttonDisabledForeground;
        }

        return result
    }

    Item {
        id: innerItem
        anchors.horizontalCenterOffset: control.leftShift
        anchors.centerIn: parent
        width: childrenRect.width
        height: glyphWrapper.height

        Item {
            id: glyphWrapper
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            visible: control.glyphVisible
            height: childrenRect.height
            width: childrenRect.width
        }

        StyledText {
            anchors.left: control.glyphVisible ? glyphWrapper.right : parent.left
            anchors.leftMargin: control.glyphVisible ? control.glyphMargin : 0
            anchors.verticalCenter: parent.verticalCenter
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: control.text
            color: control.textColor
        }
    }

    MouseArea {
        id: glyphButtonMA
        anchors.fill: parent
        hoverEnabled: true
        onClicked: control.clicked()
    }
}

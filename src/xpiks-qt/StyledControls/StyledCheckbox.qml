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

CheckBox {
    property bool isContrast: false
    property int indicatorWidth: 17
    property int indicatorHeight: 17
    checked: true
    property color labelColor: enabled ? uiColors.labelActiveForeground : (isContrast ? uiColors.inputBackgroundColor : uiColors.labelInactiveForeground)
    property bool isBold: false

    style: CheckBoxStyle {
        spacing: 10

        indicator: Rectangle {
            implicitHeight: indicatorHeight
            implicitWidth: indicatorWidth
            color: (control.checked || control.hovered) ? uiColors.artworkActiveColor : (isContrast ? uiColors.defaultDarkColor : uiColors.defaultControlColor)
            opacity: control.hovered ? 0.6 : 1

            Rectangle {
                visible: control.checked
                color: uiColors.checkboxCheckedColor
                width: parent.width * 0.55
                height: parent.width*0.1
                radius: height / 2
                transformOrigin: Item.TopLeft
                rotation: 235
                x: parent.width * 0.45 + radius
                y: parent.height * 0.8 + radius
            }

            Rectangle {
                visible: control.checked
                color: uiColors.checkboxCheckedColor
                width: parent.width * 0.7
                radius: height / 2
                height: parent.width*0.1
                transformOrigin: Item.TopLeft
                rotation: 300
                x: parent.width * 0.45
                y: parent.height * 0.8
            }
        }

        label: StyledText {
            text: control.text
            color: control.labelColor
            font.bold: control.isBold
        }
    }
}

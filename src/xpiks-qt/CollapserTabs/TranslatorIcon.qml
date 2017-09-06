/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

import QtQuick 2.0
import QtQuick.Layouts 1.1
import "../Constants" 1.0
import "../Components"
import "../StyledControls"
import "../Dialogs"
import "../Common.js" as Common
import "../Constants/UIConfig.js" as UIConfig
import xpiks 1.0

Rectangle {
    id: translatorIconWrapper
    width: 24
    height: 20
    border.color: isHighlighted ? uiColors.labelActiveForeground : uiColors.inactiveControlColor
    border.width: 2
    color: "transparent"

    StyledText {
        text: "A"
        font.pixelSize: 12
        font.bold: true
        color: translatorIconWrapper.border.color
        anchors.centerIn: parent
        anchors.verticalCenterOffset: 1
    }
}

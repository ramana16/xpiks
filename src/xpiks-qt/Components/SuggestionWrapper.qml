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
import QtQuick.Layouts 1.1
import "../Constants"
import "../StyledControls"
import "../Constants/UIConfig.js" as UIConfig

Rectangle {
    id: itemWrapper
    property int delegateIndex
    property bool isSelected
    property string suggestionText
    property alias itemHeight: suggestionText.height

    signal actionClicked();

    color: isSelected ? uiColors.whiteColor : (innerMA.containsMouse ? uiColors.artworkActiveColor : uiColors.inactiveControlColor)

    width: suggestionText.width + 10
    height: childrenRect.height

    StyledText {
        id: suggestionText
        verticalAlignment: Text.AlignVCenter
        anchors.left: parent.left
        anchors.leftMargin: 5
        text: itemWrapper.suggestionText
        color: isSelected ? uiColors.inactiveControlColor : (innerMA.containsMouse ? uiColors.whiteColor : uiColors.labelActiveForeground)
        font.pixelSize: UIConfig.fontPixelSize * settingsModel.keywordSizeScale
    }

    MouseArea {
        id: innerMA
        anchors.left: parent.left
        anchors.top: parent.top
        width: suggestionText.width + 10
        height: suggestionText.height
        hoverEnabled: true
        onClicked: actionClicked()
        cursorShape: Qt.PointingHandCursor
    }
}

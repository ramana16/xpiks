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
    property bool isHighlighted
    property bool hasSpellCheckError: false
    property string keywordText
    property bool hasPlusSign: false
    property alias itemHeight: tagTextRect.height
    property alias closeIconDisabledColor: closeIcon.disabledColor

    signal removeClicked();
    signal actionDoubleClicked();
    signal actionRightClicked();
    signal spellSuggestionRequested();

    color: isHighlighted ? Colors.inputForegroundColor : Colors.inactiveKeywordBackground

    width: childrenRect.width
    height: childrenRect.height

    Row {
        id: row
        spacing: 0

        Item {
            id: tagTextRect
            width: childrenRect.width + 5
            height: itemHeight

            StyledText {
                id: keywordText
                anchors.left: parent.left
                anchors.leftMargin: 5 + (settingsModel.keywordSizeScale - 1)*10
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                verticalAlignment: Text.AlignVCenter
                text: itemWrapper.keywordText
                color: itemWrapper.isHighlighted ? Colors.inactiveControlColor : Colors.inactiveKeywordForeground
                font.pixelSize: UIConfig.fontPixelSize * settingsModel.keywordSizeScale
            }

            MouseArea {
                anchors.fill: keywordText
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onDoubleClicked: actionDoubleClicked()
                onClicked: {
                    if (mouse.button == Qt.RightButton) {
                        console.log("Click in keyword registered")
                        actionRightClicked()
                    }
                }

                preventStealing: true
                propagateComposedEvents: true
            }
        }

        Item {
            height: uiManager.keywordHeight
            width: height

            CloseIcon {
                id: closeIcon
                isPlus: itemWrapper.hasPlusSign
                width: 14*settingsModel.keywordSizeScale
                height: 14*settingsModel.keywordSizeScale
                isActive: itemWrapper.isHighlighted
                anchors.centerIn: parent
                onItemClicked: removeClicked()
            }
        }
    }

    Rectangle {
        height: 1.5
        anchors.left: row.left
        anchors.right: row.right
        anchors.bottom: row.bottom
        color: Colors.destructiveColor
        visible: itemWrapper.hasSpellCheckError
    }
}

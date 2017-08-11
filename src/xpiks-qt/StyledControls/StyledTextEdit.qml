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

TextEdit {
    id: textEditHost
    signal actionRightClicked();
    property string rightClickedWord
    property bool isActive: true
    property bool userDictEnabled: false
    font.family: Qt.platform.os === "windows" ? "Arial" : "Helvetica"
    font.pixelSize: UIConfig.fontPixelSize * settingsModel.keywordSizeScale
    verticalAlignment: TextInput.AlignVCenter
    selectedTextColor: Colors.inputForegroundColor
    selectionColor: Colors.inputInactiveBackground
    renderType: Text.NativeRendering
    selectByMouse: true
    cursorVisible: false
    wrapMode: TextEdit.NoWrap
    activeFocusOnPress: true
    color: (enabled && isActive) ? Colors.inputForegroundColor : Colors.inputInactiveForeground

    function isSeparator(position) {
        var separators = " ,.:;\\|<>()-";
        var symbol = text[position];
        return (separators.indexOf(symbol) >= 0);
    }

    function isRightBound(position, maxPos) {
        if (position >= maxPos) {
            return true;
        }

        return !isSeparator(position) && isSeparator(position + 1);
    }

    function getRightBound(position) {
        var cur = position;
        var maxPos = text.length

        while (!isRightBound(cur, maxPos - 1)) {
            cur++;
        }

        return cur + 1;
    }

    function isLeftBound(position) {
        if (position === 0) {
            return true;
        }

        return !isSeparator(position) && isSeparator(position - 1);
    }

    function getLeftBound(position) {
        var cur = position;
        while (!isLeftBound(cur)) {
            cur--;
        }

        return cur;
    }

    function getWordByPosition(textPosition) {
        var leftBound = getLeftBound(textPosition);
        var rightBound = getRightBound(textPosition);
        var word = getText(leftBound, rightBound);
        return word
    }

    MouseArea {
        enabled: textEditHost.enabled && textEditHost.userDictEnabled
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        propagateComposedEvents: true

        onClicked: {
            if (mouse.button == Qt.RightButton) {
                var textPosition = positionAt(mouse.x, mouse.y);
                var word = textEditHost.getWordByPosition(textPosition);

                console.log("Detected word under click: " + word);
                parent.rightClickedWord = word;

                if (word.length !== 0) {
                    actionRightClicked();
                }
            }
        }        
    }
}


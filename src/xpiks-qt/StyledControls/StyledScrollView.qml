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
import QtQuick.Dialogs 1.1
import QtQuick.Controls.Styles 1.1
import "../Constants"

ScrollView {
    activeFocusOnTab: false
    property color handleColor: uiColors.panelSelectedColor

    style: ScrollViewStyle {
        //transientScrollBars: true
        minimumHandleLength: 20

        handle: Item {
            implicitHeight: 10
            implicitWidth: 10

            Rectangle {
                anchors.fill: parent
                radius: 5
                color: control.handleColor
                opacity: styleData.pressed ? 1 : (styleData.hovered ? 0.9 : 0.8)
            }
        }

        scrollBarBackground: Item {
            property bool sticky: false
            property bool hovered: styleData.hovered
            implicitWidth: 10
            implicitHeight: 10
            clip: true

            onHoveredChanged: if (hovered) sticky = true
        }

        corner: Item {}
        decrementControl: Item { width: 1; height: 1 }
        incrementControl: Item { width: 1; height: 1 }
    }
}

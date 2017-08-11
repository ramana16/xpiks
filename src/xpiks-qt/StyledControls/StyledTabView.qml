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
import "../Components"
import "../Constants"

TabView {
    property double tabBarHeight: 1
    property double tabsHeight: 25
    property color backgroundColor: Colors.popupBackgroundColor

    style: TabViewStyle {
        frameOverlap: -tabBarHeight
        tabOverlap: -2

        frame: Rectangle {
            color: backgroundColor
        }

        leftCorner: Item {
            implicitWidth: 5
            implicitHeight: tabBarHeight
        }

        tabBar : Rectangle {
            color: backgroundColor

            Rectangle {
                anchors.bottom: parent.bottom
                height: tabBarHeight
                anchors.left: parent.left
                anchors.right: parent.right
                color: Colors.artworkActiveColor
            }
        }

        tab: Rectangle {
            color: styleData.selected ? backgroundColor : Colors.defaultControlColor
            implicitWidth: Math.max(text.width + 10, 80)
            implicitHeight: tabsHeight
            border.color: Colors.artworkActiveColor
            border.width: styleData.selected ? tabBarHeight : 0

            StyledText {
                id: text
                anchors.centerIn: parent
                anchors.verticalCenterOffset: styleData.selected ? 1 : -1
                text: styleData.title
                color: styleData.hovered ? Colors.artworkActiveColor : (styleData.selected ? Colors.inputForegroundColor : Colors.labelActiveForeground)
            }

            Rectangle {
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: styleData.selected ? 1 : 0
                anchors.rightMargin: styleData.selected ? 1 : 0
                color: styleData.selected ? backgroundColor : Colors.artworkActiveColor
                height: tabBarHeight
            }
        }
    }
}

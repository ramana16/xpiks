/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU Lesser General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.1
import QtQuick.Controls.Styles 1.1
import QtGraphicalEffects 1.0
import "../Constants"
import "../Common.js" as Common;
import "../StyledControls"

ColumnLayout {
    spacing: 0
    signal donateLinkClicked()

    Image {
        source: Colors.t + helpersWrapper.getSvgForTheme("qrc:/Graphics/Icon_donate_", settingsModel.selectedThemeIndex)
        cache: false
        width: 123
        height: 115
        //fillMode: Image.PreserveAspectFit
        asynchronous: true
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Item {
        height: 30
    }

    StyledText {
        text: i18.n + qsTr("Help us to make Xpiks even better.")
        anchors.horizontalCenter: parent.horizontalCenter
        isActive: true
    }

    Item {
        height: 10
    }

    StyledText {
        text: i18.n + qsTr("Support Xpiks")
        anchors.horizontalCenter: parent.horizontalCenter
        color: donateMA.pressed ? Colors.linkClickedColor : Colors.goldColor
        font.bold: true

        MouseArea {
            id: donateMA
            anchors.fill: parent
            cursorShape: Qt.PointingHandCursor
            onClicked: {
                Qt.openUrlExternally(switcher.donateCampaign1Link)
                switcher.setDonateCampaign1LinkClicked()
                donateLinkClicked()
            }
        }
    }
}

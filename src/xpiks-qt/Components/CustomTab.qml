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
import QtQuick.Controls 1.2
import "../Constants"
import "../StyledControls"

Rectangle {
    id: tab
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    width: 75
    color: isSelected ? Colors.defaultControlColor : Colors.defaultDarkColor
    property int tabIndex: -1
    property bool isSelected: false
    property bool hovered: false
}

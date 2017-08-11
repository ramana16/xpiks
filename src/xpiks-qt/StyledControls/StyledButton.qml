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

Button {
    property bool isDefault: false
    clip: true
    activeFocusOnTab: false

    style: ButtonStyle {
        background: Rectangle {
            implicitWidth: control.width
            implicitHeight: 24
            color: {
                var result = Colors.buttonDefaultBackground;

                if (control.enabled &&
                        (control.isDefault || control.hovered)) {
                    result = Colors.buttonHoverBackground;
                }

                return result
            }
        }

        label: StyledText {
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: control.text
            color: {
                var result = Colors.defaultControlColor;

                if (control.enabled) {
                    result = control.pressed ? Colors.buttonPressedForeground : Colors.buttonDefaultForeground;
                } else {
                    result = Colors.buttonDisabledForeground;
                }

                return result
            }
        }
    }
}

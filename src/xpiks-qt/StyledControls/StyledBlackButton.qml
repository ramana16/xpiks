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
    activeFocusOnTab: false
    property color defaultForeground: uiColors.buttonDefaultForeground
    property color hoverForeground: defaultForeground

    style: ButtonStyle {
        background: Rectangle {
            color: {
                var result = uiColors.defaultDarkColor;

                if (control.enabled &&
                        (control.isDefault || control.hovered)) {
                    result = uiColors.buttonHoverBackground;
                }

                return result
            }
            implicitWidth: control.width
            implicitHeight: 24
        }

        label: StyledText {
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: {
                var result = uiColors.defaultControlColor;

                if (control.enabled) {
                    result = control.pressed ? uiColors.buttonPressedForeground : (control.hovered ? control.hoverForeground : control.defaultForeground);
                } else {
                    result = uiColors.inactiveControlColor;
                }

                return result
            }
            text: control.text
        }
    }
}

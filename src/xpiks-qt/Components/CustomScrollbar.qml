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

Item {
    id: scrollbar;
    width: (handleSize + 2);
    visible: (flickable.visibleArea.heightRatio < 1.0);

    anchors {
        top: flickable.top;
        right: flickable.right;
        bottom: flickable.bottom;
        margins: 1;
    }

    property Flickable flickable               : null;
    property int       handleSize              : 10;

    Binding {
        target: handle;
        property: "y";
        value: Math.min(flickable.visibleArea.yPosition * scrollbar.height, clicker.drag.maximumY)
        when: (!clicker.drag.active);
    }

    Binding {
        target: flickable;
        property: "contentY";
        value: (handle.y * (flickable.contentHeight - flickable.height) / clicker.drag.maximumY);
        when: (clicker.drag.active || clicker.pressed);
    }

    Item {
        id: groove;
        clip: true;
        anchors {
            fill: parent;
            margins: 1
        }

        MouseArea {
            id: clicker;
            drag {
                target: handle;
                minimumY: 0;
                maximumY: (groove.height - handle.height);
                axis: Drag.YAxis;
            }
            anchors { fill: parent; }
            onClicked: { flickable.contentY = (mouse.y / groove.height * (flickable.contentHeight - flickable.height)); }
        }

        Item {
            id: handle;
            height: Math.max(flickable.visibleArea.heightRatio * scrollbar.height, 10)
            width: scrollbar.handleSize
            anchors {
                left: parent.left;
                right: parent.right;
            }

            Rectangle {
                id: backHandle
                anchors.fill: parent
                radius: 5
                color: Colors.artworkActiveColor
                opacity: clicker.pressed ? 1 : (clicker.containsMouse ? 0.9 : 0.8)
            }
        }
    }
}


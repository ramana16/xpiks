/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

import QtQuick 2.0
import "../StyledControls"

Item {
    id: screen
    property string imageUrl
    property string title: "title"
    opacity: 1
    clip: true

    function hide() {
        screen.state = 'hide';
        screen.x = 0;
    }

    function show(xVal) {
        screen.x = xVal;
        screen.state  = 'show';
    }

    Image {
        id: rect
        width: screen.width
        height: screen.height - 50
        source: screen.imageUrl
        //fillMode: Image.PreserveAspectFit
        asynchronous: true
        cache: false
    }

    Rectangle {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        color: uiColors.defaultDarkColor
        height: 50

        StyledText {
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            id: title
            text: screen.title;
        }
    }

    states: [
        State {
            name: "show"
            PropertyChanges {
                target: screen
                x: 0
                opacity: 1
            }
        },
        State {
            name: "hide"
            PropertyChanges {
                target: screen
                opacity: 0
            }
        }
    ]

    transitions: [
        Transition {
            from:"hide"
            to:"show"
            NumberAnimation {
                properties: "x";
                duration: 400;
                easing {
                    type: Easing.InCubic;
                }
            }
            NumberAnimation { properties: "opacity"; duration: 500 }
        },
        Transition {
            //from: "show"
            to: "hide"
            NumberAnimation { properties: "opacity"; duration: 500 }
        }
    ]
}

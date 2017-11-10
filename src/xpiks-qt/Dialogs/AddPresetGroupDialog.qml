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
import QtQuick.Dialogs 1.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import "../Constants"
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"

Item {
    id: addGroupComponent
    property var callbackObject
    property bool anyError: false
    anchors.fill: parent

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    function submitGroupName(groupName) {
        if (anyError) { return; }

        callbackObject.onSuccess(groupName)
        closePopup()
    }

    Keys.onEscapePressed: closePopup()

    function closePopup() {
        callbackObject.onClose()
        addGroupComponent.destroy()
    }

    PropertyAnimation { target: addGroupComponent; property: "opacity";
        duration: 400; from: 0; to: 1;
        easing.type: Easing.InOutQuad ; running: true }

    // This rectange is the a overlay to partially show the parent through it
    // and clicking outside of the 'dialog' popup will do 'nothing'
    Rectangle {
        anchors.fill: parent
        id: overlay
        color: "#000000"
        opacity: 0.6
        // add a mouse area so that clicks outside
        // the dialog window will not do anything
        MouseArea {
            anchors.fill: parent
        }
    }

    FocusScope {
        anchors.fill: parent

        MouseArea {
            anchors.fill: parent
            onWheel: wheel.accepted = true
            onClicked: mouse.accepted = true
            onDoubleClicked: mouse.accepted = true

            property real old_x : 0
            property real old_y : 0

            onPressed:{
                var tmp = mapToItem(addGroupComponent, mouse.x, mouse.y);
                old_x = tmp.x;
                old_y = tmp.y;
            }

            onPositionChanged: {
                var old_xy = Common.movePopupInsideComponent(addGroupComponent, dialogWindow, mouse, old_x, old_y);
                old_x = old_xy[0]; old_y = old_xy[1];
            }
        }

        RectangularGlow {
            anchors.fill: dialogWindow
            anchors.topMargin: glowRadius/2
            anchors.bottomMargin: -glowRadius/2
            glowRadius: 4
            spread: 0.0
            color: uiColors.defaultControlColor
            cornerRadius: glowRadius
        }

        // This rectangle is the actual popup
        Rectangle {
            id: dialogWindow
            width: 240
            height: childrenRect.height + 40
            color: uiColors.popupBackgroundColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            Column {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                height: childrenRect.height
                anchors.margins: 20
                spacing: 20

                Rectangle {
                    color: enabled ? uiColors.inputBackgroundColor : uiColors.inputInactiveBackground
                    border.width: groupNameInput.activeFocus ? 1 : 0
                    border.color: addGroupComponent.anyError ? uiColors.artworkModifiedColor : uiColors.artworkActiveColor
                    width: 200
                    height: 30
                    clip: true

                    StyledTextInput {
                        id: groupNameInput
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: 5
                        anchors.rightMargin: 5
                        anchors.verticalCenter: parent.verticalCenter
                        onAccepted: submitGroupName(groupNameInput.text)

                        Keys.onBacktabPressed: {
                            event.accepted = true
                        }

                        Keys.onTabPressed: {
                            event.accepted = true
                        }

                        Keys.onPressed: {
                            if (event.key === Qt.Key_Comma) {
                                event.accepted = true
                            }
                        }

                        onTextChanged: {
                            addGroupComponent.anyError = !presetsGroups.canAddGroup(text)
                        }
                    }
                }

                RowLayout {
                    width: parent.width
                    height: 20
                    spacing: 20

                    StyledButton {
                        text: i18.n + qsTr("Add")
                        width: 90
                        enabled: !addGroupComponent.anyError && (groupNameInput.text.trim().length > 2)
                        onClicked: submitGroupName()
                    }

                    StyledButton {
                        text: i18.n + qsTr("Cancel")
                        width: 90
                        onClicked: closePopup()
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        focus = true
        groupNameInput.forceActiveFocus()
    }
}

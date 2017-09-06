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
import "../Constants/UIConfig.js" as UIConfig

Item {
    id: enterPasswordComponent
    property bool wrongTry: false
    property var callbackObject
    anchors.fill: parent

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    function closePopup() {
        enterPasswordComponent.destroy()
    }

    function testPassword() {
        var mp = masterPassword.text
        if (secretsManager.testMasterPassword(mp)) {
            secretsManager.setMasterPassword(mp)
            callbackObject.onSuccess()
            closePopup()
        } else {
            wrongTry = true
        }
    }

    PropertyAnimation { target: enterPasswordComponent; property: "opacity";
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
                var tmp = mapToItem(enterPasswordComponent, mouse.x, mouse.y);
                old_x = tmp.x;
                old_y = tmp.y;
            }

            onPositionChanged: {
                var old_xy = Common.movePopupInsideComponent(enterPasswordComponent, dialogWindow, mouse, old_x, old_y);
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
            width: 260
            height: childrenRect.height + 40
            color: uiColors.popupBackgroundColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            ColumnLayout {
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: childrenRect.height
                anchors.margins: 20
                spacing: 20

                StyledText {
                    anchors.left: parent.left
                    text: i18.n + qsTr("Enter current Master Password:")
                }

                Rectangle {
                    width: 220
                    height: UIConfig.textInputHeight
                    anchors.left: parent.left
                    color: enabled ? uiColors.inputBackgroundColor : uiColors.inputInactiveBackground
                    border.width: masterPassword.activeFocus ? 1 : 0
                    border.color: wrongTry ? uiColors.destructiveColor : uiColors.artworkActiveColor
                    clip: true

                    StyledTextInput {
                        id: masterPassword
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: 5
                        anchors.rightMargin: 5
                        echoMode: showPasswordCheckBox.checked ? TextInput.Normal : TextInput.Password
                        anchors.verticalCenter: parent.verticalCenter

                        Keys.onBacktabPressed: {
                            event.accepted = true
                        }

                        Keys.onTabPressed: {
                            event.accepted = true
                        }

                        onAccepted: {
                            testPassword()
                        }
                    }
                }

                StyledCheckbox {
                    checked: false
                    anchors.left: parent.left
                    id: showPasswordCheckBox
                    text: i18.n + qsTr("Show password")
                }

                RowLayout {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: 24
                    spacing: 0

                    StyledButton {
                        text: i18.n + qsTr("Ok")
                        width: 90
                        onClicked: testPassword()
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        text: i18.n + qsTr("Cancel")
                        tooltip: i18.n + qsTr("This will leave password fields blank")
                        width: 90
                        onClicked: {
                            callbackObject.onFail()
                            closePopup()
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        focus = true
        masterPassword.forceActiveFocus()
    }
}

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
import QtGraphicalEffects 1.0
import "../Constants"
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"
import "../Constants/UIConfig.js" as UIConfig

Item {
    id: termsComponent
    property string termsText
    anchors.fill: parent
    z: 20000

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    function closePopup() {
        termsComponent.destroy()
    }

    Component.onCompleted: focus = true

    PropertyAnimation { target: termsComponent; property: "opacity";
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
            width: 700
            height: 400
            color: uiColors.popupBackgroundColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            StyledText {
                id: header
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.topMargin: 20
                anchors.leftMargin: 20
                text: i18.n + qsTr("Terms and conditions:")
            }

            Rectangle {
                anchors.top: header.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                anchors.topMargin: 10
                anchors.bottom: footer.top
                anchors.bottomMargin: 20
                color: uiColors.popupDarkInputBackground

                StyledScrollView {
                    id: scrollView
                    anchors.fill: parent
                    anchors.margins: 10

                    StyledTextEdit {
                        id: textEdit
                        width: 600
                        text: termsComponent.termsText
                        wrapMode: TextEdit.Wrap
                        selectionColor: uiColors.inputBackgroundColor
                        readOnly: true
                    }
                }
            }

            RowLayout {
                id: footer
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 20
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.right: parent.right
                anchors.rightMargin: 20
                height: 24
                spacing: 20

                Item {
                    Layout.fillWidth: true
                }

                StyledButton {
                    id: agreeButton
                    text: i18.n + qsTr("Agree")
                    width: 100
                    onClicked: {
                        settingsModel.userAgreeHandler()
                        helpersWrapper.reportOpen()
                        closePopup()
                    }
                }

                StyledButton {
                    text: i18.n + qsTr("Disagree")
                    width: 100
                    tooltip: i18.n + qsTr("Close Xpiks")
                    onClicked: {
                        closePopup()
                        if (!debug) {
                            Qt.quit()
                        }
                    }
                }
            }
        }
    }
}

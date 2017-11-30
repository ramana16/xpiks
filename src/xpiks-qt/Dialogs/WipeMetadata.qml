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

Item {
    id: metadataWipingComponent
    anchors.fill: parent
    property bool isInProgress: false

    function closePopup() {
        metadataWipingComponent.isInProgress = false
        metadataWipingComponent.destroy()
    }

    Keys.onEscapePressed: {
        if (!metadataWipingComponent.isInProgress) {
            closePopup()
        }

        event.accepted = true
    }

    Component.onCompleted: focus = true

    Connections {
        target: metadataIOCoordinator
        onMetadataWritingFinished: {
            metadataWipingComponent.isInProgress = false

            if (!metadataIOCoordinator.hasErrors) {
                wipeInfoDialog.open()
            } else {
                errorsNotification.open()
            }
        }
    }

    MessageDialog {
        id: errorsNotification
        title: i18.n + qsTr("Warning")
        text: i18.n + qsTr("Action finished with errors. See logs for details.")

        onAccepted: {
            closePopup()
        }
    }

    MessageDialog {
        id: confirmWipeDialog
        title: i18.n + qsTr("Warning")
        text: i18.n + qsTr("This action cannot be undone. Proceed?")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            exportButton.text = i18.n + qsTr("Wiping...")
            metadataWipingComponent.isInProgress = true
            spinner.height = spinner.width
            dialogWindow.height += spinner.height + column.spacing
            spinner.running = true
            filteredArtItemsModel.wipeMetadataFromSelectedArtworks(useBackupsCheckbox.checked)
        }
    }

    MessageDialog {
        id: wipeInfoDialog
        title: i18.n + qsTr("Information")
        text: i18.n + qsTr("Metadata is wiped, but your edits are still available in Xpiks.")

        onAccepted: {
            closePopup()
        }
    }

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    PropertyAnimation { target: metadataWipingComponent; property: "opacity";
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
                var tmp = mapToItem(metadataWipingComponent, mouse.x, mouse.y);
                old_x = tmp.x;
                old_y = tmp.y;

                var dialogPoint = mapToItem(dialogWindow, mouse.x, mouse.y);
                if (!Common.isInComponent(dialogPoint, dialogWindow)) {
                    if (!metadataWipingComponent.isInProgress) {
                        closePopup()
                    }
                }
            }

            onPositionChanged: {
                var old_xy = Common.movePopupInsideComponent(metadataWipingComponent, dialogWindow, mouse, old_x, old_y);
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
            width: 380
            height: childrenRect.height + 40
            color: uiColors.popupBackgroundColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            Behavior on height {
                NumberAnimation {
                    duration: 200
                    easing.type: Easing.InQuad
                }
            }

            Column {
                id: column
                spacing: 20
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: childrenRect.height
                anchors.margins: 20

                add: Transition {
                    NumberAnimation { properties: "x,y"; easing.type: Easing.InQuad; duration: 200 }
                }

                move: Transition {
                    NumberAnimation { properties: "x,y"; easing.type: Easing.InQuad; duration: 200 }
                }

                Item {
                    height: childrenRect.height
                    anchors.left: parent.left
                    anchors.right: parent.right

                    StyledText {
                        anchors.left: parent.left
                        text: i18.n + qsTr("Wipe metadata", "caption")
                        color: uiColors.artworkModifiedColor
                    }

                    StyledText {
                        anchors.right: parent.right
                        text: i18.n + qsTr("%1 file(s) selected").arg(filteredArtItemsModel.selectedArtworksCount)
                        color: uiColors.inputForegroundColor
                    }
                }

                LoaderIcon {
                    id: spinner
                    width: 150
                    height: 0
                    anchors.horizontalCenter: parent.horizontalCenter
                    running: false
                }

                StyledCheckbox {
                    id: useBackupsCheckbox
                    text: i18.n + qsTr("Backup each file")
                    checked: true
                    enabled: settingsModel.useExifTool && !metadataWipingComponent.isInProgress
                    visible: settingsModel.useExifTool
                }

                StyledCheckbox {
                    id: consentCheckbox
                    text: i18.n + qsTr("I know what I'm doing")
                    checked: false
                }

                RowLayout {
                    height: 24
                    anchors.left: parent.left
                    anchors.right: parent.right

                    StyledButton {
                        id: exportButton
                        isDefault: true
                        text: i18.n + qsTr("Start")
                        width: 130
                        enabled: !metadataWipingComponent.isInProgress && consentCheckbox.checked
                        onClicked: {
                            confirmWipeDialog.open()
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        text: i18.n + qsTr("Close")
                        width: 100
                        enabled: !metadataWipingComponent.isInProgress
                        onClicked: {
                            closePopup()
                        }
                    }
                }
            }
        }
    }
}

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
    id: zipArtworksComponent
    property bool immediateProcessing: false
    property var callbackObject
    anchors.fill: parent
    property var zipArchiver: helpersWrapper.getZipArchiver();

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    function closePopup() {
        zipArtworksComponent.destroy()
    }

    Connections {
        target: zipArchiver
        onRequestCloseWindow: {
            closePopup();
        }
    }

    PropertyAnimation { target: zipArtworksComponent; property: "opacity";
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
                var tmp = mapToItem(zipArtworksComponent, mouse.x, mouse.y);
                old_x = tmp.x;
                old_y = tmp.y;

                var dialogPoint = mapToItem(dialogWindow, mouse.x, mouse.y);
                if (!Common.isInComponent(dialogPoint, dialogWindow)) {
                    if (!zipArchiver.inProgress) {
                        closePopup()
                    }
                }
            }

            onPositionChanged: {
                var old_xy = Common.movePopupInsideComponent(zipArtworksComponent, dialogWindow, mouse, old_x, old_y);
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
            width: 480
            height: childrenRect.height + 40
            color: uiColors.popupBackgroundColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            Column {
                spacing: 20
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                height: childrenRect.height
                anchors.margins: 20

                RowLayout {
                    anchors.left: parent.left
                    anchors.right: parent.right

                    StyledText {
                        text: i18.n + qsTr("Zip vectors with previews")
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledText {
                        id:textItemsAvailable
                        text: i18.n + getOriginalText()

                        function getOriginalText() {
                            return zipArchiver.itemsCount == 1 ? qsTr("1 artwork with vector") : qsTr("%1 artworks with vectors").arg(zipArchiver.itemsCount)
                        }

                        Connections {
                            target: zipArchiver
                            onItemsNumberChanged: {
                               textItemsAvailable.originalText=zipArchiver.itemsCount == 1 ? qsTr("1 artwork with vector") : qsTr("%1 artworks with vectors").arg(zipArchiver.itemsCount)
                               textItemsAvailable.text=i18.n + originalText
                            }
                       }
                    }
                }

                SimpleProgressBar {
                    id: progress
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width
                    height: 20
                    color: zipArchiver.isError ? uiColors.destructiveColor : uiColors.artworkActiveColor
                    value: zipArchiver.percent
                }

                RowLayout {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: 24
                    spacing: 20

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        id: importButton
                        isDefault: true
                        width: 130
                        text: i18.n + qsTr("Start Zipping")
                        enabled: !zipArchiver.inProgress && (zipArchiver.itemsCount > 0)
                        onClicked: {
                            text = i18.n + qsTr("Zipping...")
                            zipArchiver.resetModel()
                            zipArchiver.archiveArtworks()
                        }

                        Connections {
                            target: zipArchiver
                            onFinishedProcessing: {
                                importButton.text = i18.n + qsTr("Start Zipping")

                                if (immediateProcessing) {
                                    if (typeof callbackObject !== "undefined") {
                                        callbackObject.afterZipped()
                                    }

                                    closePopup()
                                }

                                //if (!zipArchiver.isError) {
                                //    closePopup()
                                //}
                            }
                        }
                    }

                    StyledButton {
                        text: i18.n + qsTr("Close")
                        width: 100
                        enabled: !zipArchiver.inProgress
                        onClicked: {
                            closePopup()
                        }
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        focus = true
        if (immediateProcessing) {
            zipArchiver.resetModel()
            zipArchiver.archiveArtworks()
        }
    }
}

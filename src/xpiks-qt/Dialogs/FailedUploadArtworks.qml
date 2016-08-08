/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2016 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.2
import QtQuick.Dialogs 1.1
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import "../Constants"
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"
import "../Constants/UIConfig.js" as UIConfig

Item {

    id: failedUploadArtworks
    Keys.onEscapePressed: closePopup()
    anchors.fill: parent

    function closePopup() {
        failedUploadArtworks.destroy();
    }

    Component.onCompleted: focus = true
    PropertyAnimation { target: failedUploadArtworks; property: "opacity";
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

            onPressed: {
                var tmp = mapToItem(failedUploadArtworks, mouse.x, mouse.y);
                old_x = tmp.x;
                old_y = tmp.y;

                var dialogPoint = mapToItem(dialogWindow, mouse.x, mouse.y);
                if (!Common.isInComponent(dialogPoint, dialogWindow)) {
                    closePopup()
                }
            }

            onPositionChanged: {
                var old_xy = Common.movePopupInsideComponent(failedUploadArtworks, dialogWindow, mouse, old_x, old_y);
                old_x = old_xy[0]; old_y = old_xy[1];
            }
        }

        RectangularGlow {
            anchors.fill: dialogWindow
            anchors.topMargin: glowRadius/2
            anchors.bottomMargin: -glowRadius/2
            glowRadius: 4
            spread: 0.0
            color: Colors.defaultControlColor
            cornerRadius: glowRadius
        }

        // This rectangle is the actual popup
        Rectangle {
            id: dialogWindow
            width: 680
            height: 400
            color: Colors.selectedImageBackground
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 20

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: Colors.defaultDarkerColor
                    anchors.fill: parent

                    StyledScrollView {
                        anchors.fill: parent
                        anchors.margins: 10

                        ListView {
                            id: warningsListView
                            model: artworkUploader.uploadWatcher
                            anchors.fill:parent
                            spacing : 10

                            delegate: Rectangle {
                                property int delegateIndex: index
                                color: Colors.defaultDarkColor
                                id: hostRectangleWrapper
                                anchors.margins: 10
                                anchors.left: parent.left
                                anchors.right: parent.right
                                height: hostTitle.height + flow.height + 30

                                StyledText {
                                    anchors.top: parent.top
                                    anchors.right : parent.right
                                    anchors.left: parent.left
                                    id: hostTitle
                                    text: hostname
                                    font.pixelSize: 30
                                    color: Colors.artworkActiveColor
                                }

                                Flow {
                                    id: flow
                                    spacing: 20
                                    anchors.top: hostTitle.bottom
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    anchors.margins: 20

                                    Repeater {
                                        id: photosGrid
                                        model: artworkUploader.uploadWatcher.getFailedImages(delegateIndex)
                                        delegate: Item{
                                            id: imageItem
                                            width: 120
                                            height: 80
                                            property string delegateData: modelData

                                            Image {
                                                anchors.fill: parent
                                                source: "image://cached/" + imageItem.delegateData
                                                sourceSize.width: 150
                                                sourceSize.height: 150
                                                fillMode: settingsModel.fitSmallPreview ? Image.PreserveAspectFit : Image.PreserveAspectCrop
                                                asynchronous: true
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

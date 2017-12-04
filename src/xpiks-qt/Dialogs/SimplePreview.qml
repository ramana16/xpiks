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
import QtQml 2.2
import xpiks 1.0
import "../Constants"
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"
import "../Constants/UIConfig.js" as UIConfig

Rectangle {
    id: artworkSimplePreview
    color: uiColors.defaultDarkerColor
    anchors.fill: parent
    property int index

    Component.onCompleted: {
        focus = true
    }

    function closePopup() {
        artworkSimplePreview.destroy()
    }


    Item  {
        id: boundsRect
        Layout.fillWidth: true
        anchors.fill: parent

        Rectangle {
            id: topHeader
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            height: 55
            color: uiColors.defaultDarkColor

            RowLayout {
                anchors.leftMargin: 10
                anchors.rightMargin: 20
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                height: childrenRect.height
                spacing: 0

                BackGlyphButton {
                    text: i18.n + qsTr("Back")
                    onClicked: {
                        closePopup()
                    }
                }

                Item {
                    width: 10
                }

                Item {
                    Layout.fillWidth: true
                }
            }
        }

        Item {
            id: imageWrapper
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: topHeader.bottom
            anchors.bottom: parent.bottom
            property int imageMargin: 10

            LoaderIcon {
                width: 100
                height: 100
                anchors.centerIn: parent
                running: image.status == Image.Loading
            }

            StyledScrollView {
                id: scrollview
                anchors.fill: parent
                anchors.leftMargin: imageWrapper.imageMargin
                anchors.topMargin: imageWrapper.imageMargin

                Image {
                    id: image
                    source: "image://global/" + combinedArtworks.getThumbPathAt(index)
                    property bool isFullSize: false
                    width: isFullSize ? sourceSize.width : (imageWrapper.width - 2*imageWrapper.imageMargin)
                    height: isFullSize ? sourceSize.height : (imageWrapper.height - 2*imageWrapper.imageMargin)
                    fillMode: Image.PreserveAspectFit
                    anchors.centerIn: parent
                    asynchronous: true
                }
            }

            Rectangle {
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                width: 50
                height: 50
                color: uiColors.defaultDarkColor

                ZoomAmplifier {
                    id: zoomIcon
                    anchors.fill: parent
                    anchors.margins: 10
                    scale: zoomMA.pressed ? 0.9 : 1
                }

                MouseArea {
                    id: zoomMA
                    anchors.fill: parent
                    onClicked: {
                        image.isFullSize = !image.isFullSize
                        zoomIcon.isPlus = !zoomIcon.isPlus
                    }
                }
            }
        }
    }
}

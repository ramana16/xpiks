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
import xpiks 1.0
import QtGraphicalEffects 1.0
import "../Constants"
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"
import "../Constants/UIConfig.js" as UIConfig

Rectangle {
    color: uiColors.defaultDarkColor
    property bool wasLeftSideCollapsed
    property bool isRestricted: false

    Stack.onStatusChanged: {
        if (Stack.status == Stack.Active) {
            warningsModel.processPendingUpdates()
        }
    }

    function closePopup() {
        mainStackView.pop({immediate: true})
        restoreLeftPane()
    }

    function restoreLeftPane() {
        if (mainStackView.depth === 1) {
            if (!wasLeftSideCollapsed) {
                expandLeftPane()
            }
        }
    }

    Rectangle {
        id: header
        color: uiColors.defaultDarkColor
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: 55

        RowLayout {
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.right: parent.right
            height: childrenRect.height
            spacing: 0
            anchors.leftMargin: 20
            anchors.rightMargin: 40

            BackGlyphButton {
                text: i18.n + qsTr("Back")
                onClicked: closePopup()
            }

            Item {
                Layout.fillWidth: true
            }

            StyledText {
                text: i18.n + qsTr("Warnings")
                isActive: false
            }
        }
    }

    StyledScrollView {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: header.bottom
        anchors.bottom: parent.bottom
        anchors.leftMargin: 20
        anchors.rightMargin: 10
        anchors.bottomMargin: 20

        ListView {
            id: warningsListView
            model: warningsModel
            spacing: 20

            delegate: Rectangle {
                property int delegateIndex: index
                color: uiColors.defaultControlColor
                id: imageWrapper
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.rightMargin: 10
                height: columnRectangle.height
                radius: 2

                Item {
                    id: imageItem
                    width: 180
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom

                    Item {
                        id: imageHost
                        anchors.top: parent.top
                        anchors.topMargin: 25
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: 150
                        height: 120

                        Image {
                            id: artworkImage
                            anchors.fill: parent
                            source: "image://cached/" + thumbpath
                            sourceSize.width: 150
                            sourceSize.height: 150
                            fillMode: settingsModel.fitSmallPreview ? Image.PreserveAspectFit : Image.PreserveAspectCrop
                            asynchronous: true
                            // caching is implemented on different level
                            cache: false
                        }

                        Image {
                            id: videoTypeIconSmall
                            visible: isvideo
                            enabled: isvideo
                            source: "qrc:/Graphics/video-icon-s.png"
                            fillMode: Image.PreserveAspectFit
                            //sourceSize.width: 150
                            //sourceSize.height: 150
                            anchors.fill: artworkImage
                            cache: true
                        }

                        Image {
                            id: imageTypeIcon
                            visible: hasvectorattached
                            enabled: hasvectorattached
                            source: "qrc:/Graphics/vector-icon.svg"
                            sourceSize.width: 20
                            sourceSize.height: 20
                            anchors.left: artworkImage.left
                            anchors.bottom: artworkImage.bottom
                            cache: true
                        }
                    }

                    StyledText {
                        anchors.top: imageHost.bottom
                        anchors.topMargin: 3
                        width: parent.width - 10
                        elide: Text.ElideMiddle
                        horizontalAlignment: Text.AlignHCenter
                        text: basefilename
                        isActive: false
                    }
                }

                Rectangle {
                    id: columnRectangle
                    anchors.left: imageItem.right
                    anchors.top: parent.top
                    anchors.right: parent.right
                    height: (childrenRect.height < 120) ? 180 : (childrenRect.height + 60)
                    color: uiColors.defaultControlColor

                    Column {
                        id: warningsTextList
                        spacing: 15

                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.leftMargin: 10
                        anchors.rightMargin: 10
                        anchors.topMargin: 30

                        Repeater {
                            id: warningsDescriptions
                            model: warningsList

                            delegate: RowLayout {
                                width: warningsTextList.width
                                height: 10
                                spacing: 5

                                Rectangle {
                                    height: 6
                                    width: height
                                    radius: height/2
                                    anchors.verticalCenter: parent.verticalCenter
                                    color: uiColors.inactiveControlColor
                                }

                                StyledText {
                                    Layout.fillWidth: true
                                    text: i18.n + modelData
                                    color: uiColors.artworkModifiedColor
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                            }
                        }
                    }
                }

                ToolButton {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: 30
                    enabled: !isRestricted && warningsListView.count > 0
                    visible: enabled
                    normalIcon: uiColors.t + helpersWrapper.getAssetForTheme("Edit_icon_normal.svg", settingsModel.selectedThemeIndex)
                    disabledIcon: uiColors.t + helpersWrapper.getAssetForTheme("Edit_icon_disabled.svg", settingsModel.selectedThemeIndex)
                    hoveredIcon: uiColors.t + helpersWrapper.getAssetForTheme("Edit_icon_hovered.svg", settingsModel.selectedThemeIndex)
                    clickedIcon: uiColors.t + helpersWrapper.getAssetForTheme("Edit_icon_clicked.svg", settingsModel.selectedThemeIndex)
                    tooltip: i18.n + qsTr("Edit")
                    iconWidth: 33
                    iconHeight: 33
                    onClicked: {
                        var index = imageWrapper.delegateIndex
                        var originalIndex = warningsModel.getOriginalIndex(index);
                        var derivedIndex = filteredArtItemsModel.getDerivedIndex(originalIndex)
                        var metadata = filteredArtItemsModel.getArtworkMetadata(derivedIndex)
                        startOneItemEditing(metadata, derivedIndex, originalIndex)
                    }
                }
            }
        }
    }

    Item {
        anchors.fill: parent
        visible: warningsListView.count == 0

        StyledText {
            text: i18.n + qsTr("There are no warnings")
            anchors.centerIn: parent
            isActive: false
        }
    }
}

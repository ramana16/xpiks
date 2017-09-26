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
            duplicatesModel.processPendingUpdates()
        }
    }

    function closePopup() {
        duplicatesModel.clearModel()
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
        height: 45

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
                text: i18.n + qsTr("Semantic duplicates")
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
            id: duplicatesListView
            model: duplicatesModel
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
                        anchors.topMargin: 30
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: 150
                        height: 150
                        visible: hasthumbnail
                        enabled: hasthumbnail

                        Image {
                            id: artworkImage
                            anchors.fill: parent
                            source: hasthumbnail ? "image://cached/" + thumbpath : ""
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
                    anchors.left: hasthumbnail ? imageItem.right : parent.left
                    anchors.top: parent.top
                    anchors.right: hasthumbnail ? editIcon.left : parent.right
                    height: (childrenRect.height < 120) ? 160 : (childrenRect.height + 60)
                    color: uiColors.defaultControlColor

                    Item {
                        clip: true
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.right: parent.right
                        anchors.leftMargin: hasthumbnail ? 5 : 20
                        anchors.rightMargin: hasthumbnail ? 10 : 20
                        anchors.topMargin: 10
                        height: childrenRect.height

                        StyledText {
                            id: titleHit
                            text: i18.n + qsTr("Title:")
                            isActive: false
                            anchors.left: parent.left
                            anchors.top: parent.top
                        }

                        Rectangle {
                            id: titleRectangle
                            height: childrenRect.height + 10
                            color: uiColors.inputInactiveBackground
                            anchors.right: parent.right
                            anchors.left: parent.left
                            anchors.top: titleHit.bottom
                            anchors.topMargin: 3

                            StyledTextEdit {
                                id: titleText
                                anchors.top: parent.top
                                anchors.topMargin: 5
                                anchors.right: parent.right
                                anchors.left: parent.left
                                anchors.leftMargin: 5
                                anchors.rightMargin: 5
                                wrapMode: TextEdit.Wrap
                                text: dtrigger + duplicatesModel.getTitleDuplicates(imageWrapper.delegateIndex)
                                readOnly: true
                                selectByKeyboard: false
                                selectByMouse: false
                                isActive: false
                                Component.onCompleted: {
                                    duplicatesModel.initTitleHighlighting(imageWrapper.delegateIndex, titleText.textDocument)
                                }
                            }
                        }

                        StyledText {
                            id: descriptionHit
                            text: i18.n + qsTr("Description:")
                            isActive: false
                            anchors.left: parent.left
                            anchors.top: titleRectangle.bottom
                            anchors.topMargin: 10
                        }

                        Rectangle {
                            id: descriptionRectangle
                            color: uiColors.inputInactiveBackground
                            height: childrenRect.height + 10
                            anchors.top: descriptionHit.bottom
                            anchors.topMargin: 3
                            anchors.right: parent.right
                            anchors.left: parent.left

                            StyledTextEdit {
                                id: descriptionText
                                wrapMode: TextEdit.Wrap
                                anchors.top: parent.top
                                anchors.topMargin: 5
                                anchors.right: parent.right
                                anchors.left: parent.left
                                anchors.leftMargin: 5
                                anchors.rightMargin: 5
                                text: dtrigger + duplicatesModel.getDescriptionDuplicates(imageWrapper.delegateIndex).trim()
                                readOnly: true
                                selectByKeyboard: false
                                selectByMouse: false
                                isActive: false
                                Component.onCompleted: {
                                    duplicatesModel.initDescriptionHighlighting(imageWrapper.delegateIndex, descriptionText.textDocument)
                                }
                            }
                        }

                        StyledText {
                            id: keywordsHit
                            text: i18.n + qsTr("Keywords:")
                            isActive: false
                            anchors.left: parent.left
                            anchors.top: descriptionRectangle.bottom
                            anchors.topMargin: 10
                        }

                        Rectangle {
                            id: keywordsRectangle
                            color: uiColors.inputInactiveBackground
                            height: childrenRect.height + 10
                            anchors.right: parent.right
                            anchors.left: parent.left
                            anchors.top: keywordsHit.bottom
                            anchors.topMargin: 3

                            StyledTextEdit {
                                id: keywordsText
                                anchors.top: parent.top
                                anchors.topMargin: 5
                                anchors.right: parent.right
                                anchors.left: parent.left
                                anchors.leftMargin: 5
                                anchors.rightMargin: 5
                                wrapMode: TextEdit.Wrap
                                text: dtrigger + duplicatesModel.getKeywordsDuplicates(imageWrapper.delegateIndex)
                                readOnly: true
                                selectByKeyboard: false
                                selectByMouse: false
                                isActive: false

                                Component.onCompleted: {
                                    duplicatesModel.initKeywordsHighlighting(imageWrapper.delegateIndex, keywordsText.textDocument)
                                }
                            }
                        }
                    }
                }

                Item {
                    id: editIcon
                    visible: hasthumbnail
                    width: 100
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.right: parent.right

                    EditIcon {
                        backgroundColor: columnRectangle.color
                        anchors.verticalCenterOffset: -5
                        anchors.centerIn: parent
                        enabled: !isRestricted && duplicatesListView.count > 0

                        onActionInvoked: {
                            var index = imageWrapper.delegateIndex
                            var derivedIndex = filteredArtItemsModel.getDerivedIndex(originalIndex)
                            var metadata = filteredArtItemsModel.getArtworkMetadata(derivedIndex)
                            startOneItemEditing(metadata, derivedIndex, originalIndex)
                        }
                    }
                }
            }
        }
    }

    Item {
        anchors.fill: parent
        visible: duplicatesListView.count == 0

        StyledText {
            text: i18.n + qsTr("There are no duplicates")
            anchors.centerIn: parent
            isActive: false
        }
    }
}

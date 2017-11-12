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
import QtQuick.Layouts 1.1
import "../Constants" 1.0
import "../Components"
import "../StyledControls"
import "../Dialogs"
import "../Common.js" as Common
import "../Constants/UIConfig.js" as UIConfig

ColumnLayout {
    anchors.fill: parent
    anchors.leftMargin: 10
    anchors.rightMargin: 10
    anchors.topMargin: 15
    anchors.bottomMargin: 10
    spacing: 0
    enabled: mainStackView.areActionsAllowed

    StyledBlackButton {
        implicitHeight: 30
        height: 30
        anchors.left: parent.left
        anchors.right: parent.right
        text: i18.n + qsTr("Add directory")
        onClicked: chooseDirectoryDialog.open()
        enabled: (applicationWindow.openedDialogsCount == 0)
    }

    Item {
        height: 10
    }

    StyledBlackButton {
        implicitHeight: 30
        height: 30
        anchors.left: parent.left
        anchors.right: parent.right
        text: i18.n + qsTr("Add files", "button")
        action: addFilesAction
    }

    Item {
        height: 20
    }

    Item {
        Layout.fillHeight: true
        anchors.left: parent.left
        anchors.right: parent.right

        StyledScrollView {
            anchors.fill: parent
            anchors.topMargin: 5

            ListView {
                id: sourcesListView
                model: artworkRepository
                boundsBehavior: Flickable.StopAtBounds
                anchors.fill: parent

                spacing: 10

                displaced: Transition {
                    NumberAnimation { properties: "x,y"; duration: 230 }
                }

                addDisplaced: Transition {
                    NumberAnimation { properties: "x,y"; duration: 230 }
                }

                removeDisplaced: Transition {
                    NumberAnimation { properties: "x,y"; duration: 230 }
                }

                delegate: Rectangle {
                    id: sourceWrapper
                    enabled: !isremoved
                    property int delegateIndex: index

                    function getDirectoryIndex() {
                        return artworkRepository.getOriginalIndex(index)
                    }

                    color: isselected ? uiColors.itemsSourceSelected : uiColors.itemsSourceBackground
                    width: parent.width
                    height: 31
                    Layout.minimumWidth: 237

                    /*MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            filteredArtItemsModel.selectDirectory(sourceWrapper.delegateIndex)
                        }
                    }*/

                    RowLayout {
                        spacing: 10
                        anchors.fill: parent

                        Item {
                            id: placeholder1
                            width: 1
                        }

                        StyledText {
                            id: directoryPath
                            Layout.fillWidth: true
                            anchors.verticalCenter: parent.verticalCenter
                            height: 31
                            color: uiColors.itemsSourceForeground
                            text: path + " (" + usedimagescount + ")"
                            elide: Text.ElideMiddle
                            font.bold: isselected
                            font.strikeout: isremoved

                            MouseArea {
                                id: selectionMA
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                hoverEnabled: true
                                onClicked: {
                                    artworkRepository.selectDirectory(sourceWrapper.delegateIndex)
                                }
                            }
                        }

                        StyledText {
                            enabled: debug
                            visible: debug
                            text: isfull ? "(full)" : ""
                            isActive: false
                        }

                        CloseIcon {
                            width: 14
                            height: 14
                            anchors.verticalCenter: parent.verticalCenter
                            isActive: false
                            crossOpacity: 1

                            onItemClicked: {
                                if (mustUseConfirmation()) {
                                    confirmRemoveDirectoryDialog.directoryIndex = sourceWrapper.getDirectoryIndex()
                                    confirmRemoveDirectoryDialog.open()
                                } else {
                                    filteredArtItemsModel.removeArtworksDirectory(sourceWrapper.getDirectoryIndex())
                                }
                            }
                        }

                        Item {
                            id: placeholder2
                            width: 1
                        }
                    }
                }
            }
        }
    }

    StyledBlackButton {
        implicitHeight: 30
        height: 30
        anchors.left: parent.left
        anchors.right: parent.right
        text: i18.n + qsTr("Support Xpiks")
        defaultForeground: uiColors.goldColor
        hoverForeground: uiColors.buttonDefaultForeground
        visible: {
            if (switcher.isDonationCampaign1Active) {
                if ((artworkRepository.artworksSourcesCount > 0) ||
                        (switcher.isDonateCampaign1LinkClicked)) {
                    return true
                }
            }

            return false
        }
        onClicked: {
            Qt.openUrlExternally(switcher.donateCampaign1Link)
            switcher.setDonateCampaign1LinkClicked()
        }
    }
}

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
    anchors.topMargin: 15
    anchors.bottomMargin: 10
    spacing: 0
    enabled: mainStackView.areActionsAllowed

    StyledBlackButton {
        implicitHeight: 30
        height: 30
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 10
        anchors.rightMargin: 10
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
        anchors.leftMargin: 10
        anchors.rightMargin: 10
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

        ListView {
            id: sourcesListView
            model: artworkRepository
            boundsBehavior: Flickable.StopAtBounds
            anchors.fill: parent

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

                color: isselected ? uiColors.inactiveControlColor : "transparent"
                anchors.left: parent.left
                anchors.right: parent.right
                height: 50

                MouseArea {
                    id: itemSelectionMA
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        artworkRepository.selectDirectory(sourceWrapper.delegateIndex)
                    }
                }

                Rectangle {
                    anchors.fill: parent
                    color: uiColors.panelColor
                    visible: itemSelectionMA.containsMouse
                    opacity: isselected ? 0.3 : 1
                }

                RowLayout {
                    spacing: 10
                    anchors.fill: parent

                    Item {
                        id: placeholder1
                        width: 10
                    }

                    Rectangle {
                        width: 4
                        height: width
                        radius: width / 2
                        color: uiColors.artworkActiveColor
                        enabled: isselected
                        visible: isselected
                    }

                    StyledText {
                        id: directoryPath
                        Layout.fillWidth: true
                        anchors.verticalCenter: parent.verticalCenter
                        height: parent.height
                        color: uiColors.itemsSourceForeground
                        text: path + " (" + usedimagescount + ")"
                        elide: Text.ElideMiddle
                        font.bold: isselected
                        font.strikeout: isremoved
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
                        thickness: 2

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
                        width: 10
                    }
                }
            }

            CustomScrollbar {
                id: filesFoldersScrollbar
                anchors.topMargin: 0
                anchors.bottomMargin: 0
                anchors.rightMargin: 0
                flickable: sourcesListView
                //canShow: leftPanelMA.containsMouse
            }
        }
    }
}

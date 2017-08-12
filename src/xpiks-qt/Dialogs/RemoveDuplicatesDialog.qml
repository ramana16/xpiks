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
    id: removeDuplicatesDialog
    anchors.fill: parent
    property var callbackObject
    property bool initialized: false

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    Component.onCompleted: focus = true

    Keys.onEscapePressed: closePopup()

    function closePopup() {
        duplicatesModel.clearModel()
        removeDuplicatesDialog.destroy();
    }

    PropertyAnimation { target: removeDuplicatesDialog; property: "opacity";
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
                var tmp = mapToItem(removeDuplicatesDialog, mouse.x, mouse.y);
                old_x = tmp.x;
                old_y = tmp.y;
            }

            onPositionChanged: {
                var old_xy = Common.movePopupInsideComponent(removeDuplicatesDialog, dialogWindow, mouse, old_x, old_y);
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
            width: 730
            height: 610
            color: uiColors.popupBackgroundColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            ColumnLayout {
                spacing: 10
                anchors.fill: parent
                anchors.margins: 20

                RowLayout {
                    anchors.left: parent.left
                    anchors.right: parent.right

                    StyledText {
                        text: i18.n + qsTr("Duplicates")
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledText {
                        text: i18.n + getOriginalText()

                        function getOriginalText() {
                           // return spellCheckSuggestionModel.artworksCount === 1 ? qsTr("1 artwork selected") : qsTr("%1 artworks selected").arg(spellCheckSuggestionModel.artworksCount)
                           return qsTr("1 artwork selected");
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: uiColors.defaultControlColor

                    Item {
                        visible: duplicatesListView.count == 0
                        anchors.fill: parent

                        StyledText {
                            text: i18.n + qsTr("No suggestions available")
                            isActive: false
                            anchors.centerIn: parent
                        }
                    }

                    StyledScrollView {
                        anchors.fill: parent
                        anchors.margins: 10
                        focus: true

                        ListView {
                            id: duplicatesListView
                            model: duplicatesModel
                            focus: true
                            spacing: 5

                            delegate: Rectangle {
                                id: duplicatesWrapper
                                property int delegateIndex: index
                                color: uiColors.panelColor
                                anchors.left: parent.left
                                anchors.right: parent.right
                                height: duplicatesListRect.height

                                Item {
                                    id: duplicatesListRect
                                    anchors.left: parent.left
                                    anchors.leftMargin: 5
                                    anchors.top: parent.top
                                    anchors.right: parent.right
                                    height: childrenRect.height + 20

                                    Flow {
                                        anchors.left: parent.left
                                        anchors.leftMargin: 10
                                        anchors.right: parent.right
                                        anchors.rightMargin: 10
                                        anchors.top: parent.top
                                        anchors.topMargin: 10
                                        spacing: 10
                                        focus: true

                                        Repeater {
                                            model: duplicatesModel.getSelectableKeywordsModel(delegateIndex)
                                            visible: model.count == 0

                                            delegate: SuggestionWrapper {
                                                id: wrapper
                                                property int suggestionIndex: index
                                                itemHeight: 20 * settingsModel.keywordSizeScale + (settingsModel.keywordSizeScale - 1)*10
                                                suggestionText: keyword
                                                isSelected: isselected
                                                onActionClicked: {
                                                    isselected = !isselected
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                Item {
                    height: 1
                }

                RowLayout {
                    spacing: 20

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        text: i18.n + qsTr("Remove selected")
                        width: 100
                        onClicked: {
                            duplicatesModel.saveChanges()
                            closePopup()
                        }
                    }

                    StyledButton {
                        text: i18.n + qsTr("Cancel")
                        width: 80
                        onClicked: closePopup()
                    }
                }
            }
        }
    }
}

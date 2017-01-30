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
        id: userDictComponent

        anchors.fill: parent
        property variant componentParent

        signal dialogDestruction();
        Component.onDestruction: dialogDestruction();

        function closePopup() {
            userDictComponent.destroy()
        }

        Component.onCompleted: focus = true
        Keys.onEscapePressed: closePopup()


        MessageDialog {
            id: clearKeywordsDialog

            title: i18.n + qsTr("Confirmation")
            text: i18.n + qsTr("Are you sure you want to clear user dictionary? \nThis action cannot be undone.")
            standardButtons: StandardButton.Yes | StandardButton.No
            onYes: spellCheckUserDictionary.clearModel()
        }

        function scrollToBottom() {
            var flickable = scrollView.flickableItem
            if (flickable.contentHeight > flickable.height) {
                flickable.contentY = flickable.contentHeight - flickable.height
            } else {
                flickable.contentY = 0
            }
        }

        PropertyAnimation { target: userDictComponent; property: "opacity";
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
                    var tmp = mapToItem(userDictComponent, mouse.x, mouse.y);
                    old_x = tmp.x;
                    old_y = tmp.y;

                    var dialogPoint = mapToItem(dialogWindow, mouse.x, mouse.y);
                    if (!Common.isInComponent(dialogPoint, dialogWindow)) {
                        closePopup()
                    }
                }

                onPositionChanged: {
                    var old_xy = Common.movePopupInsideComponent(userDictComponent, dialogWindow, mouse, old_x, old_y);
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
                width: 700
                height: 350
                color: Colors.popupBackgroundColor
                anchors.centerIn: parent
                Component.onCompleted: anchors.centerIn = undefined
                ColumnLayout{
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 4


                    Item {
                        Layout.fillWidth: true
                    }


                    Rectangle {
                        id: keywordsWrapper
                        border.color: Colors.artworkActiveColor
                        border.width: flv.isFocused ? 1 : 0
                        anchors.left: parent.left
                        anchors.right: parent.right
                        Layout.fillHeight: true
                        color: enabled ? Colors.inputBackgroundColor : Colors.inputInactiveBackground
                        property var keywordsModel: spellCheckUserDictionary

                        function removeKeyword(index) {
                            spellCheckUserDictionary.removeKeywordAt(index)
                        }

                        function removeLastKeyword() {
                            spellCheckUserDictionary.removeLastKeyword()
                        }

                        function appendKeyword(keyword) {
                            spellCheckUserDictionary.appendKeyword(keyword)
                        }

                        EditableTags {
                            id: flv
                            objectName: "keywordsInput"
                            anchors.fill: parent
                            model: keywordsWrapper.keywordsModel
                            property int keywordHeight: 20 * settingsModel.keywordSizeScale + (settingsModel.keywordSizeScale - 1)*10
                            populateAnimationEnabled: false
                            scrollStep: keywordHeight

                            delegate: KeywordWrapper {
                                id: kw
                                keywordText: keyword
                                delegateIndex: index
                                itemHeight: flv.keywordHeight
                                closeIconDisabledColor: Colors.closeIconInactiveColor
                                onRemoveClicked: keywordsWrapper.removeKeyword(delegateIndex)
                            }

                            onTagAdded: {
                                keywordsWrapper.appendKeyword(text)
                            }

                            onRemoveLast: {
                                keywordsWrapper.removeLastKeyword()
                            }

                            onBackTabPressed: {
                                if (descriptionCheckBox.checked) {
                                    descriptionTextInput.forceActiveFocus()
                                    descriptionTextInput.cursorPosition = descriptionTextInput.text.length
                                } else if (titleCheckBox.checked) {
                                    titleTextInput.forceActiveFocus()
                                    titleTextInput.cursorPosition = titleTextInput.text.length
                                }
                            }

                            onRightClickedInside: {
                                presetsMenu.popup()
                            }
                        }

                        CustomScrollbar {
                            anchors.topMargin: -5
                            anchors.bottomMargin: -5
                            anchors.rightMargin: -15
                            flickable: flv
                        }
                    }

                    RowLayout {
                        id: footer
                        anchors.left: parent.left
                        anchors.leftMargin: 20
                        anchors.right: parent.right
                        anchors.rightMargin: 20
                        height: 24
                        spacing: 20

                        Item {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            height: childrenRect.height

                            RowLayout {
                                anchors.top: parent.top
                                anchors.left: parent.left
                                anchors.leftMargin: 3
                                anchors.right: parent.right
                                anchors.rightMargin: 3
                                spacing: 5

                                Item {
                                    Layout.fillWidth: true
                                }

                                StyledText {
                                    text: i18.n + qsTr("Save")
                                    color: enabled ? (copyKeywordsMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor) : Colors.labelActiveForeground

                                    MouseArea {
                                        id: copyKeywordsMA
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: spellCheckUserDictionary.saveToExternalModel()
                                    }
                                }

                                StyledText {
                                    text: "|"
                                    verticalAlignment: Text.AlignVCenter
                                }

                                StyledText {
                                    text: i18.n + qsTr("Cancel")
                                    color: enabled ? (cancel.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor) : Colors.labelActiveForeground

                                    MouseArea {
                                        id: cancel
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: closePopup()
                                    }
                                }

                                StyledText {
                                    text: "|"
                                    verticalAlignment: Text.AlignVCenter
                                }

                                StyledText {
                                    text: i18.n + qsTr("Clear")
                                    color: enabled ? (clearKeywordsMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor) : Colors.labelActiveForeground

                                    MouseArea {
                                        id: clearKeywordsMA
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: {
                                            if (spellCheckUserDictionary.keywordsCount > 0) {
                                                clearKeywordsDialog.open()
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

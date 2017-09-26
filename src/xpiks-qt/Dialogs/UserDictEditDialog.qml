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
    id: userDictComponent
    anchors.fill: parent
    property variant componentParent

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    function closePopup() {
        userDictComponent.destroy()
    }

    Component.onCompleted: {
        flv.activateEdit()
    }

    MessageDialog {
        id: clearKeywordsDialog

        title: i18.n + qsTr("Confirmation")
        text: i18.n + qsTr("Are you sure you want to clear user dictionary? \nThis action cannot be undone.")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: userDictEditModel.clearModel()
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
            color: uiColors.defaultControlColor
            cornerRadius: glowRadius
        }

        // This rectangle is the actual popup
        Rectangle {
            id: dialogWindow
            width: 500
            height: 300
            color: uiColors.popupBackgroundColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 0

                StyledText {
                    text: i18.n + qsTr("Words added to the user dictionary:")
                }

                Item {
                    height: 5
                }

                Rectangle {
                    id: keywordsWrapper
                    border.color: uiColors.artworkActiveColor
                    border.width: flv.isFocused ? 1 : 0
                    anchors.left: parent.left
                    anchors.right: parent.right
                    Layout.fillHeight: true
                    color: enabled ? uiColors.inputBackgroundColor : uiColors.inputInactiveBackground
                    property var keywordsModel: userDictEditModel.getBasicModel()

                    function removeKeyword(index) {
                        userDictEditModel.removeKeywordAt(index)
                    }

                    function removeLastKeyword() {
                        userDictEditModel.removeLastKeyword()
                    }

                    function appendKeyword(keyword) {
                        userDictEditModel.appendKeyword(keyword)
                    }

                    EditableTags {
                        id: flv
                        objectName: "keywordsInput"
                        anchors.fill: parent
                        model: keywordsWrapper.keywordsModel
                        property int keywordHeight: uiManager.keywordHeight
                        populateAnimationEnabled: false
                        scrollStep: keywordHeight

                        delegate: KeywordWrapper {
                            id: kw
                            keywordText: keyword
                            delegateIndex: index
                            hasSpellCheckError: false
                            hasDuplicate: hasduplicate
                            isHighlighted: true
                            itemHeight: flv.keywordHeight
                            closeIconDisabledColor: uiColors.closeIconInactiveColor
                            onRemoveClicked: keywordsWrapper.removeKeyword(delegateIndex)
                        }

                        onTagAdded: {
                            keywordsWrapper.appendKeyword(text)
                        }

                        onRemoveLast: {
                            keywordsWrapper.removeLastKeyword()
                        }
                    }

                    CustomScrollbar {
                        anchors.topMargin: -5
                        anchors.bottomMargin: -5
                        anchors.rightMargin: -15
                        flickable: flv
                    }
                }

                Item {
                    height: 20
                }

                RowLayout {
                    id: footer
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: 24
                    spacing: 20

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        text: i18.n + qsTr("Save")
                        width: 100
                        onClicked: {
                            userDictEditModel.saveUserDict()
                            closePopup()
                        }
                    }

                    StyledButton {
                        text: i18.n + qsTr("Cancel")
                        width: 100
                        onClicked: {
                            closePopup()
                        }
                    }
                }
            }
        }
    }
}

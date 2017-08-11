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
import QtGraphicalEffects 1.0
import "../StyledControls"

Item {
    id: completeBox
    property color highlightedItemColor: Colors.artworkActiveColor
    property double maxCount: 5
    property bool isBelowEdit: true
    property alias model: dropDownItems.model
    property var autoCompleteSource: acSource
    property real itemHeight: 25

    signal boxDestruction();
    Component.onDestruction: boxDestruction();

    width: 200
    height: childrenRect.height

    signal itemSelected(string completion, bool expandPreset);

    function openPopup() {
        completeBox.state = "dropDown"
    }

    function closePopup() {
        console.debug("Closing completion popup")
        completeBox.state = ""
        completeBox.destroy()
    }

    Connections {
        target: autoCompleteSource
        onDismissPopupRequested: closePopup()
        onCompletionAccepted: itemSelected(completion, expandPreset)
        onSelectedIndexChanged: dropDownItems.positionViewAtIndex(autoCompleteSource.selectedIndex, ListView.Contain)
    }

    RectangularGlow {
        anchors.left: dropDown.left
        anchors.right: dropDown.right
        anchors.top: dropDown.top
        anchors.bottom: dropDown.bottom
        anchors.topMargin: isBelowEdit ? glowRadius : -glowRadius/2
        anchors.bottomMargin: isBelowEdit ? -glowRadius : glowRadius
        visible: dropDown.visible
        height: dropDown.height
        glowRadius: 4
        spread: 0.0
        color: Colors.defaultControlColor
        cornerRadius: glowRadius
    }

    Rectangle {
        id: dropDown
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        color: Colors.inputForegroundColor
        height: 0
        focus: false
        clip: true

        onActiveFocusChanged: {
            if (!activeFocus) {
                completeBox.state = ""
            }
        }

        ListView {
            id: dropDownItems
            anchors.fill: parent
            anchors.topMargin: 5
            anchors.bottomMargin: 5
            anchors.rightMargin: scrollBar.visible ? 14 : 0
            anchors.margins: 1
            boundsBehavior: Flickable.StopAtBounds

            delegate: Rectangle {
                id: currentDelegate
                color: isSelected ? highlightedItemColor : Colors.inputForegroundColor
                property var itemText: display
                property bool isLastItem: index === (dropDownItems.count - 1)
                property bool isSelected: index === autoCompleteSource.selectedIndex
                anchors.left: parent.left
                anchors.right: parent.right
                height: itemHeight + 1

                StyledText {
                    id: completionText
                    text: parent.itemText
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 16
                    color: isSelected ? Colors.whiteColor : Colors.inputBackgroundColor
                }

                StyledText {
                    text: "p"
                    visible: ispreset
                    font.pixelSize: 10
                    color: completionText.color
                    anchors.right: parent.right
                    anchors.rightMargin: 25
                    anchors.verticalCenter: parent.verticalCenter
                }

                Rectangle {
                    visible: !currentDelegate.isLastItem
                    enabled: !currentDelegate.isLastItem
                    height: 1
                    color: isSelected ? highlightedItemColor : Colors.listSeparatorColor
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: 6
                    anchors.rightMargin: 20
                    anchors.bottomMargin: -height/2
                }

                MouseArea {
                    id: itemMA
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        autoCompleteSource.acceptSelected()
                    }

                    onPositionChanged: {
                        autoCompleteSource.selectedIndex = index
                    }
                }
            }
        }

        CustomScrollbar {
            id: scrollBar
            visible: dropDownItems.count > maxCount
            anchors.rightMargin: -13
            anchors.topMargin: -2
            anchors.bottomMargin: -2
            flickable: dropDownItems
        }
    }

    states: State {
        name: "dropDown";
        PropertyChanges {
            target: dropDown;
            height: dropDownItems.count > maxCount ? (maxCount * (completeBox.itemHeight + 1) + 10) : ((completeBox.itemHeight + 1) * dropDownItems.count + 10)
        }
    }

    transitions: Transition {
        NumberAnimation {
            target: dropDown
            properties: "height";
            easing.type: Easing.OutQuad;
            duration: 100
        }
    }
}

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
    id: dropdownComponent
    anchors.fill: parent

    property color highlightedItemColor: uiColors.artworkActiveColor
    property bool hasLastItemAction: false
    property string lastActionText: ''
    property double itemHeight: 28
    property double maxCount: 5
    property bool isBelow: true
    property bool withRelativePosition: false
    property bool glowCoversHeader: false
    property double headerHeight: 0

    property alias model: dropDownItems.model
    property alias selectedIndex: dropDownItems.currentIndex
    property alias count: dropDownItems.count

    property double topPadding: 0
    property double bottomPadding: 0
    property double leftPadding: 0

    property real dropDownHeight: dropDownItems.itemsCount > maxCount ? (maxCount * (itemHeight + 1) + 10) : ((itemHeight + 1) * dropDownItems.itemsCount + 10)
    property double dropDownWidth: 200
    property double glowTopMargin: 0

    signal comboItemSelected(int index);
    signal lastItemActionInvoked();

    function closePopup() {
        dropdownComponent.destroy()
    }

    signal popupDismissed();
    Component.onDestruction: popupDismissed();

    Component.onCompleted: {
        focus = true
        if (withRelativePosition) {
            var leftCorner = dropDown.mapToItem(parent, 0, dropDown.height)
            if (leftCorner.y > parent.height) {
                isBelow = false
            }
        }
    }
    Keys.onEscapePressed: closePopup()

    MouseArea {
        anchors.fill: parent
        propagateComposedEvents: false
        preventStealing: true
        onWheel: wheel.accepted = true
        onClicked: {
            mouse.accepted = true
            closePopup()
        }
        onDoubleClicked: {
            mouse.accepted = true
            closePopup()
        }
    }

    RectangularGlow {
        anchors.left: dropDown.left
        anchors.right: dropDown.right
        anchors.top: dropDown.top
        anchors.bottom: dropDown.bottom
        property double headerShift: glowCoversHeader ? headerHeight : 0
        anchors.topMargin: isBelow ? (glowRadius/2 + glowTopMargin - headerShift) : -glowRadius
        anchors.bottomMargin: isBelow ? -glowRadius : (glowRadius/2 + glowTopMargin - headerShift)
        visible: dropDown.visible
        height: dropDown.height
        glowRadius: 4
        spread: 0.0
        color: uiColors.defaultDarkColor
        cornerRadius: glowRadius
    }

    Rectangle {
        id: dropDown
        anchors.top: isBelow ? parent.top : undefined
        anchors.bottom: isBelow ? undefined : parent.bottom
        anchors.left: parent.left
        anchors.leftMargin: dropdownComponent.leftPadding
        anchors.topMargin: isBelow ? dropdownComponent.topPadding : 0
        anchors.bottomMargin: isBelow ? 0 : dropdownComponent.bottomPadding
        color: uiColors.popupBackgroundColor
        visible: false
        width: dropDownWidth
        height: 0
        focus: true
        clip: true
        state: "dropDown"

        ListView {
            id: dropDownItems
            property int itemsCount: count + (hasLastItemAction ? 1 : 0)
            anchors.fill: parent
            anchors.topMargin: 5
            anchors.bottomMargin: 5
            anchors.rightMargin: scrollBar.visible ? 14 : 0
            anchors.margins: 1
            boundsBehavior: Flickable.StopAtBounds

            delegate: Rectangle {
                id: currentDelegate
                color: itemMA.containsMouse ? highlightedItemColor : dropDown.color
                property var itemText: modelData
                property bool isCurrentItem: index === dropdownComponent.selectedIndex
                property bool isLastItem: index === (dropDownItems.count - 1)
                anchors.left: parent.left
                anchors.right: parent.right
                height: itemHeight + 1

                StyledText {
                    text: i18.n + itemText
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: -1
                    anchors.leftMargin: 16
                    color: itemMA.containsMouse ? uiColors.whiteColor : (isCurrentItem ? highlightedItemColor : uiColors.labelActiveForeground)
                }

                Rectangle {
                    visible: !currentDelegate.isLastItem || hasLastItemAction
                    enabled: !currentDelegate.isLastItem || hasLastItemAction
                    height: 1
                    color: itemMA.containsMouse ? highlightedItemColor : uiColors.inputBackgroundColor
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: 6
                    anchors.rightMargin: 20
                }

                MouseArea {
                    id: itemMA
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        if (index !== dropdownComponent.selectedIndex) {
                            dropdownComponent.selectedIndex = index
                            comboItemSelected(index)
                        }

                        closePopup()
                    }
                }
            }

            footer: Rectangle {
                visible: hasLastItemAction
                enabled: hasLastItemAction
                color: lastItemMA.containsMouse ? highlightedItemColor : dropDown.color
                anchors.left: parent.left
                anchors.right: parent.right
                height: hasLastItemAction ? (itemHeight + 1) : 0

                StyledText {
                    text: lastActionText
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 16
                    color: lastItemMA.containsMouse ? uiColors.whiteColor : uiColors.labelActiveForeground
                }

                MouseArea {
                    id: lastItemMA
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        lastItemActionInvoked()
                        closePopup()
                    }
                }
            }
        }

        CustomScrollbar {
            id: scrollBar
            visible: dropDownItems.itemsCount > maxCount
            anchors.rightMargin: -12
            flickable: dropDownItems
        }

        states: State {
            name: "dropDown";
            PropertyChanges {
                target: dropDown;
                height: dropDownHeight
                visible: true
                //z: 100500
            }
        }

        transitions: Transition {
            NumberAnimation {
                target: dropDown;
                properties: "height";
                easing.type: Easing.OutExpo; duration: 400
            }
        }
    }
}


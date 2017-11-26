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
import QtQuick.Controls 1.2
import "../Constants"
import "../StyledControls"
import xpiks 1.0
import QtGraphicalEffects 1.0
import "../Common.js" as Common

Item {
    id: comboBox

    property var globalParent
    property var model
    property var popup

    property color highlightedItemColor: uiColors.artworkActiveColor
    property color comboboxBackgroundColor: uiColors.defaultControlColor
    property color arrowBackground: uiColors.artworkBackground
    property color arrowDisabledBackground: uiColors.panelSelectedColor
    property color arrowForeground: uiColors.labelActiveForeground
    property color arrowInactiveForeground: uiColors.labelInactiveForeground
    property color arrowDisabledForeground: uiColors.inactiveControlColor

    property bool showColorSign: true
    property bool showHeader: true
    property bool hasLastItemAction: false
    property string lastActionText: ''
    property double headerHeight: comboBox.height
    property double itemHeight: comboBox.height
    property double glowTopMargin: 0
    property double maxCount: 5

    property bool voidSelection: false
    property bool isBelow: true
    property bool isOpened: false
    property bool dockLeft: false
    property real dockLeftMargin: 0
    property real dockTopMargin: 0
    property alias hovered: headerMA.containsMouse
    property int selectedIndex
    property double dropDownWidth: 200
    property bool glowEnabled: true
    property bool withRelativePosition: false

    signal comboItemSelected(int index);
    signal lastItemActionInvoked();

    function openPopup() {
        if (voidSelection) { comboBox.selectedIndex = -1 }
        var marginPoint = comboBox.mapToItem(globalParent, dockLeft ? 0 : comboBox.width, isBelow ? comboBox.height : 0)

        var options = {
            model: comboBox.model,
            highlightedItemColor: comboBox.highlightedItemColor,
            hasLastItemAction: comboBox.hasLastItemAction,
            lastActionText: comboBox.lastActionText,
            itemHeight: comboBox.itemHeight,
            selectedIndex: comboBox.selectedIndex,
            dropDownWidth: comboBox.dropDownWidth,
            glowTopMargin: comboBox.glowTopMargin,
            maxCount: comboBox.maxCount,
            isBelow: comboBox.isBelow,
            leftPadding: dockLeft ? marginPoint.x + dockLeftMargin : marginPoint.x - dropDownWidth,
            topPadding: marginPoint.y + dockTopMargin,
            withRelativePosition: comboBox.withRelativePosition,
            bottomPadding: globalParent.height - marginPoint.y + comboBox.height
        }

        Common.launchDialog("Components/DropdownPopup.qml", globalParent, options,
                            function(dropdown) {
                                comboBox.isOpened = true
                                dropdown.comboItemSelected.connect(comboBox.onItemSelected)
                                dropdown.lastItemActionInvoked.connect(comboBox.lastItemActionInvoked)
                                dropdown.popupDismissed.connect(comboBox.onPopupDismissed)

                                comboBox.popup = dropdown
                            })
    }

    function closePopup() {
        // do nothing - for compatibility with old CustomComboBox
    }

    function onPopupDismissed() {
        comboBox.popup = undefined
        comboBox.isOpened = false
    }

    function onItemSelected(index) {
        comboBox.selectedIndex = index
        comboItemSelected(index)
    }

    RectangularGlow {
        anchors.left: header.left
        anchors.right: header.right
        anchors.top: header.top
        anchors.bottom: header.bottom
        property bool shouldBeBelow: comboBox.popup ? comboBox.popup.isBelow : false
        anchors.topMargin: shouldBeBelow ? glowRadius/2 : -glowRadius
        anchors.bottomMargin: shouldBeBelow ? -glowRadius : glowRadius/2
        visible: glowEnabled && comboBox.isOpened
        enabled: glowEnabled
        height: header.height
        glowRadius: 4
        spread: 0.0
        color: uiColors.defaultDarkColor
        cornerRadius: glowRadius
    }

    Item {
        id: header
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: headerHeight

        Rectangle {
            id: colorSign
            anchors.left: parent.left
            width: showColorSign ? 6 : 0
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            color: highlightedItemColor
        }

        Rectangle {
            id: selectedItem
            anchors.left: showHeader ? colorSign.right : undefined
            anchors.right: showHeader ? arrowRect.left : undefined
            anchors.top: showHeader ? parent.top : undefined
            anchors.bottom: showHeader ? parent.bottom : undefined
            color: comboboxBackgroundColor
            visible: showHeader
            enabled: showHeader

            StyledText {
                id: selectedText
                text: (showHeader && comboBox.model && (comboBox.selectedIndex >= 0) && (comboBox.selectedIndex < comboBox.model.length)) ? comboBox.model[comboBox.selectedIndex] : ""
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.leftMargin: 10
                width: parent.width - 10
                elide: Text.ElideRight
                verticalAlignment: TextInput.AlignVCenter
            }
        }

        Rectangle {
            id: arrowRect
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: 20
            color: enabled ? arrowBackground : arrowDisabledBackground

            TriangleElement {
                anchors.centerIn: parent
                anchors.verticalCenterOffset: isFlipped ? height*0.3 : 0
                color: enabled ? ((headerMA.containsMouse || comboBox.isOpened) ? arrowForeground : arrowInactiveForeground) : arrowDisabledForeground
                isFlipped: !comboBox.isOpened
                width: parent.width * 0.6
                height: width * 0.5
            }
        }

        MouseArea {
            id: headerMA
            hoverEnabled: true
            anchors.fill: parent;
            onClicked: { openPopup() }
        }
    }
}

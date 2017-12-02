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
import xpiks 1.0
import "../Constants"
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"

Item {
    id: presetEditComponent
    anchors.fill: parent

    property variant componentParent

    Component.onCompleted: {
    }

    Keys.onEscapePressed: closePopup()

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    function closePopup() {
        presetEditComponent.destroy();
    }

    PropertyAnimation { target: presetEditComponent; property: "opacity";
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

    MessageDialog {
        id: confirmRemoveItemDialog
        property int itemIndex
        title: i18.n + qsTr("Confirmation")
        text: i18.n + qsTr("Are you sure you want to remove this item?")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            presetsModel.removeItem(itemIndex);

            if (presetNamesListView.count == 0) {
                addPresetButton.forceActiveFocus()
            }
        }
    }

    FocusScope {
        anchors.fill: parent

        MouseArea {
            id: backgroundMA
            anchors.fill: parent
            onWheel: wheel.accepted = true
            onClicked: mouse.accepted = true
            onDoubleClicked: mouse.accepted = true

            property real old_x : 0
            property real old_y : 0

            onPressed:{
                var tmp = mapToItem(presetEditComponent, mouse.x, mouse.y);
                old_x = tmp.x;
                old_y = tmp.y;
            }

            onPositionChanged: {
                var old_xy = Common.movePopupInsideComponent(presetEditComponent, dialogWindow, mouse, old_x, old_y);
                old_x = old_xy[0]; old_y = old_xy[1];
            }
        }

        ClipboardHelper {
            id: clipboard
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
            width: 700
            height: 450
            color: uiColors.popupBackgroundColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            Rectangle {
                id: leftPanel
                color: uiColors.defaultControlColor
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: 250
                //enabled: !csvExportModel.isExporting

                ListView {
                    id: presetNamesListView
                    model: presetsModel
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.bottom: leftFooter.top
                    anchors.topMargin: 30
                    anchors.bottomMargin: 10
                    clip: true
                    boundsBehavior: Flickable.StopAtBounds

                    add: Transition {
                        NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 230 }
                    }

                    remove: Transition {
                        NumberAnimation { property: "opacity"; to: 0; duration: 230 }
                    }

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
                        property variant myData: model
                        property int delegateIndex: index
                        property bool isCurrent: ListView.isCurrentItem
                        color: ListView.isCurrentItem ? uiColors.popupBackgroundColor : (exportPlanMA.containsMouse ? uiColors.panelColor :  leftPanel.color)
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: 50

                        MouseArea {
                            id: exportPlanMA
                            hoverEnabled: true
                            anchors.fill: parent
                            //propagateComposedEvents: true
                            //preventStealing: false

                            onClicked: {
                                if (presetNamesListView.currentIndex != sourceWrapper.delegateIndex) {
                                    presetNamesListView.currentIndex = sourceWrapper.delegateIndex

                                    groupsCombobox.updateSelectedGroup()
                                    //uploadInfos.updateProperties(sourceWrapper.delegateIndex)
                                }
                            }
                        }

                        RowLayout {
                            spacing: 10
                            anchors.fill: parent

                            Item {
                                width: 10
                            }

                            StyledText {
                                id: infoTitle
                                Layout.fillWidth: true
                                anchors.verticalCenter: parent.verticalCenter
                                height: 31
                                text: name
                                elide: Text.ElideMiddle
                            }

                            CloseIcon {
                                id: closeIcon
                                width: 14
                                height: 14
                                anchors.verticalCenterOffset: 1
                                isActive: false

                                onItemClicked: {
                                    confirmRemoveItemDialog.itemIndex = sourceWrapper.delegateIndex
                                    confirmRemoveItemDialog.open()
                                }
                            }

                            Item {
                                id: placeholder2
                                width: 15
                            }
                        }
                    }

                    Component.onCompleted: {
                        if (count > 0) {
                            titleText.forceActiveFocus()
                            titleText.cursorPosition = titleText.text.length
                        }
                    }
                }

                CustomScrollbar {
                    id: exportPlansScroll
                    anchors.topMargin: 0
                    anchors.bottomMargin: 0
                    anchors.rightMargin: 0
                    flickable: presetNamesListView
                    canShow: !backgroundMA.containsMouse && !rightPanelMA.containsMouse
                }

                Item {
                    id: leftFooter
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    height: 50

                    StyledBlackButton {
                        id: addPresetButton
                        width: 210
                        height: 30
                        anchors.centerIn: parent
                        text: i18.n + qsTr("Add new", "preset")
                        onClicked: {
                            presetsModel.addItem()
                            presetNamesListView.currentIndex = presetNamesListView.count - 1

                            // fake "default" group
                            groupsCombobox.selectedIndex = 0

                            titleText.forceActiveFocus()
                            titleText.cursorPosition = titleText.text.length
                        }
                    }
                }
            }

            Item {
                id: rightPanel
                anchors.left: leftPanel.right
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: footer.top

                MouseArea {
                    id: rightPanelMA
                    anchors.fill: parent
                    hoverEnabled: true
                }

                ColumnLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 20
                    anchors.rightMargin: 20
                    anchors.topMargin: 20
                    spacing: 4

                    StyledText {
                        text: i18.n + qsTr("Title:")
                        isActive: presetNamesListView.count > 0
                    }

                    Rectangle {
                        id: titleWrapper
                        border.width: titleText.activeFocus ? 1 : 0
                        property bool isValid: presetNamesListView.currentItem ? presetNamesListView.currentItem.myData.isnamevalid : false
                        border.color: isValid ? uiColors.artworkActiveColor : uiColors.artworkModifiedColor
                        anchors.left: parent.left
                        anchors.right: parent.right
                        color: enabled ? uiColors.inputBackgroundColor : uiColors.inputInactiveBackground
                        height: 30

                        StyledTextInput {
                            id: titleText
                            height: parent.height
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.rightMargin: 5
                            text: presetNamesListView.currentItem ? presetNamesListView.currentItem.myData.name : ""
                            anchors.leftMargin: 5

                            onTextChanged: {
                                if (presetNamesListView.currentItem) {
                                    presetNamesListView.currentItem.myData.editname = text
                                }
                            }

                            Keys.onTabPressed: {
                                flv.activateEdit()
                            }

                            onEditingFinished: {
                                if (text.length == 0) {
                                    if (presetNamesListView.currentItem) {
                                        presetNamesListView.currentItem.myData.editname = qsTr("Untitled")
                                    }
                                }

                                presetsModel.makeTitleValid(presetNamesListView.currentIndex)
                            }

                            onActiveFocusChanged: {
                                if (!activeFocus) {
                                    //ftpListAC.cancelCompletion()
                                }
                            }

                            validator: RegExpValidator {
                                // copy paste in keys.onpressed Paste
                                regExp: /[a-zA-Z0-9 _-]*$/
                            }

                            Keys.onPressed: {
                                if (event.matches(StandardKey.Paste)) {
                                    var clipboardText = clipboard.getText();
                                    clipboardText = clipboardText.replace(/(\r\n|\n|\r)/gm, '');
                                    // same regexp as in validator
                                    var sanitizedText = clipboardText.replace(/[^a-zA-Z0-9 _-]/g, '');
                                    titleText.paste(sanitizedText)
                                    event.accepted = true
                                }
                            }
                        }
                    }

                    Item {
                        height: 12
                    }

                    StyledText {
                        text: i18.n + qsTr("Group:")
                        isActive: presetNamesListView.count > 0
                    }

                    ComboBoxPopup {
                        id: groupsCombobox
                        model: presetsGroups.groupNames
                        hasLastItemAction: true
                        enabled: presetNamesListView.count > 0
                        lastActionText: i18.n + qsTr("Add group...", "preset group")
                        width: 200
                        height: 24
                        itemHeight: 28
                        showColorSign: false
                        dropDownWidth: width
                        glowEnabled: true
                        glowTopMargin: 2
                        globalParent: presetEditComponent

                        onComboItemSelected: {
                            if (presetNamesListView.currentItem) {
                                var groupID = presetsGroups.findGroupIdByIndex(selectedIndex - 1)
                                presetNamesListView.currentItem.myData.editgroup = groupID
                            }
                        }

                        onLastItemActionInvoked: {
                            var callbackObject = {
                                onSuccess: function(groupName) {
                                    var groupID = presetsGroups.addGroup(groupName)
                                    if (presetNamesListView.currentItem) {
                                        presetNamesListView.currentItem.myData.editgroup = groupID
                                    }
                                    groupsCombobox.closePopup()
                                    groupsCombobox.selectedIndex = (presetsGroups.getGroupsCount() - 1) + 1
                                },
                                onClose: function() {
                                    groupsCombobox.closePopup()
                                }
                            }

                            Common.launchDialog("Dialogs/AddPresetGroupDialog.qml",
                                                componentParent,
                                                {
                                                    callbackObject: callbackObject
                                                })
                        }

                        function updateSelectedGroup() {
                            if (presetNamesListView.currentItem) {
                                var groupID = presetNamesListView.currentItem.myData.group
                                var groupIndex = presetsGroups.findGroupIndexById(groupID);
                                if (groupIndex !== -1) {
                                    // take into account empty group
                                    groupsCombobox.selectedIndex = (groupIndex + 1)
                                } else {
                                    groupsCombobox.selectedIndex = 0
                                }
                            } else {
                                groupsCombobox.selectedIndex = 0
                            }
                        }

                        Component.onCompleted: {
                            groupsCombobox.updateSelectedGroup()
                        }
                    }

                    Item {
                        height: 12
                    }

                    RowLayout {
                        spacing: 5

                        StyledText {
                            text: i18.n + qsTr("Keywords:")
                            isActive: presetNamesListView.count > 0
                        }

                        Item {
                            Layout.fillWidth: true
                        }

                        StyledText {
                            text: presetNamesListView.currentItem ? presetNamesListView.currentItem.myData.keywordscount : ""
                        }
                    }

                    Rectangle {
                        id: keywordsWrapper
                        anchors.left: parent.left
                        anchors.right: parent.right
                        Layout.fillHeight: true
                        border.color: uiColors.artworkActiveColor
                        border.width: flv.isFocused ? 1 : 0
                        color: uiColors.inputBackgroundColor
                        state: ""

                        function removeKeyword(index) {
                            presetsModel.removeKeywordAt(presetNamesListView.currentIndex, index)
                        }

                        function removeLastKeyword() {
                            presetsModel.removeLastKeyword(presetNamesListView.currentIndex)
                        }

                        function appendKeyword(keyword) {
                            var added = presetsModel.appendKeyword(presetNamesListView.currentIndex, keyword)
                            if (!added) {
                                keywordsWrapper.state = "blinked"
                                blinkTimer.start()
                            }
                        }

                        function pasteKeywords(keywordsList) {
                            presetsModel.pasteKeywords(presetNamesListView.currentIndex, keywordsList)
                        }

                        EditableTags {
                            id: flv
                            anchors.fill: parent
                            enabled: presetNamesListView.currentIndex >= 0
                            model: presetsModel.getKeywordsModel(presetNamesListView.currentIndex)
                            property int keywordHeight: uiManager.keywordHeight
                            scrollStep: keywordHeight

                            delegate: KeywordWrapper {
                                id: kw
                                isHighlighted: true
                                keywordText: keyword
                                hasSpellCheckError: !iscorrect
                                hasDuplicate: hasduplicate
                                delegateIndex: index
                                itemHeight: flv.keywordHeight
                                onRemoveClicked: presetsModel.removeKeywordAt(presetNamesListView.currentIndex, index)
                                onActionDoubleClicked: {
                                    var callbackObject = {
                                        onSuccess: function(replacement) {
                                            presetsModel.editKeyword(presetNamesListView.currentIndex, kw.delegateIndex, replacement)
                                        },
                                        onClose: function() {
                                            flv.activateEdit()
                                        }
                                    }

                                    var basicModel = presetsModel.getKeywordsModel(presetNamesListView.currentIndex)

                                    Common.launchDialog("Dialogs/EditKeywordDialog.qml",
                                                        componentParent,
                                                        {
                                                            callbackObject: callbackObject,
                                                            previousKeyword: keyword,
                                                            keywordIndex: kw.delegateIndex,
                                                            keywordsModel: basicModel
                                                        })
                                }
                            }

                            onTagAdded: {
                                keywordsWrapper.appendKeyword(text)
                            }

                            onRemoveLast: {
                                keywordsWrapper.removeLastKeyword()
                            }

                            onTagsPasted: {
                                keywordsWrapper.pasteKeywords(tagsList)
                            }

                            onBackTabPressed: {
                                titleText.forceActiveFocus()
                                titleText.cursorPosition = titleText.text.length
                            }

                            onCompletionRequested: {
                                // no completion in presets for now
                            }
                        }

                        CustomScrollbar {
                            anchors.topMargin: -5
                            anchors.bottomMargin: -5
                            anchors.rightMargin: -15
                            flickable: flv
                        }

                        Timer {
                            id: blinkTimer
                            repeat: false
                            interval: 400
                            triggeredOnStart: false
                            onTriggered: keywordsWrapper.state = ""
                        }

                        states: State {
                            name: "blinked";
                            PropertyChanges {
                                target: keywordsWrapper;
                                border.width: 0
                            }
                        }
                    }
                }
            }

            Item {
                id: footer
                anchors.left: leftPanel.right
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: 50
                //color: uiColors.defaultDarkColor

                RowLayout {
                    id: footerRow
                    anchors.fill: parent
                    anchors.leftMargin: 20
                    anchors.rightMargin: 20
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    spacing: 20

                    StyledLink {
                        id: plainTextText
                        text: i18.n + qsTr("<u>edit in plain text</u>")
                        normalLinkColor: uiColors.labelActiveForeground
                        enabled: presetNamesListView.currentItem ? true : false
                        visible: presetNamesListView.count > 0
                        anchors.verticalCenter: parent.verticalCenter
                        onClicked: {
                            if (!presetNamesListView.currentItem) { return; }

                            var callbackObject = {
                                onSuccess: function(text, spaceIsSeparator) {
                                    presetsModel.plainTextEdit(presetNamesListView.currentIndex, text, spaceIsSeparator)
                                },
                                onClose: function() {
                                    flv.activateEdit()
                                }
                            }

                            var basicModel = presetsModel.getKeywordsModel(presetNamesListView.currentIndex)

                            Common.launchDialog("../Dialogs/PlainTextKeywordsDialog.qml",
                                                applicationWindow,
                                                {
                                                    callbackObject: callbackObject,
                                                    keywordsText: presetNamesListView.currentItem.myData.keywordsstring,
                                                    keywordsModel: basicModel
                                                });
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        text: i18.n + qsTr("Close")
                        anchors.verticalCenter: parent.verticalCenter
                        width: 100
                        onClicked: {
                            closePopup();
                        }
                    }
                }
            }
        }
    }
}

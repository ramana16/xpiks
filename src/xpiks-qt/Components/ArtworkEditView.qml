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
import xpiks 1.0
import QtGraphicalEffects 1.0
import "../Constants"
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"
import "../Constants/UIConfig.js" as UIConfig

Item {
    id: artworkEditComponent
    anchors.fill: parent

    property variant componentParent
    property var autoCompleteBox

    property int artworkIndex: -1
    property var keywordsModel

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    function onAutoCompleteClose() {
        autoCompleteBox = undefined
    }

    function closePopup() {
        mainStackView.pop()
    }

    Component.onCompleted: {
        focus = true
        //titleTextInput.forceActiveFocus()
        //titleTextInput.cursorPosition = titleTextInput.text.length
    }

    Connections {
        target: helpersWrapper
        onGlobalBeforeDestruction: {
            console.debug("UI:EditArtworkHorizontalDialog # globalBeforeDestruction")
            //closePopup()
        }
    }

    Connections {
        target: artItemsModel
        onFileWithIndexUnavailable: {
            if (artworkIndex == index) {
                console.debug("Artwork unavailable")
                closePopup()
            }
        }
    }

    MessageDialog {
        id: clearKeywordsDialog

        title: i18.n + qsTr("Confirmation")
        text: i18.n + qsTr("Clear all keywords?")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: filteredArtItemsModel.clearKeywords(artworkEditComponent.artworkIndex)
    }

    Keys.onEscapePressed: closePopup()

    Connections {
        target: artworkProxy

        onCompletionsAvailable: {
            if (typeof artworkEditComponent.autoCompleteBox !== "undefined") {
                // update completion
                return
            }

            var directParent = artworkEditComponent;
            var currWordStartRect = flv.editControl.getCurrentWordStartRect()

            var tmp = flv.editControl.mapToItem(directParent,
                                                currWordStartRect.x - 17,
                                                flv.editControl.height + 1)

            var visibleItemsCount = Math.min(acSource.getCount(), 5);
            var popupHeight = visibleItemsCount * (25 + 1) + 10

            var isBelow = (tmp.y + popupHeight) < directParent.height;

            var options = {
                model: acSource,
                editableTags: flv,
                isBelowEdit: isBelow,
                "anchors.left": directParent.left,
                "anchors.leftMargin": Math.min(tmp.x, directParent.width - 200)
            }

            if (isBelow) {
                options["anchors.top"] = directParent.top
                options["anchors.topMargin"] = tmp.y
            } else {
                options["anchors.bottom"] = directParent.bottom
                options["anchors.bottomMargin"] = directParent.height - tmp.y + flv.editControl.height
            }

            var component = Qt.createComponent("CompletionBox.qml");
            if (component.status !== Component.Ready) {
                console.debug("Component Error: " + component.errorString());
            } else {
                var instance = component.createObject(directParent, options);

                instance.boxDestruction.connect(artworkEditComponent.onAutoCompleteClose)
                instance.itemSelected.connect(flv.editControl.acceptCompletion)
                artworkEditComponent.autoCompleteBox = instance

                instance.openPopup()
            }
        }
    }

    SplitView {
        orientation: Qt.Horizontal
        anchors.fill: parent

        handleDelegate: Rectangle {
            color: Colors.defaultDarkColor
        }

        Rectangle {
            id: boundsRect
            Layout.fillWidth: true
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            color: Colors.defaultControlColor

            Item {
                id: topHeader
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: 10
                height: 35

                RowLayout {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 0

                    StyledBlackButton {
                        width: 80
                        text: qsTr("Back")
                        onClicked: closePopup()
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledText {
                        text: artworkProxy.basename
                    }
                }
            }

            Rectangle {
                id: imageWrapper
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: topHeader.bottom
                anchors.bottom: parent.bottom
                color: Colors.artworkImageBackground

                Image {
                    id: previewImage
                    source: "image://global/" + artworkProxy.imagePath
                    cache: false
                    width: imageWrapper.width - 20
                    height: imageWrapper.height - 20
                    fillMode: Image.PreserveAspectFit
                    anchors.centerIn: parent
                    asynchronous: true
                }
            }
        }

        Item {
            id: rightPane
            Layout.maximumWidth: 450
            Layout.minimumWidth: 250
            Layout.preferredWidth: 300
            width: 300
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            RowLayout {
                id: tabsHeader
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                height: 45
                spacing: 0

                CustomTab {
                    tabIndex: 0
                    width: rightPane.width/2
                    isSelected: tabIndex == editTabView.currentIndex
                    hovered: (!isSelected) && editTabMA.containsMouse
                    color: isSelected ? Colors.selectedImageBackground : Colors.defaultControlColor

                    StyledText {
                        color: parent.isSelected ? Colors.artworkActiveColor : (parent.hovered ? Colors.inputForegroundColor : Colors.labelActiveForeground)
                        text: qsTr("Edit")
                        anchors.centerIn: parent
                    }

                    MouseArea {
                        id: editTabMA
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: editTabView.currentIndex = parent.tabIndex
                    }
                }

                CustomTab {
                    tabIndex: 1
                    width: rightPane.width/2
                    isSelected: tabIndex == editTabView.currentIndex
                    hovered: (!isSelected) && infoTabMA.containsMouse
                    color: isSelected ? Colors.selectedImageBackground : Colors.defaultControlColor

                    StyledText {
                        color: parent.isSelected ? Colors.artworkActiveColor : (parent.hovered ? Colors.inputForegroundColor : Colors.labelActiveForeground)
                        text: qsTr("Info")
                        anchors.centerIn: parent
                    }

                    MouseArea {
                        id: infoTabMA
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: editTabView.currentIndex = parent.tabIndex
                    }
                }
            }

            TabView {
                id: editTabView
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: tabsHeader.bottom
                anchors.bottom: parent.bottom

                style: TabViewStyle {
                    frame: Rectangle {
                        color: Colors.selectedImageBackground
                    }

                    tabBar: Rectangle {
                        color: Colors.selectedImageBackground
                    }

                    tab: Item {}
                }

                Tab {
                    active: true
                    anchors.fill: parent

                    ColumnLayout {
                        id: fields
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 0

                        RowLayout {
                            spacing: 5

                            StyledText {
                                text: i18.n + qsTr("Title:")
                            }

                            StyledText {
                                text: i18.n + qsTr("(same as Description if empty)")
                            }

                            Item {
                                Layout.fillWidth: true
                            }

                            StyledText {
                                text: titleTextInput.length
                            }
                        }

                        Item {
                            height: 5
                        }

                        Rectangle {
                            id: anotherRect
                            anchors.left: parent.left
                            anchors.right: parent.right
                            height: 25
                            color: Colors.inputBackgroundColor
                            border.color: Colors.artworkActiveColor
                            border.width: titleTextInput.activeFocus ? 1 : 0
                            clip: true

                            Flickable {
                                id: titleFlick
                                contentWidth: titleTextInput.paintedWidth
                                contentHeight: titleTextInput.paintedHeight
                                anchors.fill: parent
                                anchors.margins: 5
                                clip: true
                                flickableDirection: Flickable.HorizontalFlick
                                interactive: false
                                focus: false

                                function ensureVisible(r) {
                                    if (contentX >= r.x)
                                        contentX = r.x;
                                    else if (contentX+width <= r.x+r.width)
                                        contentX = r.x+r.width-width;
                                }

                                StyledTextEdit {
                                    id: titleTextInput
                                    focus: true
                                    width: titleFlick.width
                                    height: titleFlick.height
                                    text: artworkProxy.title
                                    onTextChanged: artworkProxy.title = text
                                    userDictEnabled: true

                                    onActionRightClicked: {
                                        if (artworkProxy.hasTitleWordSpellError(rightClickedWord)) {
                                            console.log("Context menu for add word " + rightClickedWord)
                                            addWordContextMenu.word = rightClickedWord
                                            addWordContextMenu.popup()
                                        }
                                    }

                                    Keys.onBacktabPressed: {
                                        event.accepted = true
                                    }

                                    Keys.onTabPressed: {
                                        descriptionTextInput.forceActiveFocus()
                                        descriptionTextInput.cursorPosition = descriptionTextInput.text.length
                                    }

                                    Component.onCompleted: {
                                        artworkProxy.initTitleHighlighting(titleTextInput.textDocument)
                                    }

                                    onCursorRectangleChanged: titleFlick.ensureVisible(cursorRectangle)

                                    onActiveFocusChanged: artworkProxy.spellCheckTitle()

                                    Keys.onPressed: {
                                        if(event.matches(StandardKey.Paste)) {
                                            var clipboardText = clipboard.getText();
                                            if (Common.safeInsert(titleTextInput, clipboardText)) {
                                                event.accepted = true
                                            }
                                        } else if ((event.key === Qt.Key_Return) || (event.key === Qt.Key_Enter)) {
                                            event.accepted = true
                                        }
                                    }
                                }
                            }
                        }

                        Item {
                            height: 20
                        }

                        RowLayout {
                            anchors.left: parent.left
                            anchors.right: parent.right

                            StyledText {
                                text: i18.n + qsTr("Description:")
                            }

                            Item {
                                Layout.fillWidth: true
                            }

                            StyledText {
                                text: descriptionTextInput.length
                            }
                        }

                        Item {
                            height: 5
                        }

                        Rectangle {
                            id: rect
                            anchors.left: parent.left
                            anchors.right: parent.right
                            height: 60
                            color: Colors.inputBackgroundColor
                            border.color: Colors.artworkActiveColor
                            border.width: descriptionTextInput.activeFocus ? 1 : 0
                            clip: true

                            Flickable {
                                id: descriptionFlick
                                contentWidth: descriptionTextInput.paintedWidth
                                contentHeight: descriptionTextInput.paintedHeight
                                anchors.fill: parent
                                anchors.margins: 5
                                interactive: false
                                flickableDirection: Flickable.HorizontalFlick
                                clip: true

                                function ensureVisible(r) {
                                    if (contentX >= r.x)
                                        contentX = r.x;
                                    else if (contentX+width <= r.x+r.width)
                                        contentX = r.x+r.width-width;
                                    if (contentY >= r.y)
                                        contentY = r.y;
                                    else if (contentY+height <= r.y+r.height)
                                        contentY = r.y+r.height-height;
                                }

                                StyledTextEdit {
                                    id: descriptionTextInput
                                    width: descriptionFlick.width
                                    height: descriptionFlick.height
                                    text: artworkProxy.description
                                    focus: true
                                    userDictEnabled: true
                                    property string previousText: text
                                    property int maximumLength: 280
                                    onTextChanged: {
                                        if (text.length > maximumLength) {
                                            var cursor = cursorPosition;
                                            text = previousText;
                                            if (cursor > text.length) {
                                                cursorPosition = text.length;
                                            } else {
                                                cursorPosition = cursor-1;
                                            }
                                        }

                                        previousText = text
                                        artworkProxy.description = text
                                    }

                                    onActionRightClicked: {
                                        if (artworkProxy.hasDescriptionWordSpellError(rightClickedWord)) {
                                            console.log("Context menu for add word " + rightClickedWord)
                                            addWordContextMenu.word = rightClickedWord
                                            addWordContextMenu.popup()
                                        }
                                    }

                                    wrapMode: TextEdit.Wrap
                                    horizontalAlignment: TextEdit.AlignLeft
                                    verticalAlignment: TextEdit.AlignTop
                                    textFormat: TextEdit.PlainText

                                    Component.onCompleted: {
                                        artworkProxy.initDescriptionHighlighting(descriptionTextInput.textDocument)
                                    }

                                    Keys.onBacktabPressed: {
                                        titleTextInput.forceActiveFocus()
                                        titleTextInput.cursorPosition = titleTextInput.text.length
                                    }

                                    Keys.onTabPressed: {
                                        flv.activateEdit()
                                    }

                                    onCursorRectangleChanged: descriptionFlick.ensureVisible(cursorRectangle)

                                    onActiveFocusChanged: artworkProxy.spellCheckDescription()

                                    Keys.onPressed: {
                                        if(event.matches(StandardKey.Paste)) {
                                            var clipboardText = clipboard.getText();
                                            if (Common.safeInsert(descriptionTextInput, clipboardText)) {
                                                event.accepted = true
                                            }
                                        } else if ((event.key === Qt.Key_Return) || (event.key === Qt.Key_Enter)) {
                                            event.accepted = true
                                        }
                                    }
                                }
                            }
                        }

                        Item {
                            height: 20
                        }

                        RowLayout {
                            spacing: 5

                            StyledText {
                                id: keywordsLabel
                                text: i18.n + qsTr("Keywords:")
                            }

                            StyledText {
                                text: i18.n + qsTr("(comma-separated)")
                            }

                            Item {
                                Layout.fillWidth: true
                            }

                            StyledText {
                                text: artworkProxy.keywordsCount
                            }
                        }

                        Item {
                            height: 4
                        }

                        Rectangle {
                            id: keywordsWrapper
                            border.color: Colors.artworkActiveColor
                            border.width: flv.isFocused ? 1 : 0
                            height: 255
                            anchors.left: parent.left
                            anchors.right: parent.right
                            color: Colors.inputBackgroundColor

                            function removeKeyword(index) {
                                artworkProxy.removeKeywordAt(index)
                            }

                            function removeLastKeyword() {
                                artworkProxy.removeLastKeyword()
                            }

                            function appendKeyword(keyword) {
                                artworkProxy.appendKeyword(keyword)
                            }

                            function pasteKeywords(keywords) {
                                artworkProxy.pasteKeywords(keywords)
                            }

                            EditableTags {
                                id: flv
                                anchors.fill: parent
                                model: artworkEditComponent.keywordsModel
                                property int keywordHeight: 20 * settingsModel.keywordSizeScale + (settingsModel.keywordSizeScale - 1)*10
                                scrollStep: keywordHeight

                                delegate: KeywordWrapper {
                                    id: kw
                                    isHighlighted: true
                                    keywordText: keyword
                                    hasSpellCheckError: !iscorrect
                                    delegateIndex: index
                                    itemHeight: flv.keywordHeight
                                    onRemoveClicked: keywordsWrapper.removeKeyword(delegateIndex)
                                    onActionDoubleClicked: {
                                        var callbackObject = {
                                            onSuccess: function(replacement) {
                                                artworkProxy.editKeyword(kw.delegateIndex, replacement)
                                            },
                                            onClose: function() {
                                                flv.activateEdit()
                                            }
                                        }

                                        Common.launchDialog("Dialogs/EditKeywordDialog.qml",
                                                            componentParent,
                                                            {
                                                                callbackObject: callbackObject,
                                                                previousKeyword: keyword,
                                                                keywordIndex: kw.delegateIndex,
                                                                keywordsModel: artworkProxy.getBasicModel()
                                                            })
                                    }

                                    onActionRightClicked: {
                                        if (!iscorrect) {
                                            console.log("Context menu for add word")
                                            addWordContextMenu.word = kw.keywordText;
                                            addWordContextMenu.popup()
                                        }
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
                                    descriptionTextInput.forceActiveFocus()
                                    descriptionTextInput.cursorPosition = descriptionTextInput.text.length
                                }

                                onCompletionRequested: {
                                    helpersWrapper.autoCompleteKeyword(prefix,
                                                                       artworkProxy.getBasicModel())
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
                            height: 4
                        }

                        Item {
                            anchors.right: parent.right
                            width: parent.width
                            height: childrenRect.height

                            RowLayout {
                                anchors.left: parent.left
                                anchors.leftMargin: 3
                                anchors.right: parent.right
                                anchors.rightMargin: 3
                                spacing: 5

                                StyledText {
                                    text: i18.n + qsTr("Fix spelling")
                                    enabled: artworkEditComponent.keywordsModel ? artworkEditComponent.keywordsModel.hasSpellErrors : false
                                    color: enabled ? (fixSpellingMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor) : Colors.labelActiveForeground

                                    MouseArea {
                                        id: fixSpellingMA
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: {
                                            artworkProxy.suggestCorrections()
                                            Common.launchDialog("Dialogs/SpellCheckSuggestionsDialog.qml",
                                                                componentParent,
                                                                {})
                                        }
                                    }
                                }

                                StyledText {
                                    text: "|"
                                    verticalAlignment: Text.AlignVCenter
                                }

                                StyledText {
                                    text: i18.n + qsTr("Suggest")
                                    color: enabled ? (suggestKeywordsMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor) : Colors.labelActiveForeground

                                    MouseArea {
                                        id: suggestKeywordsMA
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: {
                                            var callbackObject = {
                                                promoteKeywords: function(keywords) {
                                                    artworkProxy.pasteKeywords(keywords)
                                                }
                                            }

                                            Common.launchDialog("Dialogs/KeywordsSuggestion.qml",
                                                                componentParent,
                                                                {callbackObject: callbackObject});
                                        }
                                    }
                                }

                                StyledText {
                                    text: "|"
                                    verticalAlignment: Text.AlignVCenter
                                }

                                StyledText {
                                    text: i18.n + qsTr("Copy")
                                    color: copyKeywordsMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor

                                    MouseArea {
                                        id: copyKeywordsMA
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: clipboard.setText(artworkProxy.getKeywordsString())
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
                                        onClicked: clearKeywordsDialog.open()
                                    }
                                }

                                Item {
                                    Layout.fillWidth: true
                                }

                                StyledText {
                                    id: plainTextText
                                    text: i18.n + qsTr("<u>edit in plain text</u>")
                                    color: plainTextMA.containsMouse ? Colors.linkClickedColor : Colors.labelActiveForeground

                                    MouseArea {
                                        id: plainTextMA
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: {
                                            // strange bug with clicking on the keywords field
                                            if (!containsMouse) { return; }

                                            var callbackObject = {
                                                onSuccess: function(text) {
                                                    artworkProxy.plainTextEdit(text)
                                                },
                                                onClose: function() {
                                                    flv.activateEdit()
                                                }
                                            }

                                            Common.launchDialog("Dialogs/PlainTextKeywordsDialog.qml",
                                                                applicationWindow,
                                                                {
                                                                    callbackObject: callbackObject,
                                                                    keywordsText: artworkProxy.getKeywordsString(),
                                                                    keywordsModel: artworkProxy.getBasicModel()
                                                                });
                                        }
                                    }
                                }
                            }
                        }

                        Item {
                            Layout.fillHeight: true
                        }
                    }
                }

                Tab {
                    anchors.fill: parent

                    StyledText {
                        text: "Test here"
                        anchors.centerIn: parent
                    }
                }
            }
        }
    }

    ClipboardHelper {
        id: clipboard
    }
}

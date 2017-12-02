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
import QtQuick.Dialogs 1.1
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import QtQuick.Window 2.0
import QtQml 2.2
import xpiks 1.0
import "../Constants" 1.0
import "../StyledControls"
import "../Dialogs"
import "../Components"
import "../Common.js" as Common
import "../Constants/UIConfig.js" as UIConfig

Item {
    id: mainGridComponent
    property int myLeftMargin: applicationWindow.leftSideCollapsed ? 0 : 2

    Action {
        id: searchAction
        shortcut: StandardKey.Find
        onTriggered: filterText.forceActiveFocus()
        enabled: (artworkRepository.artworksSourcesCount > 0) && (applicationWindow.openedDialogsCount == 0)
    }

    Action {
        id: selectAllAction
        shortcut: StandardKey.SelectAll
        enabled: (artworkRepository.artworksSourcesCount > 0) && (applicationWindow.openedDialogsCount == 0)
        onTriggered: {
            if (selectAllCheckbox.checked) {
                filteredArtItemsModel.selectFilteredArtworks();
            } else {
                filteredArtItemsModel.unselectFilteredArtworks();
            }
        }
    }

    function fixDuplicatesAction(artworkIndex) {
        artItemsModel.setupDuplicatesModel(artworkIndex)

        var wasCollapsed = applicationWindow.leftSideCollapsed
        applicationWindow.collapseLeftPane()
        mainStackView.push({
                               item: "qrc:/StackViews/DuplicatesReView.qml",
                               properties: {
                                   componentParent: applicationWindow,
                                   wasLeftSideCollapsed: wasCollapsed
                               },
                               destroyOnPop: true
                           })
    }

    function suggestKeywords(artworkIndex) {
        var callbackObject = {
            promoteKeywords: function(keywords) {
                artItemsModel.addSuggestedKeywords(artworkIndex, keywords)
            }
        }

        artItemsModel.initSuggestion(artworkIndex)

        Common.launchDialog("../Dialogs/KeywordsSuggestion.qml",
                            applicationWindow,
                            {callbackObject: callbackObject});
    }

    function fixSpelling(artworkIndex) {
        artItemsModel.suggestCorrections(artworkIndex)
        Common.launchDialog("../Dialogs/SpellCheckSuggestionsDialog.qml",
                            applicationWindow,
                            {})
    }

    function closeAutoComplete() {
        if (typeof workflowHost.autoCompleteBox !== "undefined") {
            workflowHost.autoCompleteBox.closePopup()
        }
    }

    function clearFilter() {
        filteredArtItemsModel.searchTerm = ''
        if (filterText.length > 0) {
            filterText.text = ''
        }
        filterClearTimer.start()
    }

    Menu {
        id: wordRightClickMenu
        property string word
        property int artworkIndex
        property int keywordIndex
        property bool showAddToDict: true
        property bool showExpandPreset: false

        function popupIfNeeded() {
            if (showAddToDict || showExpandPreset) {
                popup()
            }
        }

        MenuItem {
            visible: wordRightClickMenu.showAddToDict
            text: i18.n + qsTr("Add \"%1\" to dictionary").arg(wordRightClickMenu.word)
            onTriggered: spellCheckService.addWordToUserDictionary(wordRightClickMenu.word);
        }

        Menu {
            id: presetSubMenu
            visible: wordRightClickMenu.showExpandPreset
            title: i18.n + qsTr("Expand as preset")

            Instantiator {
                id : presetsInstantiator
                model: filteredPresetsModel
                onObjectAdded: presetSubMenu.insertItem( index, object )
                onObjectRemoved: presetSubMenu.removeItem( object )
                delegate: MenuItem {
                    text: name
                    onTriggered: {
                        var presetID = filteredPresetsModel.getOriginalID(index)
                        artItemsModel.expandPreset(wordRightClickMenu.artworkIndex, wordRightClickMenu.keywordIndex, presetID);
                    }
                }
            }
        }
    }

    Menu {
        id: presetsMenu
        property int artworkIndex

        Menu {
            id: subMenu
            property var defaultGroupModel: presetsGroups.getDefaultGroupModel()
            title: i18.n + qsTr("Insert preset")

            Instantiator {
                model: presetsGroups
                onObjectAdded: subMenu.insertItem( index, object )
                onObjectRemoved: subMenu.removeItem( object )
                delegate: Menu {
                    id: groupMenu
                    property int delegateIndex: index
                    property var groupModel: presetsGroups.getGroupModel(groupMenu.delegateIndex)
                    title: gname

                    Instantiator {
                        model: groupMenu.groupModel
                        onObjectAdded: groupMenu.insertItem( index, object )
                        onObjectRemoved: groupMenu.removeItem( object )

                        delegate: MenuItem {
                            text: name
                            onTriggered: {
                                artItemsModel.addPreset(presetsMenu.artworkIndex, groupMenu.groupModel.getOriginalID(index));
                            }
                        }
                    }
                }
            }

            Instantiator {
                model: subMenu.defaultGroupModel
                onObjectAdded: subMenu.insertItem( index, object )
                onObjectRemoved: subMenu.removeItem( object )

                delegate: MenuItem {
                    text: name
                    onTriggered: {
                        artItemsModel.addPreset(presetsMenu.artworkIndex, subMenu.defaultGroupModel.getOriginalID(index));
                    }
                }
            }
        }
    }

    Menu {
        id: keywordsMoreMenu
        property int artworkIndex
        property int filteredIndex
        property int keywordsCount: 0
        property var editableTags
        property bool hasDuplicates: false
        property bool hasSpellingErrors: false

        MenuItem {
            text: i18.n + qsTr("Suggest keywords")
            onTriggered: {
               suggestKeywords(keywordsMoreMenu.artworkIndex)
            }
        }

        MenuSeparator { }

        MenuItem {
            text: i18.n + qsTr("Fix spelling")
            enabled: keywordsMoreMenu.hasSpellingErrors
            onTriggered: {
                fixSpelling(keywordsMoreMenu.artworkIndex)
            }
        }

        MenuItem {
            text: i18.n + qsTr("Show duplicates")
            enabled: keywordsMoreMenu.hasDuplicates
            onTriggered: {
                fixDuplicatesAction(keywordsMoreMenu.artworkIndex)
            }
        }

        MenuSeparator { }

        MenuItem {
            text: i18.n + qsTr("Copy")
            enabled: keywordsMoreMenu.keywordsCount > 0
            onTriggered: {
                var keywordsString = filteredArtItemsModel.getKeywordsString(keywordsMoreMenu.filteredIndex)
                clipboard.setText(keywordsString)
            }
        }

        MenuItem {
            text: i18.n + qsTr("Paste")
            onTriggered: {
                keywordsMoreMenu.editableTags.paste()
                keywordsMoreMenu.editableTags.activateEdit()
            }
        }

        MenuSeparator { }

        MenuItem {
            text: i18.n + qsTr("Edit in plain text")
            onTriggered: {
                var callbackObject = {
                    onSuccess: function(text, spaceIsSeparator) {
                        artItemsModel.plainTextEdit(keywordsMoreMenu.artworkIndex, text, spaceIsSeparator)
                    },
                    onClose: function() {
                        filteredArtItemsModel.focusCurrentItemKeywords(keywordsMoreMenu.filteredIndex)
                        //flv.activateEdit()
                    }
                }

                var basicModel = filteredArtItemsModel.getBasicModel(keywordsMoreMenu.filteredIndex)
                var keywordsString = filteredArtItemsModel.getKeywordsString(keywordsMoreMenu.filteredIndex)

                Common.launchDialog("../Dialogs/PlainTextKeywordsDialog.qml",
                                    applicationWindow,
                                    {
                                        callbackObject: callbackObject,
                                        keywordsText: keywordsString,
                                        keywordsModel: basicModel
                                    });
            }
        }

        MenuItem {
            text: i18.n + qsTr("Clear")
            enabled: keywordsMoreMenu.keywordsCount > 0
            onTriggered: {
                filteredArtItemsModel.clearKeywords(keywordsMoreMenu.filteredIndex)
            }
        }
    }

    Menu {
        id: selectedArtworksMoreMenu

        MenuItem { action: fixSpellingInSelectedAction }
        MenuItem { action: fixDuplicatesInSelectedAction }
        MenuItem { action: removeMetadataAction }
        MenuItem { action: deleteKeywordsAction }
        MenuItem { action: detachVectorsAction }
        MenuItem { action: createArchivesAction }
        MenuItem { action: exportToCsvAction }
        MenuItem { action: reimportMetadataAction }
        MenuItem { action: overwriteMetadataAction }
    }

    Rectangle {
        id: toolbar
        height: 55
        color: uiColors.defaultDarkColor
        // tooltip-related
        z: 2000
        anchors.left: parent.left
        anchors.leftMargin: mainGridComponent.myLeftMargin
        anchors.right: parent.right
        anchors.top: parent.top

        RowLayout {
            spacing: 20
            anchors.fill: parent
            anchors.leftMargin: 10

            Rectangle {
                id: checkBoxWrapper
                height: 35
                anchors.verticalCenter: parent.verticalCenter
                width: childrenRect.width
                property bool isHovered: selectAllMA.containsMouse || selectAllCheckbox.hovered || selectionCombobox.hovered
                color: (enabled && (isHovered || selectionCombobox.isOpened)) ? (selectionCombobox.isOpened ? uiColors.inactiveControlColor : uiColors.defaultControlColor) : "transparent"
                enabled: artworkRepository.artworksSourcesCount > 0

                MouseArea {
                    id: selectAllMA
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        selectionCombobox.openPopup()
                    }
                }

                StyledCheckbox {
                    id: selectAllCheckbox
                    anchors.left: parent.left
                    anchors.leftMargin: 10
                    anchors.verticalCenter: parent.verticalCenter
                    defaultBoxColor: (checkBoxWrapper.isHovered && !selectionCombobox.isOpened) ? uiColors.inactiveControlColor : uiColors.defaultControlColor
                    text: ""
                    checked: filteredArtItemsModel.selectedArtworksCount > 0

                    onClicked: {
                        if (checked) {
                            filteredArtItemsModel.selectFilteredArtworks();
                        } else {
                            filteredArtItemsModel.unselectFilteredArtworks();
                        }
                    }

                    Connections {
                        target: filteredArtItemsModel
                        onSelectedArtworksCountChanged: {
                            selectAllCheckbox.checked = filteredArtItemsModel.selectedArtworksCount > 0
                        }
                    }
                }

                ComboBoxPopup {
                    id: selectionCombobox
                    anchors.left: selectAllCheckbox.right
                    anchors.verticalCenter: parent.verticalCenter
                    width: 20
                    height: 28
                    dropDownWidth: 120
                    showColorSign: false
                    showHeader: false
                    globalParent: globalHost
                    arrowBackground: "transparent"
                    arrowDisabledBackground: "transparent"
                    dockLeft: true
                    dockLeftMargin: -37
                    dockTopMargin: 3
                    maxCount: 6
                    glowEnabled: false
                    glowTopMargin: 5
                    voidSelection: true

                    model: [i18.n + qsTr("All", "select"),
                        i18.n + qsTr("None", "select"),
                        i18.n + qsTr("Modified", "select"),
                        i18.n + qsTr("Images", "select"),
                        i18.n + qsTr("Vectors", "select"),
                        i18.n + qsTr("Videos", "select")]

                    onComboItemSelected: {
                        filteredArtItemsModel.selectArtworksEx(index)
                    }
                }
            }

            StyledText {
                text: i18.n + qsTr("%1 selected").arg(filteredArtItemsModel.selectedArtworksCount)
                color: filteredArtItemsModel.selectedArtworksCount > 0 ? uiColors.artworkActiveColor : uiColors.selectedArtworkBackground
                verticalAlignment: Text.AlignVCenter
                enabled: mainStackView.areActionsAllowed
            }

            ToolButton {
                enabled: (filteredArtItemsModel.selectedArtworksCount > 0) && editAction.enabled
                normalIcon: uiColors.t + helpersWrapper.getAssetForTheme("Edit_icon_normal.svg", settingsModel.selectedThemeIndex)
                disabledIcon: uiColors.t + helpersWrapper.getAssetForTheme("Edit_icon_disabled.svg", settingsModel.selectedThemeIndex)
                hoveredIcon: uiColors.t + helpersWrapper.getAssetForTheme("Edit_icon_hovered.svg", settingsModel.selectedThemeIndex)
                clickedIcon: uiColors.t + helpersWrapper.getAssetForTheme("Edit_icon_clicked.svg", settingsModel.selectedThemeIndex)
                tooltip: i18.n + qsTr("Edit")
                onClicked: editAction.trigger()
            }

            ToolButton {
                enabled: (filteredArtItemsModel.selectedArtworksCount > 0) && removeAction.enabled
                normalIcon: uiColors.t + helpersWrapper.getAssetForTheme("Remove_icon_normal.svg", settingsModel.selectedThemeIndex)
                disabledIcon: uiColors.t + helpersWrapper.getAssetForTheme("Remove_icon_disabled.svg", settingsModel.selectedThemeIndex)
                hoveredIcon: uiColors.t + helpersWrapper.getAssetForTheme("Remove_icon_hovered.svg", settingsModel.selectedThemeIndex)
                clickedIcon: uiColors.t + helpersWrapper.getAssetForTheme("Remove_icon_clicked.svg", settingsModel.selectedThemeIndex)
                tooltip: i18.n + qsTr("Remove")
                onClicked: removeAction.trigger()
            }

            ToolButton {
                enabled: (filteredArtItemsModel.selectedArtworksCount > 0) && saveAction.enabled
                normalIcon: uiColors.t + helpersWrapper.getAssetForTheme("Save_icon_normal.svg", settingsModel.selectedThemeIndex)
                disabledIcon: uiColors.t + helpersWrapper.getAssetForTheme("Save_icon_disabled.svg", settingsModel.selectedThemeIndex)
                hoveredIcon: uiColors.t + helpersWrapper.getAssetForTheme("Save_icon_hovered.svg", settingsModel.selectedThemeIndex)
                clickedIcon: uiColors.t + helpersWrapper.getAssetForTheme("Save_icon_clicked.svg", settingsModel.selectedThemeIndex)
                tooltip: i18.n + qsTr("Save")
                onClicked: saveAction.trigger()
            }

            ToolButton {
                enabled: (filteredArtItemsModel.selectedArtworksCount > 0) && uploadAction.enabled
                normalIcon: uiColors.t + helpersWrapper.getAssetForTheme("Upload_icon_normal.svg", settingsModel.selectedThemeIndex)
                disabledIcon: uiColors.t + helpersWrapper.getAssetForTheme("Upload_icon_disabled.svg", settingsModel.selectedThemeIndex)
                hoveredIcon: uiColors.t + helpersWrapper.getAssetForTheme("Upload_icon_hovered.svg", settingsModel.selectedThemeIndex)
                clickedIcon: uiColors.t + helpersWrapper.getAssetForTheme("Upload_icon_clicked.svg", settingsModel.selectedThemeIndex)
                tooltip: i18.n + qsTr("Upload")
                onClicked: uploadAction.trigger()
            }

            ToolButton {
                enabled: filteredArtItemsModel.selectedArtworksCount > 0
                normalIcon: uiColors.t + helpersWrapper.getAssetForTheme("More_icon_normal.svg", settingsModel.selectedThemeIndex)
                disabledIcon: uiColors.t + helpersWrapper.getAssetForTheme("More_icon_disabled.svg", settingsModel.selectedThemeIndex)
                hoveredIcon: uiColors.t + helpersWrapper.getAssetForTheme("More_icon_hovered.svg", settingsModel.selectedThemeIndex)
                clickedIcon: uiColors.t + helpersWrapper.getAssetForTheme("More_icon_clicked.svg", settingsModel.selectedThemeIndex)
                tooltip: i18.n + qsTr("More")
                onClicked: selectedArtworksMoreMenu.popup()
            }

            Item {
                Layout.fillWidth: true
            }

            ToolButton {
                normalIcon: "qrc:/Graphics/Icon_heart.svg"
                disabledIcon: "qrc:/Graphics/Icon_heart.svg"
                hoveredIcon: "qrc:/Graphics/Icon_heart.svg"
                clickedIcon: "qrc:/Graphics/Icon_heart_clicked.svg"
                tooltip: i18.n + qsTr("Support Xpiks")
                iconWidth: 25
                iconHeight: 25
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

            LayoutButton {
                enabled: artworkRepository.artworksSourcesCount > 0
                anchors.verticalCenter: parent.verticalCenter
                isListLayout: applicationWindow.listLayout
                onLayoutChanged: {
                    applicationWindow.listLayout = !applicationWindow.listLayout
                    closeAutoComplete()
                }
            }

            Rectangle {
                id: filterTab
                anchors.bottom: parent.bottom
                anchors.top: parent.top
                width: 55
                enabled: artworkRepository.artworksSourcesCount > 0
                color: filterWrapper.isOpened ? uiColors.defaultControlColor : uiColors.defaultDarkColor

                ZoomAmplifier {
                    visible: !filterWrapper.isOpened
                    enabled: !filterWrapper.isOpened
                    isPlus: false
                    showPlus: false
                    width: 20
                    height: 20
                    anchors.centerIn: parent
                    lineColor: enabled ? (searchTabMA.pressed ? uiColors.whiteColor : (searchTabMA.containsMouse ? uiColors.labelActiveForeground : uiColors.labelInactiveForeground)) : uiColors.selectedArtworkBackground
                    thickness: 2

                    CustomTooltip {
                        id: searchTooltip
                        tooltipText: i18.n + qsTr("Filter")
                        withOwnMouseArea: false
                    }
                }

                Item {
                    id: crossIcon
                    visible: filterWrapper.isOpened
                    enabled: filterWrapper.isOpened
                    property color iconColor: {
                        var resultColor = uiColors.artworkActiveColor
                        if (searchTabMA.containsMouse) {
                            if (searchTabMA.pressed) {
                                resultColor = uiColors.whiteColor
                            } else {
                                resultColor = uiColors.labelActiveForeground
                            }
                        }

                        return resultColor
                    }

                    property double thickness: 2
                    width: 20
                    height: 20
                    anchors.centerIn: parent

                    Rectangle {
                        color: crossIcon.iconColor
                        width: parent.width
                        height: crossIcon.thickness
                        transformOrigin: Item.Center
                        rotation: 45
                        transform: Translate { y: (crossIcon.width - crossIcon.thickness)/2 }
                    }

                    Rectangle {
                        color: crossIcon.iconColor
                        width: parent.width
                        height: crossIcon.thickness
                        transformOrigin: Item.Center
                        rotation: 135
                        transform: Translate { y: (crossIcon.width - crossIcon.thickness)/2 }
                    }
                }

                MouseArea {
                    id: searchTabMA
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: { searchTooltip.trigger() }
                    onExited: { searchTooltip.dismiss() }
                    onClicked: {
                        console.info("Toggle artworks filter")

                        if (filterWrapper.isOpened) {
                            clearFilter()
                        }

                        filterWrapper.userOpened = !filterWrapper.userOpened
                        if (filterWrapper.userOpened) {
                            filterText.forceActiveFocus()
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        id: filterWrapper
        property bool userOpened: false
        property bool isOpened: (filteredArtItemsModel.searchTerm !== "") || userOpened
        anchors.left: parent.left
        anchors.leftMargin: mainGridComponent.myLeftMargin
        anchors.right: parent.right
        anchors.top: toolbar.bottom
        color: uiColors.defaultControlColor
        height: 0
        opacity: 0

        states: [
            State {
                name: "opened"
                when: filterWrapper.isOpened
                PropertyChanges {
                    target: filterWrapper
                    height: 45
                    opacity: 1
                }
            }
        ]

        transitions: Transition {
            PropertyAnimation {
                properties: "height, opacity"
                easing.type: "InOutQuad"
            }
        }

        Rectangle {
            id: filterRect
            anchors.left: parent.left
            anchors.leftMargin: 20
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: 2
            color: uiColors.defaultDarkColor
            width: 400
            height: UIConfig.textInputHeight

            ZoomAmplifier {
                id: zoomIcon
                isPlus: false
                showPlus: false
                width: 14
                height: 15
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                lineColor: uiColors.labelInactiveForeground
                thickness: 1
            }

            StyledTextInput {
                id: filterText
                width: parent.width - 50
                height: UIConfig.textInputHeight
                clip: true
                anchors.left: zoomIcon.right
                anchors.leftMargin: 10
                enabled: artworkRepository.artworksSourcesCount > 0
                selectionColor: uiColors.inputInactiveForeground
                selectedTextColor: uiColors.whiteColor
                color: uiColors.inputInactiveForeground
                focus: true

                onAccepted: {
                    filteredArtItemsModel.searchTerm = text
                }

                Connections {
                    target: filteredArtItemsModel
                    onSearchTermChanged: filterText.text = filteredArtItemsModel.searchTerm
                }
            }

            CloseIcon {
                width: 14
                height: 14
                anchors.right: parent.right
                anchors.rightMargin: 5
                enabled: filterText.length > 0
                anchors.verticalCenter: parent.verticalCenter
                onItemClicked: clearFilter()
            }

            StyledText {
                text: i18.n + qsTr("Search...")
                visible: !(filterClearTimer.running || filterText.activeFocus || (filterText.length > 0))
                color: uiColors.inputHintForeground
                anchors.left: filterText.left
                anchors.verticalCenter: parent.verticalCenter
            }

            Timer {
                id: filterClearTimer
                running: false
                interval: 750
                repeat: false
            }
        }

        StyledBlackButton {
            id: searchButton
            anchors.left: filterRect.right
            anchors.leftMargin: 20
            anchors.verticalCenter: parent.verticalCenter
            anchors.verticalCenterOffset: 2
            width: 100
            text: i18.n + qsTr("Search")
            enabled: artworkRepository.artworksSourcesCount > 0
            onClicked: filteredArtItemsModel.searchTerm = filterText.text
        }
    }

    Rectangle {
        anchors.left: parent.left
        anchors.leftMargin: mainGridComponent.myLeftMargin
        anchors.right: parent.right
        anchors.top: filterWrapper.bottom
        anchors.bottom: parent.bottom
        color: uiColors.defaultControlColor

        Item {
            id: workflowHost
            anchors.fill: parent
            property var autoCompleteBox

            function onAutoCompleteClose() {
                autoCompleteBox = undefined
            }

            Rectangle {
                id: undoRedoRect
                color: uiColors.defaultDarkColor
                width: parent.width
                height: 0
                opacity: 0
                anchors.topMargin: 4
                anchors.top: parent.top

                states: [
                    State {
                        name: "canundo"
                        when: undoRedoManager.canUndo
                        PropertyChanges {
                            target: undoRedoRect
                            height: 40
                            opacity: 1
                        }
                    }
                ]

                transitions: Transition {
                    PropertyAnimation {
                        properties: "height, opacity"
                        easing.type: "InOutQuad"
                    }
                }

                Rectangle {
                    anchors.left: parent.left
                    color: uiColors.artworkSavedColor
                    width: applicationWindow.listLayout ? 6 : 3
                    height: parent.height
                }

                RowLayout {
                    anchors.centerIn: parent

                    StyledText {
                        id: undoDescription
                        text: undoRedoManager.undoDescription
                        isActive: false
                    }

                    Item {
                        width: 10
                    }

                    StyledText {
                        text: i18.n + qsTr("Undo")
                        color: undoMA.pressed ? uiColors.linkClickedColor : uiColors.artworkActiveColor

                        MouseArea {
                            id: undoMA
                            anchors.fill: parent
                            enabled: undoRedoManager.canUndo
                            cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                            onClicked: {
                                undoRedoManager.undoLastAction()
                            }
                        }
                    }

                    StyledText {
                        text: i18.n + qsTr("Dismiss (%1)").arg(settingsModel.dismissDuration - (autoDismissTimer.iterations % (settingsModel.dismissDuration + 1)))
                        color: dismissUndoMA.pressed ? uiColors.linkClickedColor : uiColors.labelInactiveForeground

                        MouseArea {
                            id: dismissUndoMA
                            anchors.fill: parent
                            enabled: undoRedoManager.canUndo
                            cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                            onClicked: {
                                undoRedoManager.discardLastAction()
                            }
                        }
                    }

                }

                Timer {
                    id: autoDismissTimer
                    property int iterations: 0
                    interval: 1000
                    repeat: true
                    running: undoRedoManager.canUndo
                    onTriggered: {
                        iterations += 1

                        if (iterations % (settingsModel.dismissDuration + 1) === settingsModel.dismissDuration) {
                            undoRedoManager.discardLastAction()
                            iterations = 0
                        }
                    }
                }

                Connections {
                    target: undoRedoManager
                    onItemRecorded: {
                        autoDismissTimer.iterations = 0
                    }
                }
            }

            Rectangle {
                id: separator
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.rightMargin: artworksHost.count > 3 ? 15 : 0
                anchors.topMargin: visible ? 2 : 4
                anchors.top: undoRedoRect.bottom
                height: visible ? 2 : 0
                color: uiColors.defaultDarkColor
                visible: !undoRedoManager.canUndo && (artworksHost.count > 0)
            }

            Item {
                id: mainScrollView
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: separator.bottom
                anchors.bottom: parent.bottom
                clip: true
                property bool areScrollbarsVisible: artworksHost.contentHeight > artworksHost.height
                //__wheelAreaScrollSpeed: 50 + 10*settingsModel.scrollSpeedScale
                // does not work for now in Qt 5.4.1 in combination with ListView
                //verticalScrollBarPolicy: Qt.ScrollBarAlwaysOn

                GridView {
                    id: artworksHost
                    property bool isEditingAllowed: filteredArtItemsModel.selectedArtworksCount == 0
                    anchors.fill: parent
                    anchors.rightMargin: mainScrollView.areScrollbarsVisible ? 10 : 0
                    model: filteredArtItemsModel
                    boundsBehavior: Flickable.StopAtBounds
                    property int cellSpacing: 4
                    property double defaultRowHeight: 205
                    cellHeight: applicationWindow.listLayout ? (defaultRowHeight + 80*(settingsModel.keywordSizeScale - 1.0) + cellSpacing) : (defaultRowHeight + cellSpacing)
                    cellWidth: applicationWindow.listLayout ? artworksHost.width : (208 + cellSpacing)
                    highlightFollowsCurrentItem: false
                    currentIndex: -1
                    focus: true
                    flickDeceleration: 1500/settingsModel.scrollSpeedScale

                    function forceUpdateArtworks(needToMoveCurrentItem) {
                        console.debug("UI::forceUpdateArtworks # updating main listview")
                        artworksHost.returnToBounds()
                        artworksHost.forceLayout()
                        artworksHost.update()

                        // this piece of code is here in order to beat caching of ListView
                        // when you remove all items but few it fails to draw them
                        if (needToMoveCurrentItem) {
                            console.debug("UI::forceUpdateArtworks # Moving into current item " + artworksHost.currentIndex)
                            artworksHost.moveCurrentIndexDown()
                            artworksHost.positionViewAtIndex(artworksHost.currentIndex, GridView.Visible)
                        }
                    }

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

                    onContentYChanged: closeAutoComplete()

                    delegate: FocusScope {
                        id: wrappersScope
                        width: applicationWindow.listLayout ? (mainScrollView.areScrollbarsVisible ? (parent.width - 5) : parent.width) : 208
                        height: applicationWindow.listLayout ? (artworksHost.defaultRowHeight + 80*(settingsModel.keywordSizeScale - 1.0)) : artworksHost.defaultRowHeight

                        function updateCurrentIndex() {
                            GridView.view.currentIndex = rowWrapper.delegateIndex
                        }

                        Rectangle {
                            id: rowWrapper
                            objectName: "artworkDelegate"
                            property bool isHighlighted: isItemSelected || wrappersScope.GridView.isCurrentItem
                            color: isHighlighted ? uiColors.selectedArtworkBackground : uiColors.artworkBackground
                            property var artworkModel: filteredArtItemsModel.getArtworkMetadata(index)
                            property var keywordsModel: filteredArtItemsModel.getBasicModel(index)
                            property int delegateIndex: index
                            property bool isItemSelected: filteredArtItemsModel.s || isselected
                            anchors.fill: parent

                            function popupArtworkContextMenu() {
                                console.log("Context menu for artwork")
                                artworkContextMenu.filename = filename;
                                artworkContextMenu.index = rowWrapper.delegateIndex
                                artworkContextMenu.popup()
                            }

                            function getIndex() {
                                return filteredArtItemsModel.getOriginalIndex(index)
                            }

                            function focusDescription() {
                                descriptionTextInput.forceActiveFocus()
                                descriptionTextInput.cursorPosition = descriptionTextInput.text.length
                            }

                            function isAnyFocused() {
                                return descriptionTextInput.activeFocus || flv.isFocused || titleTextInput.activeFocus;
                            }

                            function focusIfNeeded() {
                                if (!isAnyFocused()) {
                                    focusDescription()
                                }

                                if (!isselected) {
                                    flv.removeFocus()
                                }
                            }

                            function switchChecked() {
                                editisselected = !isselected
                                itemCheckedCheckbox.checked = isselected
                                wrappersScope.updateCurrentIndex()
                            }

                            onIsHighlightedChanged: {
                                if (isHighlighted) {
                                    filteredArtItemsModel.registerCurrentItem(rowWrapper.delegateIndex)
                                }
                            }

                            Connections {
                                target: rowWrapper.artworkModel

                                onFocusRequested: {
                                    if (directionSign === +1) {
                                        descriptionTextInput.forceActiveFocus()
                                        descriptionTextInput.cursorPosition = descriptionTextInput.text.length
                                    } else if (directionSign === -1) {
                                        flv.activateEdit()
                                    } else {
                                        console.warn("Wrong direction sign: " + directionSign)
                                    }

                                    artworksHost.positionViewAtIndex(rowWrapper.delegateIndex, GridView.Contain)
                                }
                            }

                            Connections {
                                target: rowWrapper.keywordsModel

                                // HACK:
                                onDescriptionSpellingChanged: descriptionTextInput.deselect()
                                onTitleSpellingChanged: {
                                    if (columnLayout.isWideEnough) {
                                        titleTextInput.deselect()
                                    }
                                }

                                onCompletionsAvailable: {
                                    if (!applicationWindow.actionsEnabled) { return }

                                    acSource.initializeCompletions()

                                    if (typeof workflowHost.autoCompleteBox !== "undefined") {
                                        if (workflowHost.autoCompleteBox.isBelowEdit) {
                                            // update completion
                                            return;
                                        }
                                    }

                                    var directParent = mainScrollView;
                                    var currWordStartRect = flv.editControl.getCurrentWordStartRect()

                                    var tmp = flv.editControl.mapToItem(directParent,
                                                                        currWordStartRect.x - 17,
                                                                        flv.editControl.height + 1)

                                    var visibleItemsCount = Math.min(acSource.getCount(), 5);
                                    var popupHeight = visibleItemsCount * (25 + 1) + 10

                                    if (typeof workflowHost.autoCompleteBox !== "undefined") {
                                        if (!workflowHost.autoCompleteBox.isBelowEdit) {
                                            workflowHost.autoCompleteBox.anchors.topMargin = tmp.y - popupHeight - flv.editControl.height - 2
                                        }
                                        // update completion
                                        return
                                    }

                                    var isBelow = (tmp.y + popupHeight) < directParent.height;

                                    var options = {
                                        model: acSource.getCompletionsModel(),
                                        autoCompleteSource: acSource,
                                        isBelowEdit: isBelow,
                                        "anchors.left": directParent.left,
                                        "anchors.leftMargin": Math.min(tmp.x, directParent.width - 200),
                                        "anchors.top": directParent.top
                                    }

                                    if (isBelow) {
                                        options["anchors.topMargin"] = tmp.y
                                    } else {
                                        options["anchors.topMargin"] = tmp.y - popupHeight - flv.editControl.height - 2
                                    }

                                    var component = Qt.createComponent("../Components/CompletionBox.qml");
                                    if (component.status !== Component.Ready) {
                                        console.warn("Component Error: " + component.errorString());
                                    } else {
                                        var instance = component.createObject(directParent, options);

                                        instance.boxDestruction.connect(workflowHost.onAutoCompleteClose)
                                        instance.itemSelected.connect(flv.acceptCompletion)
                                        workflowHost.autoCompleteBox = instance

                                        instance.openPopup()
                                    }
                                }
                            }

                            Item {
                                anchors.fill: parent

                                Rectangle {
                                    id: isModifiedRectangle
                                    color: ismodified ? uiColors.artworkModifiedColor : "transparent"
                                    width: 3
                                    anchors.left: parent.left
                                    anchors.top: parent.top
                                    anchors.bottom: parent.bottom
                                }

                                Item {
                                    id: checkboxSpacer
                                    width: applicationWindow.listLayout ? 15 : 8
                                    anchors.left: isModifiedRectangle.right
                                    anchors.top: parent.top
                                    anchors.bottom: parent.bottom

                                    MouseArea {
                                        anchors.fill: parent
                                        enabled: applicationWindow.listLayout
                                        onClicked: {
                                            rowWrapper.switchChecked()
                                            rowWrapper.focusIfNeeded()
                                        }
                                    }
                                }

                                Item {
                                    id: checkboxRectangle
                                    anchors.left: checkboxSpacer.right
                                    anchors.top: parent.top
                                    anchors.bottom: parent.bottom
                                    width: 20

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            rowWrapper.switchChecked()
                                            rowWrapper.focusIfNeeded()
                                        }
                                    }

                                    StyledCheckbox {
                                        id: itemCheckedCheckbox
                                        //checked: isselected
                                        focus: false
                                        anchors.left: parent.left
                                        anchors.top: parent.top
                                        anchors.topMargin: descriptionText.height + 24
                                        activeFocusOnPress: false
                                        onClicked: editisselected = checked
                                        Component.onCompleted: itemCheckedCheckbox.checked = isselected

                                        Connections {
                                            target: filteredArtItemsModel
                                            onAllItemsSelectedChanged: {
                                                itemCheckedCheckbox.checked = rowWrapper.isItemSelected
                                            }
                                        }
                                    }

                                    DotsButton {
                                        id: artworkDots
                                        z: 10
                                        defaultDotsColor: rowWrapper.isHighlighted ? uiColors.labelInactiveForeground : uiColors.inactiveControlColor
                                        anchors.top: itemCheckedCheckbox.top
                                        anchors.topMargin: imageHost.height - artworkDots.height + 2
                                        anchors.left: parent.left
                                        anchors.leftMargin: -5
                                        onDotsClicked: rowWrapper.popupArtworkContextMenu()
                                    }
                                }

                                Item {
                                    id: imageColumnWrapper
                                    width: applicationWindow.listLayout ? 180 : 160
                                    anchors.left: checkboxRectangle.right
                                    anchors.top: parent.top
                                    anchors.bottom: parent.bottom

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            rowWrapper.switchChecked()
                                            rowWrapper.focusIfNeeded()
                                        }
                                    }

                                    Item {
                                        anchors.fill: parent
                                        anchors.leftMargin: applicationWindow.listLayout ? 10 : 0
                                        anchors.rightMargin: applicationWindow.listLayout ? 15 : 0

                                        Item {
                                            id: imageHost
                                            anchors.top: parent.top
                                            anchors.topMargin: descriptionText.height + 24
                                            anchors.horizontalCenter: parent.horizontalCenter
                                            width: 150
                                            property double desiredHeight: descriptionRect.height + keywordsWrapper.height + keywordsLabel.height + 10
                                            height: desiredHeight > 150 ? 150 : desiredHeight

                                            Rectangle {
                                                color: rowWrapper.isHighlighted ? uiColors.artworkBackground : uiColors.defaultDarkerColor
                                                anchors.fill: parent
                                            }

                                            Image {
                                                id: artworkImage
                                                anchors.fill: parent
                                                source: "image://cached/" + thumbpath
                                                sourceSize.width: 150
                                                sourceSize.height: 150
                                                fillMode: settingsModel.fitSmallPreview ? Image.PreserveAspectFit : Image.PreserveAspectCrop
                                                asynchronous: true
                                                // caching is implemented on different level
                                                cache: false
                                            }

                                            Image {
                                                id: videoTypeIcon
                                                visible: isvideo
                                                enabled: isvideo
                                                source: "qrc:/Graphics/video-icon-s.png"
                                                fillMode: Image.PreserveAspectCrop
                                                //sourceSize.width: 300
                                                //sourceSize.height: 300
                                                anchors.fill: artworkImage
                                                cache: true
                                            }

                                            Image {
                                                id: imageTypeIcon
                                                visible: hasvectorattached
                                                enabled: hasvectorattached
                                                source: "qrc:/Graphics/vector-icon.svg"
                                                sourceSize.width: 20
                                                sourceSize.height: 20
                                                anchors.left: artworkImage.left
                                                anchors.bottom: artworkImage.bottom
                                                cache: true
                                            }

                                            Rectangle {
                                                anchors.fill: parent
                                                color: "transparent"
                                                border.color: uiColors.artworkActiveColor
                                                border.width: rowWrapper.isItemSelected ? 1 : 0
                                            }

                                            MouseArea {
                                                anchors.fill: parent
                                                propagateComposedEvents: true
                                                acceptedButtons: Qt.LeftButton | Qt.RightButton

                                                function dblClickHandler() {
                                                    var index = rowWrapper.delegateIndex
                                                    var originalIndex = rowWrapper.getIndex()
                                                    var metadata = filteredArtItemsModel.getArtworkMetadata(index)
                                                    startOneItemEditing(metadata, index, originalIndex)
                                                }

                                                Timer {
                                                    id: dblClickTimer
                                                    interval: 200
                                                    onTriggered: {
                                                        rowWrapper.switchChecked()
                                                        rowWrapper.focusIfNeeded()
                                                    }
                                                }

                                                onClicked: {
                                                    if (mouse.button == Qt.RightButton && openedDialogsCount == 0) {
                                                        rowWrapper.popupArtworkContextMenu()
                                                    } else {
                                                        if (dblClickTimer.running) {
                                                            dblClickTimer.stop()
                                                            dblClickHandler()
                                                        } else {
                                                            dblClickTimer.restart()
                                                        }
                                                    }
                                                }
                                            }
                                        }

                                        StyledText {
                                            anchors.top: imageHost.bottom
                                            anchors.topMargin: 3
                                            width: parent.width
                                            elide: Text.ElideMiddle
                                            horizontalAlignment: Text.AlignHCenter
                                            text: basefilename
                                            isActive: rowWrapper.isHighlighted
                                        }
                                    }
                                }

                                Item {
                                    id: columnRectangle
                                    visible: applicationWindow.listLayout
                                    anchors.top: applicationWindow.listLayout ? parent.top : undefined
                                    anchors.bottom: applicationWindow.listLayout ? parent.bottom : undefined
                                    anchors.left: applicationWindow.listLayout ? imageColumnWrapper.right : undefined
                                    anchors.leftMargin: applicationWindow.listLayout ? 5 : 0
                                    anchors.right: applicationWindow.listLayout ? parent.right : undefined

                                    Item {
                                        id: columnLayout
                                        anchors.fill: parent
                                        anchors.leftMargin: 0
                                        anchors.rightMargin: 20
                                        anchors.topMargin: 20
                                        property bool isWideEnough: width > 450

                                        StyledText {
                                            id: descriptionText
                                            anchors.left: parent.left
                                            anchors.top: parent.top
                                            text: i18.n + qsTr("Description:")
                                            isActive: rowWrapper.isHighlighted
                                        }

                                        StyledText {
                                            anchors.left: descriptionText.right
                                            anchors.leftMargin: 3
                                            text: "*"
                                            color: uiColors.destructiveColor
                                            visible: rowWrapper.keywordsModel.hasDescriptionSpellErrors
                                        }

                                        StyledText {
                                            id: titleText
                                            anchors.left: titleRect.left
                                            anchors.top: parent.top
                                            visible: columnLayout.isWideEnough
                                            text: i18.n + qsTr("Title:")
                                            isActive: rowWrapper.isHighlighted
                                        }

                                        StyledText {
                                            anchors.left: titleText.right
                                            anchors.leftMargin: 3
                                            text: "*"
                                            color: uiColors.destructiveColor
                                            visible: rowWrapper.keywordsModel.hasTitleSpellErrors
                                        }

                                        Rectangle {
                                            id: descriptionRect
                                            height: 30
                                            anchors.left: parent.left
                                            anchors.right: titleRect.left
                                            anchors.rightMargin: columnLayout.isWideEnough ? 20 : 0
                                            anchors.top: descriptionText.bottom
                                            anchors.topMargin: 3
                                            color: rowWrapper.isHighlighted ? uiColors.inputBackgroundColor : uiColors.inputInactiveBackground
                                            border.color: uiColors.artworkActiveColor
                                            border.width: descriptionTextInput.activeFocus ? 1 : 0
                                            clip: true
                                            focus: false
                                            enabled: artworksHost.isEditingAllowed

                                            Flickable {
                                                id: descriptionFlick
                                                contentWidth: descriptionTextInput.paintedWidth
                                                contentHeight: descriptionTextInput.paintedHeight
                                                anchors.left: parent.left
                                                anchors.right: parent.right
                                                anchors.leftMargin: 5
                                                anchors.rightMargin: 5
                                                interactive: false
                                                flickableDirection: Flickable.HorizontalFlick
                                                height: parent.height
                                                anchors.verticalCenter: parent.verticalCenter
                                                clip: true
                                                focus: false

                                                function ensureVisible(r) {
                                                    if (contentX >= r.x)
                                                        contentX = r.x;
                                                    else if (contentX+width <= r.x+r.width)
                                                        contentX = r.x+r.width-width;
                                                }

                                                StyledTextEdit {
                                                    id: descriptionTextInput
                                                    width: paintedWidth > descriptionFlick.width ? paintedWidth : descriptionFlick.width
                                                    height: descriptionFlick.height
                                                    text: description
                                                    focus: true
                                                    isActive: rowWrapper.isHighlighted
                                                    onTextChanged: model.editdescription = text
                                                    userDictEnabled: true

                                                    onActionRightClicked: {
                                                        console.log("Context menu for add word " + rightClickedWord)
                                                        var showAddToDict = filteredArtItemsModel.hasDescriptionWordSpellError(rowWrapper.delegateIndex, rightClickedWord)
                                                        wordRightClickMenu.showAddToDict = showAddToDict
                                                        wordRightClickMenu.word = rightClickedWord
                                                        wordRightClickMenu.showExpandPreset = false
                                                        wordRightClickMenu.popupIfNeeded()
                                                    }

                                                    Keys.onTabPressed: {
                                                        if (columnLayout.isWideEnough) {
                                                            titleTextInput.forceActiveFocus()
                                                            titleTextInput.cursorPosition = titleTextInput.text.length
                                                        } else {
                                                            flv.activateEdit()
                                                        }
                                                    }

                                                    onActiveFocusChanged: {
                                                        if (activeFocus) {
                                                            wrappersScope.updateCurrentIndex()
                                                        }
                                                    }

                                                    Keys.onBacktabPressed: {
                                                        filteredArtItemsModel.focusPreviousItem(rowWrapper.delegateIndex)
                                                    }

                                                    Keys.onPressed: {
                                                        if(event.matches(StandardKey.Paste)) {
                                                            var clipboardText = clipboard.getText();
                                                            if (Common.safeInsert(descriptionTextInput, clipboardText)) {
                                                                event.accepted = true
                                                            }
                                                        } else if ((event.key === Qt.Key_Return) || (event.key === Qt.Key_Enter)) {
                                                            event.accepted = true
                                                        } else if ((event.key === Qt.Key_Left) && (descriptionTextInput.cursorPosition == 0)) {
                                                            event.accepted = true
                                                        } else if ((event.key === Qt.Key_Right) &&
                                                                   (descriptionTextInput.cursorPosition == descriptionTextInput.length)) {
                                                            event.accepted = true
                                                        } else if ((event.key === Qt.Key_Down) || (event.key === Qt.Key_Up)) {
                                                            event.accepted = true
                                                        }
                                                    }

                                                    Component.onCompleted: {
                                                        var index = rowWrapper.getIndex()
                                                        artItemsModel.initDescriptionHighlighting(index, descriptionTextInput.textDocument)
                                                    }

                                                    onCursorRectangleChanged: descriptionFlick.ensureVisible(cursorRectangle)
                                                }
                                            }
                                        }

                                        StyledText {
                                            text: descriptionTextInput.length
                                            anchors.right: descriptionRect.right
                                            anchors.bottom: descriptionRect.top
                                            anchors.bottomMargin: 3
                                            isActive: rowWrapper.isHighlighted
                                        }

                                        Rectangle {
                                            id: titleRect
                                            height: 30
                                            visible: columnLayout.isWideEnough
                                            enabled: columnLayout.isWideEnough && artworksHost.isEditingAllowed
                                            width: columnLayout.isWideEnough ? ((columnLayout.width / 2) - 10) : 0
                                            anchors.right: parent.right
                                            anchors.top: descriptionText.bottom
                                            anchors.topMargin: 3
                                            color: rowWrapper.isHighlighted ? uiColors.inputBackgroundColor : uiColors.inputInactiveBackground
                                            border.color: uiColors.artworkActiveColor
                                            border.width: titleTextInput.activeFocus ? 1 : 0
                                            clip: true
                                            focus: false

                                            Flickable {
                                                id: titleFlick
                                                contentWidth: titleTextInput.paintedWidth
                                                contentHeight: titleTextInput.paintedHeight
                                                height: parent.height
                                                anchors.verticalCenter: parent.verticalCenter
                                                anchors.left: parent.left
                                                anchors.right: parent.right
                                                anchors.leftMargin: 5
                                                anchors.rightMargin: 5
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
                                                    width: paintedWidth > titleFlick.width ? paintedWidth : titleFlick.width
                                                    height: titleFlick.height
                                                    text: title
                                                    focus: true
                                                    isActive: rowWrapper.isHighlighted
                                                    onTextChanged: model.edittitle = text
                                                    userDictEnabled: true

                                                    onActionRightClicked: {
                                                        console.log("Context menu for add word " + rightClickedWord)
                                                        var showAddToDict = filteredArtItemsModel.hasTitleWordSpellError(rowWrapper.delegateIndex, rightClickedWord)
                                                        wordRightClickMenu.showAddToDict = showAddToDict
                                                        wordRightClickMenu.word = rightClickedWord
                                                        wordRightClickMenu.showExpandPreset = false
                                                        wordRightClickMenu.popupIfNeeded()
                                                    }

                                                    Keys.onTabPressed: {
                                                        flv.activateEdit()
                                                    }

                                                    Keys.onBacktabPressed: {
                                                        descriptionTextInput.forceActiveFocus()
                                                        descriptionTextInput.cursorPosition = descriptionTextInput.text.length
                                                    }

                                                    onActiveFocusChanged: {
                                                        if (activeFocus) {
                                                            wrappersScope.updateCurrentIndex()
                                                        }
                                                    }

                                                    Keys.onPressed: {
                                                        if (event.matches(StandardKey.Paste)) {
                                                            var clipboardText = clipboard.getText();
                                                            if (Common.safeInsert(titleTextInput, clipboardText)) {
                                                                event.accepted = true
                                                            }
                                                        } else if ((event.key === Qt.Key_Return) || (event.key === Qt.Key_Enter)) {
                                                            event.accepted = true
                                                        } else if ((event.key === Qt.Key_Left) && (titleTextInput.cursorPosition == 0)) {
                                                            event.accepted = true
                                                        } else if ((event.key === Qt.Key_Right) &&
                                                                   (titleTextInput.cursorPosition == titleTextInput.length)) {
                                                            event.accepted = true
                                                        } else if ((event.key === Qt.Key_Down) || (event.key === Qt.Key_Up)) {
                                                            event.accepted = true
                                                        }
                                                    }

                                                    Component.onCompleted: {
                                                        var index = rowWrapper.getIndex()
                                                        artItemsModel.initTitleHighlighting(index, titleTextInput.textDocument)
                                                    }

                                                    onCursorRectangleChanged: titleFlick.ensureVisible(cursorRectangle)
                                                }
                                            }
                                        }

                                        StyledText {
                                            text: titleTextInput.length
                                            anchors.right: titleRect.right
                                            anchors.bottom: titleRect.top
                                            anchors.bottomMargin: 3
                                            isActive: rowWrapper.isHighlighted
                                            visible: columnLayout.isWideEnough
                                        }

                                        StyledText {
                                            id: keywordsLabel
                                            anchors.left: parent.left
                                            anchors.top: descriptionRect.bottom
                                            anchors.topMargin: 7
                                            text: i18.n + qsTr("Keywords:")
                                            isActive: rowWrapper.isHighlighted
                                        }

                                        StyledText {
                                            anchors.left: keywordsLabel.right
                                            anchors.leftMargin: 3
                                            anchors.top: keywordsLabel.top
                                            text: "*"
                                            color: uiColors.destructiveColor
                                            visible: rowWrapper.keywordsModel.hasKeywordsSpellErrors
                                        }

                                        StyledText {
                                            text: keywordscount
                                            anchors.right: parent.right
                                            anchors.top: descriptionRect.bottom
                                            anchors.topMargin: 7
                                            isActive: rowWrapper.isHighlighted
                                        }

                                        Rectangle {
                                            id: keywordsWrapper
                                            anchors.top: keywordsLabel.bottom
                                            anchors.topMargin: 3
                                            anchors.left: parent.left
                                            anchors.right: parent.right
                                            height: 80*settingsModel.keywordSizeScale
                                            border.color: uiColors.artworkActiveColor
                                            border.width: flv.isFocused ? 1 : 0
                                            color: rowWrapper.isHighlighted ? uiColors.inputBackgroundColor : uiColors.inputInactiveBackground
                                            state: ""

                                            function removeKeyword(index) {
                                                artItemsModel.removeKeywordAt(rowWrapper.getIndex(), index)
                                            }

                                            function removeLastKeyword() {
                                                artItemsModel.removeLastKeyword(rowWrapper.getIndex())
                                            }

                                            function appendKeyword(keyword) {
                                                var added = artItemsModel.appendKeyword(rowWrapper.getIndex(), keyword)
                                                if (!added) {
                                                    keywordsWrapper.state = "blinked"
                                                    blinkTimer.start()
                                                }
                                            }

                                            function pasteKeywords(keywords) {
                                                artItemsModel.pasteKeywords(rowWrapper.getIndex(), keywords)
                                            }

                                            function expandLastKeywordAsPreset() {
                                                artItemsModel.expandLastAsPreset(rowWrapper.getIndex())
                                            }

                                            EditableTags {
                                                id: flv
                                                model: rowWrapper.keywordsModel
                                                anchors.fill: parent
                                                property int keywordHeight: uiManager.keywordHeight
                                                scrollStep: keywordHeight
                                                stealWheel: false
                                                focus: true
                                                enabled: artworksHost.isEditingAllowed

                                                function acceptCompletion(completionID) {
                                                    var accepted = artItemsModel.acceptCompletionAsPreset(rowWrapper.getIndex(), completionID);
                                                    if (!accepted) {
                                                        var completion = acSource.getCompletion(completionID)
                                                        flv.editControl.acceptCompletion(completion)
                                                    } else {
                                                        flv.editControl.acceptCompletion('')
                                                    }
                                                }

                                                delegate: KeywordWrapper {
                                                    id: kw
                                                    isHighlighted: rowWrapper.isHighlighted
                                                    delegateIndex: index
                                                    keywordText: keyword
                                                    itemHeight: flv.keywordHeight
                                                    hasSpellCheckError: !iscorrect
                                                    hasDuplicate: hasduplicate
                                                    onRemoveClicked: keywordsWrapper.removeKeyword(kw.delegateIndex)
                                                    closeIconDisabledColor: rowWrapper.isHighlighted ? uiColors.closeIconDisabledColor : uiColors.closeIconInactiveColor

                                                    onActionDoubleClicked: {
                                                        var callbackObject = {
                                                            onSuccess: function(replacement) {
                                                                artItemsModel.editKeyword(rowWrapper.getIndex(), kw.delegateIndex, replacement)
                                                            },
                                                            onClose: function() {
                                                                try {
                                                                    flv.activateEdit()
                                                                } catch (error) {
                                                                    console.warn(error)
                                                                }
                                                            }
                                                        }

                                                        Common.launchDialog("../Dialogs/EditKeywordDialog.qml",
                                                                            applicationWindow,
                                                                            {
                                                                                callbackObject: callbackObject,
                                                                                previousKeyword: keyword,
                                                                                keywordIndex: kw.delegateIndex,
                                                                                keywordsModel: filteredArtItemsModel.getBasicModel(rowWrapper.delegateIndex)
                                                                            })
                                                    }

                                                    onActionRightClicked: {
                                                        wordRightClickMenu.showAddToDict = !iscorrect
                                                        var keyword = kw.keywordText
                                                        wordRightClickMenu.word = keyword
                                                        filteredPresetsModel.searchTerm = keyword
                                                        wordRightClickMenu.showExpandPreset = (filteredPresetsModel.getItemsCount() !== 0 )
                                                        wordRightClickMenu.artworkIndex = rowWrapper.getIndex()
                                                        wordRightClickMenu.keywordIndex = kw.delegateIndex
                                                        wordRightClickMenu.popupIfNeeded()
                                                    }
                                                }

                                                onTagAdded: {
                                                    keywordsWrapper.appendKeyword(text)
                                                }

                                                onTagsPasted: {
                                                    keywordsWrapper.pasteKeywords(tagsList)
                                                }

                                                onRemoveLast: {
                                                    keywordsWrapper.removeLastKeyword()
                                                }

                                                onBackTabPressed: {
                                                    if (columnLayout.isWideEnough) {
                                                        titleTextInput.forceActiveFocus()
                                                        titleTextInput.cursorPosition = titleTextInput.text.length
                                                    } else {
                                                        descriptionTextInput.forceActiveFocus()
                                                        descriptionTextInput.cursorPosition = descriptionTextInput.text.length
                                                    }
                                                }

                                                onTabPressed: {
                                                    filteredArtItemsModel.focusNextItem(rowWrapper.delegateIndex)
                                                }

                                                onCopyRequest: clipboard.setText(keywordsstring)

                                                onCompletionRequested: {
                                                    filteredArtItemsModel.generateCompletions(prefix,
                                                                                              rowWrapper.delegateIndex)
                                                }

                                                onExpandLastAsPreset: {
                                                    keywordsWrapper.expandLastKeywordAsPreset()
                                                }

                                                onEditActivated: {
                                                    wrappersScope.updateCurrentIndex()
                                                    flv.activateEdit()
                                                }

                                                onRightClickedInside: {
                                                    filteredPresetsModel.searchTerm = ''
                                                    presetsMenu.artworkIndex = rowWrapper.getIndex()
                                                    presetsMenu.popup()
                                                }
                                            }

                                            CustomScrollbar {
                                                anchors.topMargin: -5
                                                anchors.bottomMargin: -5
                                                anchors.rightMargin: -15
                                                flickable: flv
                                                handleColor: rowWrapper.isHighlighted ? uiColors.artworkActiveColor : uiColors.panelSelectedColor
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

                                        RowLayout {
                                            anchors.left: parent.left
                                            anchors.right: parent.right
                                            anchors.rightMargin: 3
                                            anchors.top: keywordsWrapper.bottom
                                            anchors.topMargin: 3
                                            spacing: 5

                                            Item {
                                                Layout.fillWidth: true
                                            }

                                            StyledLink {
                                                id: fixSpellingText
                                                text: i18.n + qsTr("Fix spelling")
                                                property bool canBeShown: rowWrapper.keywordsModel ? rowWrapper.keywordsModel.hasAnySpellingError : false
                                                enabled: canBeShown
                                                visible: canBeShown
                                                isActive: true //rowWrapper.isHighlighted
                                                onClicked: { fixSpelling(rowWrapper.getIndex()) }
                                            }

                                            StyledText {
                                                enabled: fixSpellingText.canBeShown
                                                visible: fixSpellingText.canBeShown
                                                text: "|"
                                                isActive: rowWrapper.isHighlighted
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            StyledLink {
                                                id: removeDuplicatesText
                                                text: i18.n + qsTr("Show duplicates")
                                                property bool canBeShown: rowWrapper.keywordsModel ? rowWrapper.keywordsModel.hasDuplicates : false
                                                enabled: canBeShown
                                                visible: canBeShown
                                                isActive: true //rowWrapper.isHighlighted
                                                onClicked: { fixDuplicatesAction(rowWrapper.getIndex()) }
                                            }

                                            StyledText {
                                                enabled: removeDuplicatesText.canBeShown
                                                visible: removeDuplicatesText.canBeShown
                                                text: "|"
                                                isActive: rowWrapper.isHighlighted
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            StyledLink {
                                                id: suggestLink
                                                text: i18.n + qsTr("Suggest keywords")
                                                property bool canBeShown: (keywordscount < warningsModel.minKeywordsCount) && (!removeDuplicatesText.canBeShown) && (!fixSpellingText.canBeShown)
                                                visible: canBeShown
                                                enabled: canBeShown
                                                onClicked: { suggestKeywords(rowWrapper.getIndex()) }
                                            }

                                            StyledText {
                                                visible: suggestLink.canBeShown
                                                enabled: suggestLink.canBeShown
                                                text: "|"
                                                isActive: rowWrapper.isHighlighted
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            StyledLink {
                                                id: copyLink
                                                text: i18.n + qsTr("Copy")
                                                property bool canBeShown: (keywordscount > 0) && (!suggestLink.canBeShown) && (!removeDuplicatesText.canBeShown) && (!fixSpellingText.canBeShown)
                                                enabled: canBeShown
                                                visible: canBeShown
                                                onClicked: clipboard.setText(keywordsstring)
                                            }

                                            StyledText {
                                                visible: copyLink.canBeShown
                                                enabled: copyLink.canBeShown
                                                text: "|"
                                                isActive: rowWrapper.isHighlighted
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            Item {
                                                width: childrenRect.width
                                                height: moreLink.height

                                                StyledText {
                                                    id: moreLink
                                                    color: enabled ? (moreMA.pressed ? uiColors.linkClickedColor : uiColors.artworkActiveColor) : (isActive ? uiColors.labelActiveForeground : uiColors.labelInactiveForeground)
                                                    text: i18.n + qsTr("More")
                                                    property bool isActive: true //rowWrapper.isHighlighted
                                                    anchors.verticalCenter: parent.verticalCenter
                                                    // \u25BE - triangle
                                                }

                                                TriangleElement {
                                                    anchors.left: moreLink.right
                                                    anchors.leftMargin: 4
                                                    anchors.verticalCenter: parent.verticalCenter
                                                    // anchors.verticalCenterOffset: isFlipped ? height*0.3 : 0
                                                    //anchors.verticalCenter: parent.verticalCenter
                                                    color: moreLink.color
                                                    isFlipped: true
                                                    width: 8
                                                    height: 6
                                                }

                                                MouseArea {
                                                    id: moreMA
                                                    anchors.fill: parent
                                                    hoverEnabled: true
                                                    cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
                                                    onClicked: {
                                                        // strange bug with clicking on the keywords field
                                                        if (!containsMouse) { return; }

                                                        keywordsMoreMenu.editableTags = flv
                                                        keywordsMoreMenu.keywordsCount = keywordscount
                                                        keywordsMoreMenu.artworkIndex = rowWrapper.getIndex()
                                                        keywordsMoreMenu.filteredIndex = rowWrapper.delegateIndex
                                                        keywordsMoreMenu.hasSpellingErrors = rowWrapper.keywordsModel ? rowWrapper.keywordsModel.hasAnySpellingErrors() : false
                                                        keywordsMoreMenu.hasDuplicates = rowWrapper.keywordsModel ? rowWrapper.keywordsModel.hasDuplicates : false
                                                        keywordsMoreMenu.popup()
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    Connections {
                        target: artItemsModel
                        onArtworksChanged: artworksHost.forceUpdateArtworks(needToMoveCurrentItem)
                    }

                    Connections {
                        target: filteredArtItemsModel
                        onAfterInvalidateFilter: {
                            artworksHost.forceUpdateArtworks(true)
                            artworksHost.positionViewAtBeginning()
                        }
                    }
                }

                CustomScrollbar {
                    id: mainScrollBar
                    anchors.topMargin: 0
                    anchors.bottomMargin: 0
                    anchors.rightMargin: -10
                    flickable: artworksHost
                }
            }

            Item {
                visible: (artworksHost.count == 0) && (!commonCenterItem.visible)
                anchors.fill: parent

                DonateComponent {
                    anchors.centerIn: parent
                    anchors.verticalCenterOffset: -30
                }
            }

            Item {
                id: commonCenterItem
                visible: {
                    if (artworksHost.count == 0) {
                        if (artworkRepository.artworksSourcesCount > 0) {
                            return true
                        }

                        if ((switcher.isDonationCampaign1Active) &&
                                (!switcher.isDonateCampaign1LinkClicked)) {
                            return false
                        }

                        return true
                    } else {
                        return false
                    }
                }

                anchors.fill: parent

                RowLayout {
                    anchors.centerIn: parent
                    spacing: 5

                    StyledText {
                        text: i18.n + qsTr("No items available.")
                        isActive: false
                    }

                    StyledLink {
                        text: i18.n + qsTr("Add files", "link")
                        onClicked: chooseArtworksDialog.open()
                    }

                    StyledText {
                        text: i18.n + qsTr("or")
                        isActive: false
                    }

                    StyledLink {
                        text: i18.n + qsTr("clear the filter")
                        onClicked: clearFilter()
                    }
                }
            }
        }
    }

    ClipboardHelper {
        id: clipboard
    }
}


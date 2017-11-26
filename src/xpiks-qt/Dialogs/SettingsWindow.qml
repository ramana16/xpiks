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
import QtQuick.Layouts 1.3
import "../Constants"
import "../Components"
import "../StyledControls"
import "../Common.js" as Common
import "../Constants/UIConfig.js" as UIConfig

ApplicationWindow {
    id: settingsWindow
    modality: "ApplicationModal"
    title: i18.n + qsTr("Settings")
    width: 750
    height: 500
    minimumWidth: width
    maximumWidth: width
    minimumHeight: height
    maximumHeight: height
    flags: Qt.Dialog

    signal dialogDestruction();
    signal refreshProxy();

    onClosing: {
        saveSettings()
        dialogDestruction();
    }

    function closeSettings() {
        settingsWindow.close()
    }

    function saveSettings() {
        settingsModel.keywordSizeScale = uxTab.sizeSliderValue
        settingsModel.scrollSpeedScale = uxTab.scrollSpeedScale
        settingsModel.selectedThemeIndex = uxTab.themeIndex
        settingsModel.userStatistics = secTab.useStatistics
        settingsModel.saveAllValues()
        uiColors.applyTheme(settingsModel.selectedThemeIndex)
    }

    function onCancelMP(firstTime) {
        settingsModel.onMasterPasswordUnset(firstTime);
    }

    function onMasterPasswordSet() {
        settingsModel.onMasterPasswordSet();
    }

    function onProxySettingsSet() {
        console.log('UI:SettingWindow # Proxy settings changed')
        settingsModel.useProxy = true
        settingsWindow.refreshProxy()
    }

    function onProxySettingNotSet(firstTimeParam){
        if (firstTimeParam) {
            console.log('UI:SettingWindow # Proxy settings not set. Disabling UseProxy')
            settingsModel.useProxy=false
            settingsWindow.refreshProxy()
        }
    }

    function openMasterPasswordDialog(firstTimeParam) {
        var callbackObject = {
            onCancel: onCancelMP,
            onSuccess: onMasterPasswordSet
        }

        Common.launchDialog("Dialogs/MasterPasswordSetupDialog.qml",
                            settingsWindow,
                            {
                                componentParent: settingsWindow,
                                firstTime: firstTimeParam,
                                callbackObject: callbackObject
                            });
    }

    function openProxyDialog(firstTimeParam) {
        var callbackObject = {
            onSuccess: onProxySettingsSet,
            onCancel: onProxySettingNotSet
        }
        Common.launchDialog("Dialogs/ProxySetupDialog.qml",
                            settingsWindow,
                            {
                                componentParent: settingsWindow,
                                firstTime: firstTimeParam,
                                callbackObject: callbackObject
                            });
    }

    FileDialog {
        id: exifToolFileDialog
        title: "Please choose ExifTool location"
        selectExisting: true
        selectMultiple: false
        nameFilters: [ "All files (*)" ]

        onAccepted: {
            console.log("UI:SettingsWindow # You chose: " + exifToolFileDialog.fileUrl)
            var path = exifToolFileDialog.fileUrl.toString().replace(/^(file:\/{3})/,"");
            settingsModel.exifToolPath = decodeURIComponent(path);
        }

        onRejected: {
            console.log("UI:SettingsWindow # File dialog canceled")
        }
    }

    FileDialog {
        id: dictPathDialog
        title: "Please choose dictionaries location"
        selectExisting: true
        selectMultiple: false
        selectFolder: true
        nameFilters: [ "All files (*)" ]

        onAccepted: {
            console.log("UI:SettingsWindow # You chose: " + dictPathDialog.folder)
            var path = dictPathDialog.folder.toString().replace(/^(file:\/{2})/,"");
            settingsModel.dictionaryPath = decodeURIComponent(path);
        }

        onRejected: {
            console.log("UI:SettingsWindow # File dialog canceled")
        }
    }

    function turnMasterPasswordOff () {
        secretsManager.resetMasterPassword()
        settingsModel.clearMasterPasswordSettings();
    }

    MessageDialog {
        id: masterPasswordOffWarningDialog
        title: i18.n + qsTr("Warning")
        text: i18.n + qsTr("Switching off master password will make your passwords storage less secure. Continue?")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            if (secretsManager.isMasterPasswordSet()) {
                var callbackObject = {
                    onSuccess: turnMasterPasswordOff,
                    onFail: function() {
                        settingsModel.mustUseMasterPassword = true;
                        settingsModel.raiseMasterPasswordSignal()
                    }
                }

                Common.launchDialog("Dialogs/EnterMasterPasswordDialog.qml",
                                    settingsWindow,
                                    {componentParent: settingsWindow, callbackObject: callbackObject})
            } else {
                turnMasterPasswordOff()
            }
        }

        onNo: {
            settingsModel.mustUseMasterPassword = true
            settingsModel.raiseMasterPasswordSignal()
        }
    }

    MessageDialog {
        id: resetSettingsDialog
        title: i18.n + qsTr("Warning")
        text: i18.n + qsTr("Are you sure you want to reset all settings? \nThis action cannot be undone.")
        standardButtons: StandardButton.Yes | StandardButton.No

        onYes: {
            secretsManager.removeMasterPassword()
            settingsModel.resetAllValues()
        }
    }

    MessageDialog {
        id: clearDictionaryDialog
        title: i18.n + qsTr("Warning")
        text: i18.n + qsTr("Are you sure you want to clear user dictionary? \nThis action cannot be undone.")
        standardButtons: StandardButton.Yes | StandardButton.No

        onYes: {
            spellCheckService.clearUserDictionary()
        }
    }

    MessageDialog {
        id: resetMPDialog
        title: i18.n + qsTr("Warning")
        text: i18.n + qsTr("Are you sure you want to reset Master password? \nAll upload hosts' passwords will be purged.")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            secretsManager.removeMasterPassword()
            settingsModel.clearMasterPasswordSettings()
        }
    }

    Rectangle {
        id: globalHost
        color: uiColors.popupBackgroundColor
        anchors.fill: parent

        Component.onCompleted: focus = true
        Keys.onEscapePressed: closeSettings()

        Rectangle {
            id: leftPanel
            color: uiColors.defaultControlColor
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: footer.top
            width: 150

            ListView {
                id: tabNamesListView
                model: [i18.n + qsTr("Behavior"),
                    i18.n + qsTr("Interface"),
                    i18.n + qsTr("External"),
                    i18.n + qsTr("Upload"),
                    i18.n + qsTr("System")]
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
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

                onCurrentIndexChanged: {
                    //csvExportModel.setCurrentItem(tabNamesListView.currentIndex)
                }

                delegate: Rectangle {
                    id: sourceWrapper
                    property variant myData: modelData
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
                            if (tabNamesListView.currentIndex != sourceWrapper.delegateIndex) {
                                tabNamesListView.currentIndex = sourceWrapper.delegateIndex
                                tabsHost.currentIndex = sourceWrapper.delegateIndex
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
                            text: sourceWrapper.myData
                            elide: Text.ElideMiddle
                            font.bold: sourceWrapper.isCurrent
                            horizontalAlignment: Text.AlignLeft
                        }

                        Item {
                            id: placeholder2
                            width: 15
                        }
                    }
                }
            }

            CustomScrollbar {
                id: settingTabsScroll
                anchors.topMargin: 0
                anchors.bottomMargin: 0
                anchors.rightMargin: 0
                flickable: tabNamesListView
                canShow: !rightPanelMA.containsMouse
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

            StackLayout {
                id: tabsHost
                anchors.fill: parent

                Item {
                    id: behaviorTab
                    signal resetRequested()
                    anchors.fill: parent
                    anchors.margins: 20

                    Connections {
                        target: settingsModel
                        onSettingsReset: { resetRequested() }
                    }

                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 20
                        anchors.margins: 20

                        Flow {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            spacing: 10
                            property real itemHeight: 40
                            property real itemWidth: 250

                            Item {
                                width: parent.itemWidth
                                height: parent.itemHeight

                                StyledCheckbox {
                                    anchors.left: parent.left
                                    anchors.leftMargin: 10
                                    anchors.verticalCenter: parent.verticalCenter
                                    id: autoDuplicateSearchCheckbox
                                    text: i18.n + qsTr("Detect duplicates automatically")
                                    onCheckedChanged: {
                                        settingsModel.detectDuplicates = checked
                                    }
                                    function onResetRequested() {
                                        checked = settingsModel.detectDuplicates
                                    }

                                    Component.onCompleted: {
                                        checked = settingsModel.detectDuplicates
                                        behaviorTab.resetRequested.connect(autoDuplicateSearchCheckbox.onResetRequested)
                                    }
                                }
                            }

                            Item {
                                width: parent.itemWidth
                                height: parent.itemHeight

                                StyledCheckbox {
                                    anchors.left: parent.left
                                    anchors.leftMargin: 10
                                    anchors.verticalCenter: parent.verticalCenter
                                    id: autoSpellCheckCheckbox
                                    text: i18.n + qsTr("Check spelling automatically")
                                    onCheckedChanged: {
                                        settingsModel.useSpellCheck = checked
                                    }
                                    function onResetRequested() {
                                        checked = settingsModel.useSpellCheck
                                    }

                                    Component.onCompleted: {
                                        checked = settingsModel.useSpellCheck
                                        behaviorTab.resetRequested.connect(autoSpellCheckCheckbox.onResetRequested)
                                    }
                                }
                            }

                            Item {
                                width: parent.itemWidth
                                height: parent.itemHeight

                                StyledCheckbox {
                                    anchors.left: parent.left
                                    anchors.leftMargin: 10
                                    anchors.verticalCenter: parent.verticalCenter
                                    id: searchUsingAndCheckbox
                                    text: i18.n + qsTr("Search match all terms")
                                    onCheckedChanged: {
                                        settingsModel.searchUsingAnd = checked
                                    }
                                    function onResetRequested() {
                                        checked = settingsModel.searchUsingAnd
                                    }
                                    Component.onCompleted: {
                                        checked = settingsModel.searchUsingAnd
                                        behaviorTab.resetRequested.connect(searchUsingAndCheckbox.onResetRequested)
                                    }
                                }
                            }

                            Item {
                                width: parent.itemWidth
                                height: parent.itemHeight

                                StyledCheckbox {
                                    anchors.left: parent.left
                                    anchors.leftMargin: 10
                                    anchors.verticalCenter: parent.verticalCenter
                                    id: searchByFilepathCheckbox
                                    text: i18.n + qsTr("Search by path")
                                    onCheckedChanged: {
                                        settingsModel.searchByFilepath = checked
                                    }
                                    function onResetRequested() {
                                        checked = settingsModel.searchByFilepath
                                    }
                                    Component.onCompleted: {
                                        checked = settingsModel.searchByFilepath
                                        behaviorTab.resetRequested.connect(searchByFilepathCheckbox.onResetRequested)
                                    }
                                }
                            }

                            Item {
                                width: parent.itemWidth
                                height: parent.itemHeight

                                StyledCheckbox {
                                    anchors.left: parent.left
                                    anchors.leftMargin: 10
                                    anchors.verticalCenter: parent.verticalCenter
                                    id: autoCompleteKeywordsCheckbox
                                    text: i18.n + qsTr("Autocomplete keywords")
                                    onCheckedChanged: {
                                        settingsModel.useKeywordsAutoComplete = checked
                                    }
                                    function onResetRequested() {
                                        checked = settingsModel.useKeywordsAutoComplete
                                    }

                                    Component.onCompleted: {
                                        checked = settingsModel.useKeywordsAutoComplete
                                        behaviorTab.resetRequested.connect(autoCompleteKeywordsCheckbox.onResetRequested)
                                    }
                                }
                            }

                            Item {
                                width: parent.itemWidth
                                height: parent.itemHeight

                                StyledCheckbox {
                                    anchors.left: parent.left
                                    anchors.leftMargin: 10
                                    anchors.verticalCenter: parent.verticalCenter
                                    id: autoCompletePresetsCheckbox
                                    text: i18.n + qsTr("Autocomplete presets")
                                    onCheckedChanged: {
                                        settingsModel.usePresetsAutoComplete = checked
                                    }
                                    function onResetRequested() {
                                        checked = settingsModel.usePresetsAutoComplete
                                    }

                                    Component.onCompleted: {
                                        checked = settingsModel.usePresetsAutoComplete
                                        behaviorTab.resetRequested.connect(autoCompletePresetsCheckbox.onResetRequested)
                                    }
                                }
                            }

                            Item {
                                width: parent.itemWidth
                                height: parent.itemHeight

                                StyledCheckbox {
                                    anchors.left: parent.left
                                    anchors.leftMargin: 10
                                    anchors.verticalCenter: parent.verticalCenter
                                    id: searchForVectorCheckbox
                                    text: i18.n + qsTr("Attach vector automatically")
                                    onCheckedChanged: {
                                        settingsModel.autoFindVectors = checked
                                    }
                                    function onResetRequested() {
                                        checked = settingsModel.autoFindVectors
                                    }

                                    Component.onCompleted: {
                                        checked = settingsModel.autoFindVectors
                                        behaviorTab.resetRequested.connect(searchForVectorCheckbox.onResetRequested)
                                    }
                                }
                            }
                        }

                        Item {
                            Layout.fillHeight: true
                        }
                    }
                }

                Item {
                    id: uxTab
                    property real sizeSliderValue: settingsModel.keywordSizeScale
                    property real scrollSpeedScale: settingsModel.scrollSpeedScale
                    property int themeIndex: settingsModel.selectedThemeIndex
                    signal resetRequested()
                    anchors.fill: parent
                    anchors.margins: 20

                    Connections {
                        target: settingsModel
                        onSettingsReset: { resetRequested() }
                    }

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 20
                        anchors.rightMargin: 20
                        anchors.topMargin: 20
                        anchors.bottomMargin: 10
                        spacing: 30

                        RowLayout {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            z: 10000
                            spacing: 20

                            StyledText {
                                text: i18.n + qsTr("Theme:")
                            }

                            ComboBoxPopup {
                                id: themeComboBox
                                model: uiColors.getThemeNames()
                                showColorSign: false
                                width: 130
                                height: 24
                                itemHeight: 28
                                dropDownWidth: 130
                                glowEnabled: true
                                glowTopMargin: 2
                                globalParent: globalHost

                                onComboItemSelected: {
                                    uxTab.themeIndex = themeComboBox.selectedIndex
                                }

                                function onResetRequested() {
                                    selectedIndex  = settingsModel.selectedThemeIndex
                                }

                                Component.onCompleted: {
                                    themeComboBox.selectedIndex = settingsModel.selectedThemeIndex
                                    uxTab.resetRequested.connect(themeComboBox.onResetRequested)
                                }
                            }
                        }

                        RowLayout {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            spacing: 20

                            StyledCheckbox {
                                id: fitArtworksCheckbox
                                text: i18.n + qsTr("Fit artwork's preview")

                                onCheckedChanged: {
                                    settingsModel.fitSmallPreview = checked
                                }

                                function onResetRequested() {
                                    checked =  settingsModel.fitSmallPreview
                                }

                                Component.onCompleted: {
                                    checked = settingsModel.fitSmallPreview
                                    uxTab.resetRequested.connect(fitArtworksCheckbox.onResetRequested)
                                }
                            }
                        }

                        Item {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            height: row.height

                            RowLayout {
                                anchors.left: parent.left
                                id: row
                                spacing: 10

                                StyledText {
                                    text: i18.n + qsTr("Keywords size")
                                }

                                StyledSlider {
                                    id: keywordSizeSlider
                                    width: 150
                                    minimumValue: 1.0
                                    maximumValue: 1.2
                                    stepSize: 0.0001
                                    orientation: Qt.Horizontal

                                    Component.onCompleted: {
                                        keywordSizeSlider.value = uxTab.sizeSliderValue
                                        uxTab.resetRequested.connect(keywordSizeSlider.onResetRequested)
                                        // do not use direct onValueChanged because of glitch with reassignning min. value
                                        keywordSizeSlider.onValueChanged.connect(keywordSizeSlider.valueChangedHandler)
                                    }

                                    function onResetRequested() {
                                        value = settingsModel.keywordSizeScale
                                        uxTab.sizeSliderValue = value
                                    }

                                    function valueChangedHandler() {
                                        uxTab.sizeSliderValue = value
                                    }
                                }
                            }

                            Rectangle {
                                id: keywordPreview
                                anchors.left: row.right
                                anchors.leftMargin: 10
                                anchors.verticalCenter: row.verticalCenter
                                color: uiColors.inputForegroundColor

                                width: childrenRect.width
                                height: childrenRect.height

                                Item {
                                    anchors.left: parent.left
                                    anchors.top: parent.top
                                    id: tagTextRect
                                    width: childrenRect.width + 5
                                    height: 20*uxTab.sizeSliderValue + (uxTab.sizeSliderValue - 1)*10

                                    StyledText {
                                        id: keywordText
                                        anchors.left: parent.left
                                        anchors.leftMargin: 5 + (uxTab.sizeSliderValue - 1)*10
                                        anchors.top: parent.top
                                        anchors.bottom: parent.bottom
                                        verticalAlignment: Text.AlignVCenter
                                        text: i18.n + qsTr("keyword", "standalone")
                                        color: uiColors.defaultControlColor
                                        font.pixelSize: UIConfig.fontPixelSize * uxTab.sizeSliderValue
                                    }
                                }

                                Item {
                                    anchors.left: tagTextRect.right
                                    anchors.top: parent.top
                                    height: 20 * uxTab.sizeSliderValue + (uxTab.sizeSliderValue - 1)*10
                                    width: height

                                    CloseIcon {
                                        isPlus: false
                                        width: 14*uxTab.sizeSliderValue
                                        height: 14*uxTab.sizeSliderValue
                                        isActive: true
                                        anchors.centerIn: parent
                                    }
                                }
                            }
                        }

                        Item {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            height: childrenRect.height

                            RowLayout {
                                anchors.left: parent.left
                                spacing: 10

                                StyledText {
                                    text: i18.n + qsTr("Scroll sensitivity")
                                }

                                StyledSlider {
                                    id: scrollSensivitySlider
                                    width: 150
                                    minimumValue: 0.1
                                    maximumValue: 2.0
                                    stepSize: 0.01
                                    orientation: Qt.Horizontal

                                    Component.onCompleted: {
                                        scrollSensivitySlider.value = uxTab.scrollSpeedScale
                                        uxTab.resetRequested.connect(scrollSensivitySlider.onResetRequested)
                                        // do not use direct onValueChanged because of glitch with reassignning min. value
                                        scrollSensivitySlider.onValueChanged.connect(scrollSensivitySlider.valueChangedHandler)
                                    }

                                    function onResetRequested() {
                                        value = settingsModel.scrollSpeedScale
                                        uxTab.scrollSpeedScale = value
                                    }

                                    function valueChangedHandler() {
                                        uxTab.scrollSpeedScale = value
                                    }
                                }
                            }
                        }

                        RowLayout {
                            width: parent.width
                            spacing: 10

                            StyledText {
                                horizontalAlignment: Text.AlignLeft
                                text: i18.n + qsTr("Undo dismiss duration:")
                            }

                            Rectangle {
                                color: enabled ? uiColors.inputBackgroundColor : uiColors.inputInactiveBackground
                                border.color: uiColors.artworkActiveColor
                                border.width: dismissDuration.activeFocus ? 1 : 0
                                width: 115
                                height: UIConfig.textInputHeight
                                clip: true

                                StyledTextInput {
                                    id: dismissDuration
                                    text: settingsModel.dismissDuration
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    anchors.leftMargin: 5
                                    anchors.rightMargin: 5
                                    anchors.verticalCenter: parent.verticalCenter
                                    onTextChanged: {
                                        if (text.length > 0) {
                                            settingsModel.dismissDuration = parseInt(text)
                                        }
                                    }

                                    function onResetRequested() {
                                        text = settingsModel.dismissDuration
                                    }

                                    validator: IntValidator {
                                        bottom: 1
                                        top: 100
                                    }
                                }
                            }

                            StyledText {
                                text: i18.n + qsTr("(seconds)")
                                isActive: false
                            }
                        }

                        Item {
                            Layout.fillHeight: true
                        }
                    }
                }

                Item {
                    id: extTab
                    signal resetRequested()
                    anchors.fill: parent
                    anchors.margins: 20

                    Connections {
                        target: settingsModel
                        onSettingsReset: { resetRequested() }
                    }

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 5
                        anchors.rightMargin: 20
                        anchors.topMargin: 20
                        anchors.bottomMargin: 10
                        spacing: 0

                        StyledCheckbox {
                            id: useExifToolCheckbox
                            text: i18.n + qsTr("Use ExifTool")
                            onCheckedChanged: {
                                settingsModel.useExifTool = checked
                            }
                            function onResetRequested() {
                                checked = settingsModel.useExifTool
                            }
                            Component.onCompleted: {
                                checked = settingsModel.useExifTool
                                extTab.resetRequested.connect(useExifToolCheckbox.onResetRequested)
                            }
                        }

                        Item {
                            height: 20
                        }

                        StyledText {
                            enabled: settingsModel.useExifTool
                            isActive: useExifToolCheckbox.checked
                            text: i18.n + qsTr("ExifTool path:")
                        }

                        Item {
                            height: 10
                        }

                        RowLayout {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            spacing: 20

                            Rectangle {
                                color: enabled ? uiColors.inputBackgroundColor : uiColors.inputInactiveBackground
                                border.color: uiColors.artworkActiveColor
                                border.width: exifToolText.activeFocus ? 1 : 0
                                Layout.fillWidth: true
                                height: UIConfig.textInputHeight
                                clip: true
                                enabled: settingsModel.useExifTool

                                StyledTextInput {
                                    id: exifToolText
                                    text: settingsModel.exifToolPath
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    anchors.leftMargin: 5
                                    anchors.rightMargin: 5
                                    anchors.verticalCenter: parent.verticalCenter
                                    onTextChanged: settingsModel.exifToolPath = text

                                    function onResetRequested() {
                                        text = settingsModel.exifToolPath
                                    }

                                    Component.onCompleted: {
                                        extTab.resetRequested.connect(exifToolText.onResetRequested)
                                    }
                                }
                            }

                            StyledButton {
                                text: i18.n + qsTr("Select...")
                                width: 100
                                onClicked: exifToolFileDialog.open()
                                enabled: settingsModel.useExifTool
                            }

                            StyledButton {
                                text: i18.n + qsTr("Reset")
                                width: 100
                                onClicked: settingsModel.resetExifTool()
                                enabled: settingsModel.useExifTool
                            }
                        }

                        Item {
                            height: 20
                        }

                        Rectangle {
                            height: 1
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.leftMargin: 10
                            anchors.rightMargin: 10
                            color: uiColors.panelSelectedColor
                        }

                        Item {
                            height: 20
                        }

                        StyledText {
                            text: i18.n + qsTr("Dictionary path:")
                            enabled: Qt.platform.os === "linux"
                            visible: enabled
                        }

                        Item {
                            height: 10
                            enabled: Qt.platform.os === "linux"
                            visible: enabled
                        }

                        RowLayout {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            spacing: 20
                            enabled: Qt.platform.os === "linux"
                            visible: enabled

                            Rectangle {
                                color: enabled ? uiColors.inputBackgroundColor : uiColors.inputInactiveBackground
                                border.color: uiColors.artworkActiveColor
                                border.width: dictText.activeFocus ? 1 : 0
                                Layout.fillWidth: true
                                height: UIConfig.textInputHeight
                                clip: true

                                StyledTextInput {
                                    id: dictText
                                    text: settingsModel.dictionaryPath
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    anchors.leftMargin: 5
                                    anchors.rightMargin: 5
                                    anchors.verticalCenter: parent.verticalCenter
                                    onTextChanged: settingsModel.dictionaryPath = text
                                }
                            }

                            StyledButton {
                                text: i18.n + qsTr("Select...")
                                width: 100
                                onClicked: dictPathDialog.open()
                            }

                            StyledButton {
                                text: i18.n + qsTr("Reset")
                                width: 100
                                onClicked: settingsModel.resetDictPath()
                            }
                        }

                        Item {
                            height: 20
                            enabled: Qt.platform.os === "linux"
                            visible: enabled
                        }

                        Rectangle {
                            height: 1
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.leftMargin: 10
                            anchors.rightMargin: 10
                            color: uiColors.panelSelectedColor
                            enabled: Qt.platform.os === "linux"
                            visible: enabled
                        }

                        Item {
                            height: 20
                            enabled: Qt.platform.os === "linux"
                            visible: enabled
                        }

                        StyledButton {
                            width: 200
                            text: i18.n + qsTr("Manage user dictionary")
                            onClicked: {
                                userDictEditModel.initializeModel()
                                Common.launchDialog("../Dialogs/UserDictEditDialog.qml",
                                                    settingsWindow,
                                                    {
                                                        componentParent: settingsWindow
                                                    })
                            }
                        }

                        Item {
                            Layout.fillHeight: true
                        }
                    }
                }

                Item {
                    id: uploadTab
                    signal resetRequested()
                    anchors.fill: parent
                    anchors.margins: 20

                    Connections {
                        target: settingsModel
                        onSettingsReset: { resetRequested() }
                    }

                    ColumnLayout {
                        spacing: 20
                        anchors.fill: parent
                        anchors.leftMargin: 20
                        anchors.rightMargin: 20
                        anchors.topMargin: 20
                        anchors.bottomMargin: 10

                        RowLayout {
                            width: parent.width
                            spacing: 10

                            StyledText {
                                Layout.preferredWidth: 130
                                horizontalAlignment: Text.AlignRight
                                text: i18.n + qsTr("File upload timeout:")
                            }

                            Rectangle {
                                color: enabled ? uiColors.inputBackgroundColor : uiColors.inputInactiveBackground
                                border.width: timeoutSeconds.activeFocus ? 1 : 0
                                border.color: uiColors.artworkActiveColor
                                width: 115
                                height: UIConfig.textInputHeight
                                clip: true

                                StyledTextInput {
                                    id: timeoutSeconds
                                    text: settingsModel.uploadTimeout
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    anchors.leftMargin: 5
                                    anchors.rightMargin: 5
                                    anchors.verticalCenter: parent.verticalCenter
                                    onTextChanged: {
                                        if (text.length > 0) {
                                            settingsModel.uploadTimeout = parseInt(text)
                                        }
                                    }

                                    function onResetRequested() {
                                        text = settingsModel.uploadTimeout
                                    }

                                    Component.onCompleted: {
                                        uploadTab.resetRequested.connect(timeoutSeconds.onResetRequested)
                                    }

                                    KeyNavigation.tab: maxParallelUploads
                                    validator: IntValidator {
                                        bottom: 0
                                        top: 300
                                    }
                                }
                            }

                            StyledText {
                                text: i18.n + qsTr("(seconds)")
                                isActive: false
                            }
                        }

                        RowLayout {
                            width: parent.width
                            spacing: 10

                            StyledText {
                                Layout.preferredWidth: 130
                                horizontalAlignment: Text.AlignRight
                                text: i18.n + qsTr("Max parallel uploads:")
                            }

                            Rectangle {
                                color: enabled ? uiColors.inputBackgroundColor : uiColors.inputInactiveBackground
                                border.width: maxParallelUploads.activeFocus ? 1 : 0
                                border.color: uiColors.artworkActiveColor
                                width: 115
                                height: UIConfig.textInputHeight
                                clip: true

                                StyledTextInput {
                                    id: maxParallelUploads
                                    text: settingsModel.maxParallelUploads
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    anchors.leftMargin: 5
                                    anchors.rightMargin: 5
                                    anchors.verticalCenter: parent.verticalCenter
                                    onTextChanged: {
                                        if (text.length > 0) {
                                            settingsModel.maxParallelUploads = parseInt(text)
                                        }
                                    }

                                    function onResetRequested() {
                                        text = settingsModel.maxParallelUploads
                                    }

                                    Component.onCompleted: {
                                        uploadTab.resetRequested.connect(maxParallelUploads.onResetRequested)
                                    }
                                    KeyNavigation.backtab: timeoutSeconds
                                    validator: IntValidator {
                                        bottom: 1
                                        top: 4
                                    }
                                }
                            }

                            StyledText {
                                text: i18.n + qsTr("(takes effect after relaunch)")
                                isActive: false
                            }
                        }

                        RowLayout {
                            width: parent.width
                            spacing: 10

                            StyledCheckbox {
                                id: useProxyCheckbox
                                text: i18.n + qsTr("Use Proxy")

                                onClicked: {
                                    if (checked) {
                                        openProxyDialog(true)
                                    } else {
                                        settingsModel.useProxy = false
                                    }
                                }

                                function refreshProxyHandler() {
                                    checked = settingsModel.useProxy
                                }

                                Component.onCompleted: {
                                    uploadTab.resetRequested.connect(refreshProxyHandler)
                                    checked = settingsModel.useProxy
                                    settingsWindow.refreshProxy.connect(refreshProxyHandler)
                                }
                            }

                            Item {
                                Layout.fillWidth: true
                            }

                            StyledButton {
                                width: 190
                                text: i18.n + qsTr("Configure proxy")
                                enabled: useProxyCheckbox.checked

                                onClicked: {
                                    openProxyDialog(false)
                                }
                            }
                        }

                        StyledCheckbox {
                            id: verboseUploadCheckbox
                            text: i18.n + qsTr("Detailed logging")
                            onCheckedChanged: {
                                settingsModel.verboseUpload = checked
                            }
                            function onResetRequested() {
                                checked = settingsModel.verboseUpload
                            }
                            Component.onCompleted: {
                                checked = settingsModel.verboseUpload
                                uploadTab.resetRequested.connect(verboseUploadCheckbox.onResetRequested)
                            }
                        }

                        Item {
                            Layout.fillHeight: true
                        }
                    }
                }

                Item {
                    id : secTab
                    signal resetRequested()
                    anchors.fill: parent
                    anchors.margins: 20
                    property bool useStatistics: settingsModel.userStatistics

                    Connections {
                        target: settingsModel
                        onSettingsReset: { resetRequested() }
                    }

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 20
                        anchors.rightMargin: 20
                        anchors.topMargin: 20
                        anchors.bottomMargin: 10

                        RowLayout {
                            StyledCheckbox {
                                id: masterPasswordCheckbox
                                text: i18.n + qsTr("Use Master password")
                                onClicked: {
                                    if (checked) {
                                        if (!settingsModel.mustUseMasterPassword) {
                                            var firstTime = true;
                                            openMasterPasswordDialog(firstTime)
                                        }
                                    } else {
                                        masterPasswordOffWarningDialog.open()
                                    }
                                }

                                Component.onCompleted: {
                                    checked = settingsModel.mustUseMasterPassword
                                    secTab.resetRequested.connect(masterPasswordCheckbox.onResetRequested)
                                }

                                Connections {
                                    target: settingsModel
                                    onMustUseMasterPasswordChanged: {
                                        masterPasswordCheckbox.checked = settingsModel.mustUseMasterPassword
                                    }

                                }

                                function onResetRequested() {
                                    checked = settingsModel.mustUseMasterPassword
                                }
                            }

                            Item {
                                Layout.fillWidth: true
                            }

                            StyledButton {
                                width: 190
                                text: i18.n + qsTr("Change Master password")
                                enabled: masterPasswordCheckbox.checked

                                onClicked: {
                                    openMasterPasswordDialog(false)
                                }
                            }
                        }

                        RowLayout {
                            Item {
                                Layout.fillWidth: true
                            }

                            StyledButton {
                                width: 190
                                text: i18.n + qsTr("Reset Master password")
                                enabled: masterPasswordCheckbox.checked

                                onClicked: {
                                    resetMPDialog.open()
                                }
                            }
                        }

                        Item {
                            height: 20
                        }

                        Rectangle {
                            height: 1
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.leftMargin: 10
                            anchors.rightMargin: 10
                            color: uiColors.panelSelectedColor
                        }

                        Item {
                            height: 20
                        }

                        Flow {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            spacing: 10
                            property real itemHeight: 40
                            property real itemWidth: 250

                            Item {
                                width: parent.itemWidth
                                height: parent.itemHeight

                                StyledCheckbox {
                                    anchors.left: parent.left
                                    anchors.leftMargin: 10
                                    anchors.verticalCenter: parent.verticalCenter
                                    id: saveSessionCheckbox
                                    text: i18.n + qsTr("Restore last session on startup")
                                    onCheckedChanged: {
                                        settingsModel.saveSession = checked
                                    }
                                    function onResetRequested() {
                                        checked = settingsModel.saveSession
                                    }

                                    Component.onCompleted: {
                                        checked = settingsModel.saveSession
                                        behaviorTab.resetRequested.connect(saveSessionCheckbox.onResetRequested)
                                    }
                                }
                            }

                            Item {
                                width: parent.itemWidth
                                height: parent.itemHeight

                                StyledCheckbox {
                                    anchors.left: parent.left
                                    anchors.leftMargin: 10
                                    anchors.verticalCenter: parent.verticalCenter
                                    id: useConfirmationDialogsCheckbox
                                    text: i18.n + qsTr("Use confirmation dialogs")
                                    onCheckedChanged: {
                                        settingsModel.mustUseConfirmations = checked
                                    }
                                    function onResetRequested() {
                                        checked = settingsModel.mustUseConfirmations
                                    }

                                    Component.onCompleted: {
                                        checked = settingsModel.mustUseConfirmations
                                        behaviorTab.resetRequested.connect(useConfirmationDialogsCheckbox.onResetRequested)
                                    }
                                }
                            }

                            Item {
                                width: parent.itemWidth
                                height: parent.itemHeight

                                StyledCheckbox {
                                    anchors.left: parent.left
                                    anchors.leftMargin: 10
                                    anchors.verticalCenter: parent.verticalCenter
                                    id: checkForUpdatesCheckbox
                                    text: i18.n + qsTr("Check for updates")
                                    onCheckedChanged: {
                                        settingsModel.checkForUpdates = checked
                                    }
                                    function onResetRequested() {
                                        checked = settingsModel.checkForUpdates
                                    }

                                    Component.onCompleted: {
                                        checked = settingsModel.checkForUpdates
                                        behaviorTab.resetRequested.connect(checkForUpdatesCheckbox.onResetRequested)
                                    }
                                }
                            }

                            Item {
                                width: parent.itemWidth
                                height: parent.itemHeight

                                StyledCheckbox {
                                    anchors.left: parent.left
                                    anchors.leftMargin: 10
                                    anchors.verticalCenter: parent.verticalCenter
                                    id: saveBackupsCheckbox
                                    text: i18.n + qsTr("Save backups for artworks")
                                    onCheckedChanged: {
                                        settingsModel.saveBackups = checked
                                    }
                                    function onResetRequested() {
                                        checked = settingsModel.saveBackups
                                    }

                                    Component.onCompleted: {
                                        checked = settingsModel.saveBackups
                                        behaviorTab.resetRequested.connect(saveBackupsCheckbox.onResetRequested)
                                    }
                                }
                            }

                            Item {
                                width: parent.itemWidth
                                height: parent.itemHeight

                                StyledCheckbox {
                                    anchors.left: parent.left
                                    anchors.leftMargin: 10
                                    anchors.verticalCenter: parent.verticalCenter
                                    id: userStatisticCheckBox
                                    text: i18.n + qsTr("Health report")

                                    function onResetRequested() {
                                        checked = settingsModel.userStatistics
                                    }

                                    Component.onCompleted: {
                                        checked = settingsModel.userStatistics
                                        secTab.resetRequested.connect(userStatisticCheckBox.onResetRequested)
                                    }

                                    onCheckedChanged: {
                                        secTab.useStatistics = checked
                                    }
                                }
                            }
                        }

                        Item {
                            Layout.fillHeight: true
                        }
                    }
                }
            }
        }

        Rectangle {
            id: footer
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: 50
            color: uiColors.defaultDarkColor

            RowLayout {
                id: footerRow
                anchors.fill: parent
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                spacing: 20

                StyledLink {
                    normalLinkColor: uiColors.labelInactiveForeground
                    text: i18.n + qsTr("Reset to defaults")
                    onClicked: {
                        resetSettingsDialog.open()

                        if (typeof useConfirmationDialogsCheckbox !== "undefined") {
                            useConfirmationDialogsCheckbox.checked = settingsModel.mustUseConfirmations
                        }

                        if (typeof masterPasswordCheckbox !== "undefined") {
                            masterPasswordCheckbox.checked = settingsModel.mustUseMasterPassword;
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                }

                StyledButton {
                    text: i18.n + qsTr("Save and close")
                    width: 150
                    onClicked: {
                        saveSettings()
                        closeSettings()
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        //exifToolText.forceActiveFocus()
    }
}


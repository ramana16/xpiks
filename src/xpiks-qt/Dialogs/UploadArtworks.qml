/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
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
import xpiks 1.0
import "../Constants"
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"

Item {
    id: uploadArtworksComponent
    anchors.fill: parent

    // if MasterPassword wasn't entered do not show passwords
    property bool emptyPasswords: false
    property bool skipUploadItems: false
    property variant componentParent
    property bool uploadEnabled: (artworkRepository.artworksSourcesCount > 0) && (filteredArtItemsModel.selectedArtworksCount > 0)
    property var ftpListAC: helpersWrapper.getFtpACList()
    property var artworkUploader: helpersWrapper.getArtworkUploader()
    property var uploadWatcher: artworkUploader.getUploadWatcher()
    property var uploadInfos: helpersWrapper.getUploadInfos();

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    Component.onCompleted: {
        ftpListAC.searchTerm = ''
    }

    Connections {
        target: helpersWrapper
        onGlobalBeforeDestruction: {
            console.debug("UI:UploadArtworks # globalBeforeDestruction")
            closePopup()
        }
    }

    function closePopup() {
        secretsManager.purgeMasterPassword()
        warningsModel.resetShowSelected()
        uploadInfos.finalizeAccounts()
        settingsModel.saveUploadHosts()
        uploadArtworksComponent.destroy()
        uploadWatcher.resetModel()
    }

    function mainAction() {
        artworkUploader.resetModel()
        artworkUploader.uploadArtworks()
    }

    function startUpload() {
        if (artworkUploader.needCreateArchives()) {
            var callbackObject = {
                afterZipped: function() {
                    mainAction();
                }
            }

            filteredArtItemsModel.setSelectedForZipping()
            Common.launchDialog("Dialogs/ZipArtworksDialog.qml",
                                uploadArtworksComponent.componentParent,
                                {
                                    immediateProcessing: true,
                                    callbackObject: callbackObject
                                });
        } else {
            mainAction();
        }
    }

    PropertyAnimation { target: uploadArtworksComponent; property: "opacity";
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
            uploadInfos.removeItem(itemIndex)
            if (uploadInfos.infosCount == 0) {
                if (typeof titleText !== "undefined") {
                    titleText.text = ""
                    ftpHost.text = ""
                    ftpUsername.text = ""
                    ftpPassword.text = ""
                }
            }
        }
    }

    MessageDialog {
        id: noPasswordDialog
        property string agenciesList
        title: i18.n + qsTr("Warning")
        text: i18.n + qsTr("Some agencies (%1) miss FTP credentials. Start upload anyway?").arg(agenciesList)
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            startUpload()
        }
    }

    MessageDialog {
        id: selectHostsMessageBox
        title: i18.n + qsTr("Warning")
        text: i18.n + qsTr("Please, select some hosts first")
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
                var tmp = mapToItem(uploadArtworksComponent, mouse.x, mouse.y);
                old_x = tmp.x;
                old_y = tmp.y;

                var dialogPoint = mapToItem(dialogWindow, mouse.x, mouse.y);
                if (!Common.isInComponent(dialogPoint, dialogWindow)) {
                    if (!artworkUploader.inProgress) {
                        closePopup()
                    }
                }
            }

            onPositionChanged: {
                var old_xy = Common.movePopupInsideComponent(uploadArtworksComponent, dialogWindow, mouse, old_x, old_y);
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
            color: Colors.defaultControlColor
            cornerRadius: glowRadius
        }

        // This rectangle is the actual popup
        Rectangle {
            id: dialogWindow
            width: 630
            height: Qt.platform.os == "windows" ? 470 : (Qt.platform.os == "linux" ? 475 : 460)
            color: Colors.popupBackgroundColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            ColumnLayout {
                spacing: 15
                anchors.fill: parent
                anchors.margins: 20

                RowLayout {
                    Layout.fillWidth: true

                    StyledText {
                        text: i18.n + qsTr("Upload artworks")
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledText {
                        id: textItemsAvailable
                        enabled: !skipUploadItems
                        visible: !skipUploadItems
                        text: i18.n + getOriginalText()

                        function getOriginalText() {
                            return artworkUploader.itemsCount === 1 ? qsTr("1 artwork selected") : qsTr("%1 artworks selected").arg(artworkUploader.itemsCount)
                        }

                        Connections {
                            target: artworkUploader
                            onItemsNumberChanged: {
                                textItemsAvailable.originalText = artworkUploader.itemsCount === 1 ? qsTr("1 artwork selected") : qsTr("%1 artworks selected").arg(artworkUploader.itemsCount)
                                textItemsAvailable.text=i18.n + originalText
                            }
                        }
                    }
                }

                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    enabled: !artworkUploader.inProgress

                    ColumnLayout {
                        anchors.left: parent.left
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        id: uploadInfosStack
                        width: 260
                        height: parent.height
                        spacing: 0

                        Rectangle {
                            height: 5
                            Layout.fillWidth: true
                            color: Colors.defaultControlColor
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.minimumWidth: 250
                            Layout.preferredWidth: 250
                            Layout.maximumWidth: 300

                            color: Colors.defaultControlColor

                            StyledScrollView {
                                anchors.fill: parent
                                anchors.margins: { left: 5; top: 5; right: 5; bottom: 5 }

                                ListView {
                                    id: uploadHostsListView
                                    model: uploadInfos
                                    boundsBehavior: Flickable.StopAtBounds

                                    spacing: 10

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
                                        property bool isSelected: ListView.isCurrentItem
                                        color: isSelected ? Colors.selectedArtworkBackground : Colors.defaultDarkColor
                                        width: parent.width - 10
                                        anchors.left: parent.left
                                        anchors.leftMargin: 5
                                        height: 31
                                        Layout.minimumWidth: 250

                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                if (uploadHostsListView.currentIndex != sourceWrapper.delegateIndex) {
                                                    // "if" is needed for tabControl
                                                    if (typeof credentialsStatus !== "undefined") {
                                                        credentialsStatus.enabled = false
                                                        credentialsStatus.isGreen = false
                                                    }

                                                    uploadHostsListView.currentIndex = sourceWrapper.delegateIndex
                                                    uploadInfos.updateProperties(sourceWrapper.delegateIndex)
                                                }
                                            }

                                            onDoubleClicked: {
                                                myData.editisselected = !myData.isselected
                                                itemCheckedCheckbox.checked = isselected
                                            }
                                        }

                                        RowLayout {
                                            spacing: 10
                                            anchors.fill: parent

                                            Item {
                                                width: 1
                                            }

                                            StyledCheckbox {
                                                id: itemCheckedCheckbox
                                                onClicked: editisselected = checked
                                                Component.onCompleted: itemCheckedCheckbox.checked = isselected
                                            }

                                            StyledText {
                                                id: infoTitle
                                                Layout.fillWidth: true
                                                anchors.verticalCenter: parent.verticalCenter
                                                height: 31
                                                text: title
                                                elide: Text.ElideMiddle
                                                font.bold: sourceWrapper.isSelected
                                            }

                                            StyledText {
                                                id: percentText
                                                text: percent + '%'
                                                visible: artworkUploader.inProgress && isselected
                                                color: Colors.artworkActiveColor
                                                font.bold: true
                                            }

                                            CloseIcon {
                                                width: 14
                                                height: 14
                                                anchors.verticalCenterOffset: 1
                                                isActive: false
                                                disabledColor: Colors.closeIconInactiveColor

                                                onItemClicked: {
                                                    confirmRemoveItemDialog.itemIndex = sourceWrapper.delegateIndex
                                                    confirmRemoveItemDialog.open()
                                                }
                                            }

                                            Item {
                                                id: placeholder2
                                                width: 1
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        Rectangle {
                            color: Colors.defaultControlColor
                            height: 40
                            anchors.left: parent.left
                            anchors.right: parent.right

                            StyledAddHostButton {
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.leftMargin: 10
                                anchors.rightMargin: 10
                                anchors.verticalCenter: parent.verticalCenter
                                text: i18.n + qsTr("Add FTP host")
                                onClicked: {
                                    uploadInfos.addItem()
                                    uploadHostsListView.currentIndex = uploadHostsListView.count - 1
                                    tabView.getTab(0).titleFocusRequested()
                                }
                            }
                        }
                    }

                    Item {
                        anchors.top: parent.top
                        anchors.leftMargin: 10
                        anchors.left: uploadInfosStack.right
                        anchors.right: parent.right
                        width: 290
                        height: (Qt.platform.os == "osx") ? parent.height : parent.height + 10

                        StyledTabView {
                            id: tabView
                            anchors.fill: parent
                            anchors.leftMargin: 10
                            anchors.topMargin: 1
                            enabled: uploadInfos.infosCount > 0

                            Tab {
                                id: generalTab
                                title: i18.n + qsTr("General")
                                property var autoCompleteBox
                                signal titleFocusRequested();

                                function onAutoCompleteClose() {
                                    autoCompleteBox = undefined
                                }

                                function showStockCompletion(textField) {
                                    if (typeof generalTab.autoCompleteBox !== "undefined") {
                                        // update completion
                                        ftpListAC.selectedIndex = -1
                                        return
                                    }

                                    var directParent = generalTab;
                                    var tmp = textField.parent.mapToItem(directParent, 0, textField.parent.height + 1)
                                    var isBelow = true

                                    var options = {
                                        model: ftpListAC,
                                        isBelowEdit: isBelow,
                                        autoCompleteSource: ftpListAC,
                                        "anchors.left": directParent.left,
                                        "anchors.leftMargin": tmp.x,
                                        "anchors.top": directParent.top,
                                        "anchors.topMargin": tmp.y
                                    }

                                    var component = Qt.createComponent("../Components/CompletionBox.qml");
                                    if (component.status !== Component.Ready) {
                                        console.debug("Component Error: " + component.errorString());
                                    } else {
                                        var instance = component.createObject(directParent, options);

                                        instance.boxDestruction.connect(generalTab.onAutoCompleteClose)
                                        instance.itemSelected.connect(textField.acceptCompletion)
                                        generalTab.autoCompleteBox = instance

                                        ftpListAC.isActive = true
                                        instance.openPopup()
                                    }
                                }

                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.topMargin: 15
                                    spacing: 4

                                    StyledText {
                                        text: i18.n + qsTr("Title:")
                                    }

                                    Rectangle {
                                        id: titleWrapper
                                        border.width: titleText.activeFocus ? 1 : 0
                                        border.color: Colors.artworkActiveColor
                                        Layout.fillWidth: true
                                        color: enabled ? Colors.inputBackgroundColor : Colors.inputInactiveBackground
                                        height: 30

                                        function onTitleFocusRequested() {
                                            titleText.forceActiveFocus()
                                            titleText.cursorPosition = titleText.text.length
                                        }

                                        Component.onCompleted: {
                                            generalTab.titleFocusRequested.connect(titleWrapper.onTitleFocusRequested)
                                        }

                                        StyledTextInput {
                                            id: titleText
                                            height: 30
                                            anchors.left: parent.left
                                            anchors.right: parent.right
                                            anchors.rightMargin: 5
                                            text: uploadHostsListView.currentItem ? uploadHostsListView.currentItem.myData.title : ""
                                            anchors.leftMargin: 5

                                            property bool autoCompleteActive: ftpListAC.isActive

                                            onTextChanged: {
                                                if (uploadHostsListView.currentItem) {
                                                    uploadHostsListView.currentItem.myData.edittitle = text
                                                }
                                            }
                                            KeyNavigation.tab: ftpHost
                                            onEditingFinished: {
                                                if (text.length == 0) {
                                                    uploadHostsListView.currentItem.myData.edittitle = qsTr("Untitled")
                                                }
                                            }

                                            onActiveFocusChanged: {
                                                if (!activeFocus) {
                                                    ftpListAC.cancelCompletion()
                                                }
                                            }

                                            function acceptCompletion(completion, expandPreset) {
                                                if (uploadHostsListView.currentItem) {
                                                    uploadHostsListView.currentItem.myData.edittitle = completion
                                                    uploadHostsListView.currentItem.myData.edithost = artworkUploader.getFtpAddress(completion)
                                                }
                                            }

                                            validator: RegExpValidator {
                                                // copy paste in keys.onpressed Paste
                                                regExp: /[a-zA-Z0-9 _-]*$/
                                            }

                                            Keys.onPressed: {
                                                if (event.matches(StandardKey.Paste)) {
                                                    var clipboardText = clipboard.getText();
                                                    // same regexp as in validator
                                                    var sanitizedText = clipboardText.replace(/[^a-zA-Z0-9 _-]/g, '');
                                                    titleText.paste(sanitizedText)
                                                    event.accepted = true
                                                } else if (autoCompleteActive && (event.key === Qt.Key_Return)) {
                                                    ftpListAC.acceptSelected()
                                                    if (ftpListAC.hasSelectedCompletion()) {
                                                        event.accepted = true
                                                    }
                                                } else if (autoCompleteActive && (event.key === Qt.Key_Up)) {
                                                    ftpListAC.moveSelectionUp()
                                                    event.accepted = true
                                                } else if (autoCompleteActive && (event.key === Qt.Key_Down)) {
                                                    ftpListAC.moveSelectionDown()
                                                    event.accepted = true;
                                                } else if (autoCompleteActive && (event.key === Qt.Key_Escape)) {
                                                    ftpListAC.cancelCompletion()
                                                    event.accepted = true;
                                                }
                                            }

                                            Keys.onReleased: {
                                                if (event.key === Qt.Key_Escape) {
                                                    return
                                                }

                                                if ((Qt.Key_A <= event.key && event.key <= Qt.Key_Z) ||
                                                        (Qt.Key_0 <= event.key && event.key <= Qt.Key_9) ||
                                                        (text.length === 0 && !ftpListAC.isActive)) {
                                                    generalTab.showStockCompletion(titleText)
                                                    ftpListAC.searchTerm = text
                                                } else if (event.key === Qt.Key_Backspace) {
                                                    ftpListAC.searchTerm = text
                                                }
                                            }
                                        }
                                    }

                                    Item {
                                        height: 5
                                    }

                                    StyledText {
                                        text: i18.n + qsTr("Host:")
                                    }

                                    Rectangle {
                                        border.width: ftpHost.activeFocus ? 1 : 0
                                        border.color: Colors.artworkActiveColor
                                        Layout.fillWidth: true
                                        color: enabled ? Colors.inputBackgroundColor : Colors.inputInactiveBackground
                                        height: 30

                                        StyledTextInput {
                                            id: ftpHost
                                            height: 30
                                            anchors.left: parent.left
                                            anchors.leftMargin: 5
                                            anchors.right: parent.right
                                            anchors.rightMargin: 5
                                            text: uploadHostsListView.currentItem ? uploadHostsListView.currentItem.myData.host : ""
                                            onTextChanged: {
                                                if (uploadHostsListView.currentItem) {
                                                    uploadHostsListView.currentItem.myData.edithost = text
                                                }
                                                credentialsStatus.enabled = false
                                            }
                                            KeyNavigation.tab: ftpUsername
                                            KeyNavigation.backtab: titleText
                                        }
                                    }

                                    Item {
                                        height: 6
                                    }

                                    StyledText {
                                        text: i18.n + qsTr("Username:")
                                    }

                                    Rectangle {
                                        border.width: ftpUsername.activeFocus ? 1 : 0
                                        border.color: Colors.artworkActiveColor
                                        Layout.fillWidth: true
                                        color: enabled ? Colors.inputBackgroundColor : Colors.inputInactiveBackground
                                        height: 30

                                        StyledTextInput {
                                            id: ftpUsername
                                            height: 30
                                            anchors.left: parent.left
                                            anchors.leftMargin: 5
                                            anchors.right: parent.right
                                            anchors.rightMargin: 5
                                            text: uploadHostsListView.currentItem ? uploadHostsListView.currentItem.myData.username : ""
                                            onTextChanged: {
                                                if (uploadHostsListView.currentItem) {
                                                    uploadHostsListView.currentItem.myData.editusername = text
                                                }
                                            }
                                            KeyNavigation.tab: ftpPassword
                                            KeyNavigation.backtab: ftpHost
                                        }
                                    }

                                    Item {
                                        height: 5
                                    }

                                    StyledText {
                                        text: i18.n + qsTr("Password:")
                                    }

                                    Rectangle {
                                        border.width: ftpPassword.activeFocus ? 1 : 0
                                        border.color: Colors.artworkActiveColor
                                        Layout.fillWidth: true
                                        height: 30
                                        color: enabled ? Colors.inputBackgroundColor : Colors.inputInactiveBackground

                                        StyledTextInput {
                                            id: ftpPassword
                                            height: 30
                                            anchors.left: parent.left
                                            anchors.leftMargin: 5
                                            anchors.right: parent.right
                                            anchors.rightMargin: 5
                                            echoMode: showPasswordCheckBox.checked ? TextInput.Normal : TextInput.Password
                                            text: uploadHostsListView.currentItem ? uploadHostsListView.currentItem.myData.password : ""
                                            onTextChanged: {
                                                if (uploadHostsListView.currentItem) {
                                                    uploadHostsListView.currentItem.myData.editpassword = text
                                                }
                                                credentialsStatus.enabled = false
                                            }
                                            KeyNavigation.backtab: ftpUsername
                                        }
                                    }

                                    Item {
                                        height: 4
                                    }

                                    RowLayout {
                                        height: 25
                                        Layout.fillWidth: true
                                        spacing: 0

                                        StyledCheckbox {
                                            id: showPasswordCheckBox
                                            text: i18.n + qsTr("Show password")
                                            checked: false
                                        }

                                        Item {
                                            Layout.fillWidth: true
                                        }

                                        CheckedComponent {
                                            width: 18
                                            height: 18
                                            id: credentialsStatus
                                            enabled: false
                                            isGreen: false
                                        }

                                        Item {
                                            width: 5
                                        }

                                        StyledButton {
                                            id: testButton
                                            text: i18.n + qsTr("Test connection")
                                            height: 24
                                            width: 130
                                            onClicked: {
                                                testButton.enabled = false
                                                credentialsStatus.enabled = false
                                                var disablePassiveMode = uploadHostsListView.currentItem.myData.disablepassivemode
                                                var disableEPSV = uploadHostsListView.currentItem.myData.disableEPSV
                                                artworkUploader.checkCredentials(ftpHost.text, ftpUsername.text, ftpPassword.text, disablePassiveMode, disableEPSV)
                                            }

                                            Connections {
                                                target: artworkUploader
                                                onCredentialsChecked: {
                                                    var currHost = ftpHost.text
                                                    if (url.indexOf(currHost) > -1) {
                                                        credentialsStatus.enabled = true
                                                        credentialsStatus.isGreen = result
                                                    }

                                                    testButton.enabled = true
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
                                title: i18.n + qsTr("Advanced")

                                ColumnLayout {
                                    anchors.fill: parent
                                    anchors.topMargin: 15
                                    spacing: 10

                                    StyledCheckbox {
                                        id: zipBeforeUploadCheckBox
                                        text: i18.n + qsTr("Zip vector with preview")
                                        Component.onCompleted: checked = uploadHostsListView.currentItem ? uploadHostsListView.currentItem.myData.zipbeforeupload : false

                                        onClicked: {
                                            if (uploadHostsListView.currentItem) {
                                                uploadHostsListView.currentItem.myData.editzipbeforeupload = checked
                                            }
                                        }

                                        Connections {
                                            target: uploadInfos
                                            onDataChanged: {
                                                zipBeforeUploadCheckBox.checked = uploadHostsListView.currentItem ? uploadHostsListView.currentItem.myData.zipbeforeupload : false
                                            }
                                        }
                                    }

                                    StyledCheckbox {
                                        id: ftpPassiveModeCheckBox
                                        text: i18.n + qsTr("Disable FTP passive mode")
                                        Component.onCompleted: checked = uploadHostsListView.currentItem ? uploadHostsListView.currentItem.myData.disablepassivemode : false

                                        onClicked: {
                                            if (uploadHostsListView.currentItem) {
                                                uploadHostsListView.currentItem.myData.editdisablepassivemode = checked
                                            }
                                        }

                                        Connections {
                                            target: uploadInfos
                                            onDataChanged: {
                                                ftpPassiveModeCheckBox.checked = uploadHostsListView.currentItem ? uploadHostsListView.currentItem.myData.disablepassivemode : false
                                            }
                                        }
                                    }

                                    StyledCheckbox {
                                        id: epsvCheckBox
                                        text: i18.n + qsTr("Disable extended passive mode")
                                        Component.onCompleted: checked = uploadHostsListView.currentItem ? uploadHostsListView.currentItem.myData.disableEPSV : false

                                        onClicked: {
                                            if (uploadHostsListView.currentItem) {
                                                uploadHostsListView.currentItem.myData.editdisableEPSV = checked
                                            }
                                        }

                                        Connections {
                                            target: uploadInfos
                                            onDataChanged: {
                                                epsvCheckBox.checked = uploadHostsListView.currentItem ? uploadHostsListView.currentItem.myData.disableEPSV : false
                                            }
                                        }
                                    }

                                    Item {
                                        Layout.fillHeight: true
                                    }
                                }
                            }
                        }

                        Rectangle {
                            id: overlayRectangle
                            anchors.fill: parent
                            color: Colors.selectedArtworkBackground
                            opacity: 0.6
                            visible: (uploadInfos.infosCount === 0) || artworkUploader.inProgress
                        }
                    }
                }

                SimpleProgressBar {
                    id: progress
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width
                    height: 20
                    color: artworkUploader.isError ? Colors.destructiveColor : Colors.artworkActiveColor
                    value: artworkUploader.percent
                }

                RowLayout {
                    height: 24
                    spacing: 20

                    StyledText {
                        visible: !skipUploadItems && (!artworkUploader.inProgress) && (uploadWatcher.failedImagesCount === 0)
                        enabled: uploadArtworksComponent.uploadEnabled && !skipUploadItems
                        text: i18.n + getOriginalText()
                        color: uploadWarmingsMA.pressed ? Colors.linkClickedColor : warningsModel.warningsCount > 0 ? Colors.artworkModifiedColor : Colors.labelActiveForeground

                        function getOriginalText() {
                            return warningsModel.warningsCount == 1 ? qsTr("1 warning") : qsTr("%1 warnings").arg(warningsModel.warningsCount)
                        }

                        MouseArea {
                            id: uploadWarmingsMA
                            anchors.fill: parent
                            cursorShape: warningsModel.warningsCount > 0 ? Qt.PointingHandCursor : Qt.ArrowCursor
                            enabled: warningsModel.warningsCount > 0
                            onClicked: {
                                if (warningsModel.warningsCount > 0) {
                                    Common.launchDialog("Dialogs/WarningsDialog.qml",
                                                        uploadArtworksComponent.componentParent,
                                                        {
                                                            componentParent: uploadArtworksComponent.componentParent,
                                                            isRestricted: true
                                                        });
                                }
                            }
                        }
                    }

                    StyledText {
                        id: failedArtworksStatus
                        visible: !skipUploadItems && (uploadWatcher.failedImagesCount > 0)
                        enabled: uploadArtworksComponent.uploadEnabled && !skipUploadItems && (uploadWatcher.failedImagesCount > 0)
                        text: i18.n + getOriginalText()
                        color: showFailedArtworksMA.pressed ? Colors.linkClickedColor : Colors.artworkModifiedColor

                        function getOriginalText() {
                            return uploadWatcher.failedImagesCount === 1 ?
                                        qsTr("1 failed upload") :
                                        qsTr("%1 failed uploads").arg(uploadWatcher.failedImagesCount)
                        }

                        MouseArea {
                            id: showFailedArtworksMA
                            anchors.fill: parent
                            cursorShape: uploadWatcher.failedImagesCount > 0 ? Qt.PointingHandCursor : Qt.ArrowCursor
                            enabled: uploadWatcher.failedImagesCount > 0
                            onClicked: {
                                Common.launchDialog("Dialogs/FailedUploadArtworks.qml",
                                                    uploadArtworksComponent.componentParent,
                                                    {})
                            }
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        id: uploadButton
                        enabled: uploadArtworksComponent.uploadEnabled && !skipUploadItems
                        text: i18.n + (artworkUploader.inProgress ? qsTr("Stop") : qsTr("Start Upload"))
                        width: 130
                        onClicked: {
                            if (!artworkUploader.inProgress) {
                                if (uploadInfos.getSelectedInfosCount() === 0) {
                                    selectHostsMessageBox.open()
                                } else {
                                    var agencies = uploadInfos.getAgenciesWithMissingDetails();
                                    if (agencies.length !== 0) {
                                        noPasswordDialog.agenciesList = agencies
                                        noPasswordDialog.open()
                                    } else {
                                        startUpload()
                                    }
                                }
                            } else {
                                console.log("UI:UploadArtworks # About to cancel upload...")
                                enabled = false
                                artworkUploader.cancelOperation()
                            }
                        }

                        Connections {
                            target: artworkUploader
                            onStartedProcessing: {
                                helpersWrapper.turnTaskbarProgressOn()
                            }
                            onPercentChanged: {
                                helpersWrapper.setTaskbarProgress(artworkUploader.percent)
                            }
                            onFinishedProcessing: {
                                uploadButton.enabled = true
                                helpersWrapper.turnTaskbarProgressOff()
                            }
                            onRequestCloseWindow: {
                                closePopup();
                            }
                        }
                    }

                    StyledButton {
                        text: i18.n + qsTr("Close")
                        width: 120
                        enabled: !artworkUploader.inProgress
                        onClicked: {
                            filteredArtItemsModel.updateSelectedArtworks()
                            closePopup()
                        }
                    }
                }
            }
        }
    }
}

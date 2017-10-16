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
    id: csvExportComponent
    anchors.fill: parent
    property variant columnsModel: csvExportModel.getColumnsModel()
    property variant propertiesModel: columnsModel.getPropertiesList()

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    function closePopup() {
        csvExportModel.requestSave()
        csvExportComponent.destroy()
    }

    signal dismissComboboxes();

    Component.onCompleted: focus = true
    Keys.onEscapePressed: closePopup()

    PropertyAnimation { target: csvExportComponent; property: "opacity";
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
            csvExportModel.removePlanAt(itemIndex)

            if (exportPlanModelsListView.count == 0) {
                addExportPlanButton.forceActiveFocus()
            }
        }
    }

    Menu {
        id: dotsClickMenu
        property int columnIndex: 0

        MenuItem {
            text: i18.n + qsTr("Remove")
            onTriggered: {
                columnsModel.removeColumn(dotsClickMenu.columnIndex)
                csvExportModel.requestSave()
            }
        }

        MenuItem {
            text: i18.n + qsTr("Add column above")
            onTriggered: {
                columnsModel.addColumnAbove(dotsClickMenu.columnIndex)
                csvExportModel.requestSave()
            }
        }

        MenuItem {
            text: i18.n + qsTr("Move up")
            onTriggered: {
                columnsModel.moveColumnUp(dotsClickMenu.columnIndex)
                csvExportModel.requestSave()
            }
        }

        MenuItem {
            text: i18.n + qsTr("Move down")
            onTriggered: {
                columnsModel.moveColumnDown(dotsClickMenu.columnIndex)
                csvExportModel.requestSave()
            }
        }
    }

    FileDialog {
        id: exportDirDialog
        title: "Please choose export location"
        selectExisting: true
        selectMultiple: false
        selectFolder: true
        folder: shortcuts.documents
        nameFilters: [ "All files (*)" ]

        onAccepted: {
            console.log("CsvExportDialog # You chose: " + exportDirDialog.folder)
            var path = exportDirDialog.folder.toString().replace(/^(file:\/{2})/,"");
            csvExportModel.outputDirectory = decodeURIComponent(path);
            csvExportModel.startExport()
        }

        onRejected: {
            console.log("CsvExportDialog # File dialog canceled")
        }
    }

    MessageDialog {
        id: selectPlansMessageBox
        title: i18.n + qsTr("Warning")
        text: i18.n + qsTr("Please, select some export plans first")
    }

    MessageDialog {
        id: exportFinishedMessageBox
        title: i18.n + qsTr("Information")
        text: i18.n + qsTr("CSV export finished")
    }

    Connections {
        target: csvExportModel
        onExportFinished: {
            exportFinishedMessageBox.open()
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
            hoverEnabled: true

            property real old_x : 0
            property real old_y : 0

            onPressed: {
                var tmp = mapToItem(csvExportComponent, mouse.x, mouse.y);
                old_x = tmp.x;
                old_y = tmp.y;
            }

            onPositionChanged: {
                if (!backgroundMA.pressed) { return }

                var old_xy = Common.movePopupInsideComponent(csvExportComponent, dialogWindow, mouse, old_x, old_y);
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
            width: 700
            height: 520 // csvExportComponent.height - 100
            color: uiColors.popupBackgroundColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            Rectangle {
                id: leftPanel
                color: uiColors.defaultControlColor
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: footer.top
                width: 250
                enabled: !csvExportModel.isExporting

                ListView {
                    id: exportPlanModelsListView
                    model: csvExportModel
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.bottom: leftFooter.top
                    anchors.topMargin: 10
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
                        csvExportModel.setCurrentItem(exportPlanModelsListView.currentIndex)
                    }

                    delegate: Rectangle {
                        id: sourceWrapper
                        property variant myData: model
                        property int delegateIndex: index
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
                                if (exportPlanModelsListView.currentIndex != sourceWrapper.delegateIndex) {
                                    exportPlanModelsListView.currentIndex = sourceWrapper.delegateIndex
                                    //uploadInfos.updateProperties(sourceWrapper.delegateIndex)
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
                                width: 10
                            }

                            StyledCheckbox {
                                id: itemCheckedCheckbox
                                isContrast: !ListView.isCurrentItem
                                onClicked: editisselected = checked
                                Component.onCompleted: itemCheckedCheckbox.checked = isselected
                            }

                            StyledText {
                                id: infoTitle
                                Layout.fillWidth: true
                                anchors.verticalCenter: parent.verticalCenter
                                height: 31
                                text: name
                                elide: Text.ElideMiddle
                                font.bold: isselected
                            }

                            CloseIcon {
                                width: 14
                                height: 14
                                anchors.verticalCenterOffset: 1
                                isActive: false
                                disabledColor: uiColors.closeIconInactiveColor

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
                    flickable: exportPlanModelsListView
                    canShow: !backgroundMA.containsMouse && !rightPanelMA.containsMouse
                }

                Item {
                    id: leftFooter
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    height: 50

                    StyledBlackButton {
                        id: addExportPlanButton
                        text: i18.n + qsTr("Add new", "csv export plan")
                        width: 210
                        height: 30
                        anchors.centerIn: parent
                        onClicked: {
                            csvExportModel.addNewPlan()
                            exportPlanModelsListView.positionViewAtIndex(exportPlanModelsListView.count - 1, ListView.Contain)
                            exportPlanModelsListView.currentIndex = exportPlanModelsListView.count - 1
                        }
                    }
                }
            }

            Item {
                id: rightHeader
                anchors.left: leftPanel.right
                anchors.right: parent.right
                anchors.top: parent.top
                height: 40

                StyledText {
                    anchors.right: parent.right
                    anchors.rightMargin: 20
                    anchors.verticalCenter: parent.verticalCenter
                    text: i18.n + getOriginalText()
                    isActive: false

                    function getOriginalText() {
                        return (csvExportModel.artworksCount == 1) ? qsTr("1 artwork selected") : qsTr("%1 artworks selected").arg(csvExportModel.artworksCount)
                    }
                }
            }

            Item {
                id: rightPanel
                anchors.left: leftPanel.right
                anchors.right: parent.right
                anchors.top: rightHeader.top
                anchors.bottom: footer.top
                enabled: !csvExportModel.isExporting && (exportPlanModelsListView.count > 0)

                MouseArea {
                    id: rightPanelMA
                    anchors.fill: parent
                    hoverEnabled: true

                    onClicked: csvExportComponent.dismissComboboxes()
                }

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 0

                    Item {
                        height: 10
                    }

                    StyledText {
                        text: i18.n + qsTr("Title:")
                        isActive: false
                        anchors.left: parent.left
                        anchors.leftMargin: 33
                    }

                    Item {
                        height: 4
                    }

                    Rectangle {
                        id: titleWrapper
                        border.width: titleText.activeFocus ? 1 : 0
                        border.color: uiColors.artworkActiveColor
                        width: 200
                        color: enabled ? uiColors.inputBackgroundColor : uiColors.inputInactiveBackground
                        height: 24
                        anchors.left: parent.left
                        anchors.leftMargin: 33

                        StyledTextInput {
                            id: titleText
                            height: parent.height
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.rightMargin: 5
                            text: exportPlanModelsListView.currentItem ? exportPlanModelsListView.currentItem.myData.name : ""
                            anchors.leftMargin: 5

                            onTextChanged: {
                                if (exportPlanModelsListView.currentItem) {
                                    exportPlanModelsListView.currentItem.myData.editname = text
                                    csvExportModel.requestSave()
                                }
                            }

                            onEditingFinished: {
                                if (text.length == 0) {
                                    if (presetNamesListView.currentItem) {
                                        presetNamesListView.currentItem.myData.editname = qsTr("Untitled")
                                        csvExportModel.requestSave()
                                    }
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
                        height: 30
                    }

                    Item {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: childrenRect.height

                        StyledText {
                            text: i18.n + qsTr("Column name:")
                            anchors.left: parent.left
                            anchors.leftMargin: 40
                            isActive: false
                        }

                        StyledText {
                            text: i18.n + qsTr("Property:")
                            anchors.left: parent.left
                            anchors.leftMargin: 210
                            isActive: false
                        }
                    }

                    Item {
                        height: 5
                    }

                    Item {
                        id: columnsHost
                        anchors.left: parent.left
                        anchors.right: parent.right
                        Layout.fillHeight: true
                        clip: false

                        ListView {
                            id: columnsListView
                            model: columnsModel
                            clip: columnsScrollbar.visible
                            anchors.fill: parent
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

                            delegate: RowLayout {
                                id: columnWrapper
                                property variant myData: model
                                property int delegateIndex: index
                                anchors.left: parent.left
                                anchors.right: parent.right
                                height: 30
                                spacing: 0
                                z: propertiesCombobox.isOpened ? 100500 : 0

                                DotsButton {
                                    id: columnMenu
                                    onDotsClicked: {
                                        dotsClickMenu.columnIndex = columnWrapper.delegateIndex
                                        dotsClickMenu.popup()
                                    }
                                }

                                Item {
                                    width: 5
                                }

                                Rectangle {
                                    id: columnNameWrapper
                                    border.width: columnNameText.activeFocus ? 1 : 0
                                    border.color: uiColors.artworkActiveColor
                                    width: 170
                                    color: enabled ? uiColors.inputBackgroundColor : uiColors.inputInactiveBackground
                                    height: 24
                                    anchors.verticalCenter: parent.verticalCenter

                                    StyledTextInput {
                                        id: columnNameText
                                        height: parent.height
                                        anchors.left: parent.left
                                        anchors.right: parent.right
                                        anchors.rightMargin: 5
                                        text: column
                                        anchors.leftMargin: 5
                                        onTextChanged: {
                                            model.editcolumn = text
                                            csvExportModel.requestSave()
                                        }

                                        onEditingFinished: {
                                            if (text.length == 0) {
                                                model.editcolumn = qsTr("Untitled")
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
                                                columnNameText.paste(sanitizedText)
                                                event.accepted = true
                                            }
                                        }
                                    }
                                }

                                Item {
                                    width: 20
                                }

                                CustomComboBox {
                                    id: propertiesCombobox
                                    model: propertiesModel
                                    width: 170
                                    height: 24
                                    itemHeight: 28
                                    showColorSign: false
                                    maxCount: columnsScrollbar.visible ? 4 : 5
                                    //z: 100500 - columnWrapper.delegateIndex
                                    isBelow: true
                                    withRelativePosition: true
                                    relativeParent: columnsHost

                                    onComboIndexChanged: {
                                        myData.editproperty = selectedIndex
                                    }

                                    onIsOpenedChanged: {
                                        // turn on/off scrolling of main listview
                                        if (isOpened) {
                                            columnsListView.interactive = false
                                        } else {
                                            columnsListView.interactive = true
                                        }
                                    }

                                    Component.onCompleted: {
                                        selectedIndex = myData.property
                                    }

                                    Connections {
                                        target: csvExportComponent
                                        onDismissComboboxes: {
                                            propertiesCombobox.closePopup()
                                        }
                                    }
                                }

                                Item {
                                    Layout.fillWidth: true
                                }
                            }

                            footer: Item {
                                anchors.left: parent.left
                                anchors.right: parent.right
                                height: 50

                                StyledButton {
                                    text: i18.n + qsTr("Add column")
                                    width: 170
                                    height: 30
                                    anchors.left: parent.left
                                    anchors.leftMargin: 33
                                    anchors.verticalCenter: parent.verticalCenter
                                    opacity: hovered ? 1 : 0.3

                                    onClicked: {
                                        columnsModel.addColumn()
                                        csvExportModel.requestSave()
                                        columnsListView.positionViewAtEnd()
                                    }
                                }
                            }
                        }

                        CustomScrollbar {
                            id: columnsScrollbar
                            anchors.topMargin: 0
                            anchors.bottomMargin: 0
                            anchors.rightMargin: -10
                            flickable: columnsListView
                            //canShow: leftPanelMA.containsMouse
                        }
                    }
                }

                Rectangle {
                    anchors.fill: parent
                    color: uiColors.defaultControlColor
                    opacity: 0.5
                    visible: csvExportModel.isExporting || (exportPlanModelsListView.count == 0)
                }

                LoaderIcon {
                    width: 100
                    height: 100
                    anchors.centerIn: parent
                    running: csvExportModel.isExporting
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
                    height: parent.height
                    spacing: 20

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        text: i18.n + qsTr("Start Export")
                        width: 110
                        isDefault: true
                        anchors.verticalCenter: parent.verticalCenter
                        enabled: !csvExportModel.isExporting && (exportPlanModelsListView.count > 0)
                        onClicked: {
                            if (csvExportModel.getSelectedPlansCount() === 0) {
                                selectPlansMessageBox.open()
                            } else {
                                exportDirDialog.open()
                            }
                        }
                    }

                    StyledButton {
                        text: i18.n + qsTr("Close")
                        width: 110
                        anchors.verticalCenter: parent.verticalCenter
                        enabled: !csvExportModel.isExporting
                        onClicked: {
                            closePopup()
                        }
                    }
                }
            }
        }
    }
}

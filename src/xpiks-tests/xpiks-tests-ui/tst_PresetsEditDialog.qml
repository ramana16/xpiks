import QtQuick 2.0

import QtQuick 2.0
import QtTest 1.1
import "../../xpiks-qt/Dialogs"

Item {
    id: root
    width: 800
    height: 600

    FakeColors {
        id: uiColors
    }

    QtObject {
        id: uiManager
        property real keywordHeight: 10
    }

    QtObject {
        id: applicationWindow
        property bool leftSideCollapsed: false
    }

    QtObject {
        id: settingsModel
        property int keywordSizeScale: 1
    }

    QtObject {
        id: i18
        property string n: ''
    }

    ListModel {
        id: presetsModel

        ListElement { name: "Jane"; keywordscount: 1 }
        ListElement { name: "Harry"; keywordscount: 2 }
        ListElement { name: "Wendy"; keywordscount: 3 }

        function getKeywordsModel(index) { return {} }
    }

    ListModel {
        id: presetsGroups

        ListElement { gname: "Default" }
        ListElement { gname: "Nature" }

        function getGroupModel(index) { return {} }
        function getDefaultGroupModel() { return {} }

        function findGroupIndexById(id) { return 0 }

        property var groupNames: ["Default", "Nature"]
    }

    ListModel {
        id: filteredPresetsModel
        ListElement { name: "Alice" }
        ListElement { name: "Bob" }
    }

    QtObject {
        id: acSource
        property bool isActive: false
        property bool tryExpandPresetFlag: false
        property bool isCancelled: false
        property bool isAccepted: false
        property bool isAnySelected: false

        function moveSelectionUp() {}
        function cancelCompletion() { isCancelled = true }
        function moveSelectionDown() {}
        function acceptSelected(tryExpandPreset) { }
        function hasSelectedCompletion() { return isAnySelected }
    }

    PresetsEditDialog {
        id: presetEditsDialog
        anchors.fill: parent
    }

    TestCase {
        name: "PresetsTests"
        when: windowShown
    }
}

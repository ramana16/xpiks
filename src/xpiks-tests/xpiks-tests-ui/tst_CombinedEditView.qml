import QtQuick 2.0
import QtTest 1.1
import xpiks 1.0
import "../../xpiks-qt/StackViews"
import "UiTestsStubPlugin"

Item {
    id: root
    width: 800
    height: 600

    property string path: ''
    property bool isselected: false

    FakeColors {
        id: uiColors
    }

    QtObject {
        id: settingsModel
        property int keywordSizeScale: 1
    }

    QtObject {
        id: i18
        property string n: ''
    }

    QtObject {
        id: keywordsWrapper
        property bool keywordsModel: false
    }

    QtObject {
        id: warningsModel
        property int minKeywordsCount: 7
    }

    QtObject {
        id: helpersWrapper
        signal globalCloseRequested();
        signal globalBeforeDestruction();

        function sanitizeKeyword(keyword) {
            return keyword;
        }

        function isKeywordValid(keyword) {
            return keyword.length >= 2 || keyword === "$"
        }
    }

    QtObject {
        id: combinedArtworks
        property int selectedArtworksCount: 0
        property int artworksCount: 0
        property string title: ""
        property string description: ""
        property bool changeDescription: true
        property bool changeTitle: true
        property bool changeKeywords: true
        property bool appendKeywords: false
        property int keywordsCount: 0
        property bool hasKeywordsSpellErrors: false
        property bool hasTitleSpellErrors: false
        property bool hasDescriptionSpellErrors: false

        signal requestCloseWindow()
        signal completionsAvailable()

        function initDescriptionHighlighting(){}
        function initTitleHighlighting(){}
        function getKeywordsModel() { return {}; }
        function registerAsCurrentItem() {}
        function getBasicModel() { return {
                hasKeywordsSpellErrors: false,
                hasTitleSpellErrors: false,
                hasDescriptionSpellErrors: false,
                hasDuplicates: false };
        }

        property string thumbpath: ""
        property bool isvideo: false
        property bool hasvectorattached: false
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

    ListModel {
        id: presetsModel

        ListElement { name: "Jane" }
        ListElement { name: "Harry" }
        ListElement { name: "Wendy" }
    }

    ListModel {
        id: presetsGroups

        ListElement { gname: "Default" }
        ListElement { gname: "Nature" }

        function getGroupModel(index) { return {} }
        function getDefaultGroupModel() { return {} }
    }

    ListModel {
        id: filteredPresetsModel
        ListElement { name: "Alice" }
        ListElement { name: "Bob" }
    }

    CombinedEditView {
        id: combinedView
        anchors.fill: parent
    }

    TestCase {
        name: "CombinedEdit"
        when: windowShown
        property var descriptionInput
        property var titleInput
        property var keywordsInput
        property var descriptionCheckBox

        function initTestCase() {
            titleInput = findChild(combinedView, "titleTextInput")
            descriptionInput = findChild(combinedView, "descriptionTextInput")
            keywordsInput = findChild(combinedView, "keywordsInput")
            descriptionCheckBox = findChild(combinedView, "descriptionCheckBox")
        }

        function test_TabTopToBottom() {
            combinedArtworks.changeTitle = true
            descriptionCheckBox.checked = true
            combinedArtworks.changeKeywords = true

            titleInput.forceActiveFocus()
            keyClick(Qt.Key_Tab)
            verify(descriptionInput.activeFocus)
            keyClick(Qt.Key_Tab)
            verify(keywordsInput.isFocused)
        }

        function test_TabOverTitle() {
            combinedArtworks.changeTitle = true
            descriptionCheckBox.checked = false
            combinedArtworks.changeKeywords = true

            titleInput.forceActiveFocus()
            keyClick(Qt.Key_Tab)
            verify(!(descriptionInput.activeFocus))
            verify(keywordsInput.isFocused)
        }

        function test_TabFromKeywords() {
            combinedArtworks.changeTitle = true
            descriptionCheckBox.checked = false
            combinedArtworks.changeKeywords = true

            keywordsInput.activateEdit()
            keyClick(Qt.Key_Backtab)
            verify(!(descriptionInput.activeFocus))
            verify(titleInput.activeFocus)
        }
    }
}

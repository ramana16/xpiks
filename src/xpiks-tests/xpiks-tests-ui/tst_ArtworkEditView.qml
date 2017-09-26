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
        id: uiManager
        property real keywordHeight: 10
    }

    QtObject {
        id: applicationWindow
        property bool leftSideCollapsed: false
    }

    QtObject {
        id: artworkProxy

        property string basename
        property string thumbPath
        property bool isVideo: false
        property string title
        property string description
        property int keywordsCount: 0
        property bool isValid: true

        function getPropertiesMap() { return {} }
        function initTitleHighlighting() {}
        function initDescriptionHighlighting() {}
        function resetModel() {}
        function registerAsCurrentItem() {}

        signal itemBecomeUnavailable()
        signal completionsAvailable();
    }

    QtObject {
        id: filteredArtItemsModel

        property string thumbpath: ""
        property bool isvideo: false
        property bool hasvectorattached: false
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
        id: filteredPresetsModel
        ListElement { name: "Alice" }
        ListElement { name: "Bob" }
    }

    ArtworkEditView {
        id: artworkEditView
    }

    TestCase {
        name: "ArtworkEdit"
        when: windowShown
    }
}

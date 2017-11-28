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
        id: archiver

        property bool isError: false
        property bool inProgress: false
        property int itemsCount: 0
        property int percent: 50

        function resetModel() {}
        function archiveArtworks() {}

        signal finishedProcessing()
        signal requestCloseWindow()
    }

    QtObject {
        id: helpersWrapper

        signal globalBeforeDestruction()

        function getZipArchiver() { return archiver }
    }

    QtObject {
        id: i18
        property string n: ""
    }

    ZipArtworksDialog {
        id: zipDialog
        anchors.fill: parent
    }

    TestCase {
        name: "ZipArchiver"
        when: windowShown
    }
}

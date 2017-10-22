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

    ListModel {
        id: ftpListAC

        property int selectedIndex: 0
        property bool isActive: false
        property string searchTerm: ""
    }

    QtObject {
        id: helpersWrapper

        signal globalBeforeDestruction()

        function getFtpACList() { return ftpListAC }
        function getArtworkUploader() { return artworkUploader }
        function getUploadInfos() { return uploadInfos }
    }

    QtObject {
        id: filteredArtItemsModel

        property int selectedArtworksCount: 4
    }

    QtObject {
        id: artworkUploader

        property bool inProgress: false
        property bool isError: false
        property int percent: 50
        property int itemsCount: 0

        function getUploadWatcher() { return uploadWatcher }

        signal startedProcessing()
        //signal percentChanged()
        signal finishedProcessing()
        signal requestCloseWindow()
        signal credentialsChecked()
        signal itemsNumberChanged()
    }

    QtObject {
        id: artworkRepository

        property int artworksSourcesCount: 2
    }

    ListModel {
        id: uploadInfos

        property int artworksSourcesCount: 1

        signal dataChanged()

        ListElement {
            title: "Shutterstock"
            host: "ftp.shutterstock.com"
            username: "username"
            password: "password"
            isselected: false
            percent: 50
            disablepassivemode: false
            disableEPSV: false
            zipbeforeupload: false
        }
    }

    QtObject {
        id: warningsModel
        property int warningsCount: 7
    }

    QtObject {
        id: uploadWatcher
        property int failedImagesCount: 0
    }

    QtObject {
        id: i18
        property string n: ""
    }

    UploadArtworks {
        id: uploadDialog
    }

    TestCase {
        name: "UploadArtworks"
        when: windowShown
    }
}

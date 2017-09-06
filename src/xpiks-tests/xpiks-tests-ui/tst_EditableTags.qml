import QtQuick 2.0
import QtTest 1.1
import xpiks 1.0
import "../../xpiks-qt/Components"

Item {
    id: root
    width: 800
    height: 600

    FakeColors {
        id: uiColors
    }

    QtObject {
        id: settingsModel
        property int keywordSizeScale: 1
    }

    QtObject {
        id: helpersWrapper

        function sanitizeKeyword(keyword) {
            return keyword;
        }

        function isKeywordValid(keyword) {
            return keyword.length >= 2 || keyword === "$"
        }
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

        function acceptSelected(tryExpandPreset) {
            tryExpandPresetFlag = tryExpandPreset
            isAccepted = true
        }

        function hasSelectedCompletion() { return isAnySelected }

        function clear() {
            isActive = false
            isCancelled = false
            tryExpandPresetFlag = false
            isAccepted = false
            isAnySelected = false
        }
    }

    EditableTags {
        id: editableTags
    }

    SignalSpy {
        id: tagAddedSpy
        target: editableTags
        signalName: "tagAdded"
    }

    SignalSpy {
        id: removeLastSpy
        target: editableTags
        signalName: "removeLast"
    }

    SignalSpy {
        id: tagsPastedSpy
        target: editableTags
        signalName: "tagsPasted"
    }

    SignalSpy {
        id: copyAllSpy
        target: editableTags
        signalName: "copyRequest"
    }

    SignalSpy {
        id: completionRequestedSpy
        target: editableTags
        signalName: "completionRequested"
    }

    ClipboardHelper {
        id: clipboard
    }

    TestCase {
        name: "EditableTagsTests"
        when: windowShown
        property var input

        function initTestCase() {
            input = findChild(editableTags, "nextTagTextInput")
        }

        function test_PressCommaAddsKeyword() {
            tagAddedSpy.clear()
            acSource.clear()
            compare(tagAddedSpy.count, 0)

            input.text = "next_keyword"
            input.forceActiveFocus()
            keyClick(Qt.Key_Comma)

            compare(tagAddedSpy.count, 1)
            compare(acSource.isCancelled, true)
        }

        function test_GetTextAPI() {
            input.text = "keyword"
            compare(editableTags.getEditedText(), "keyword")
        }

        function test_RaiseTagAdded() {
            tagAddedSpy.clear()
            editableTags.raiseAddTag("any text")
            compare(tagAddedSpy.count, 1)
        }

        function test_PressCommaCleansInput() {
            input.text = "next_keyword"
            input.forceActiveFocus()
            keyClick(Qt.Key_Comma)

            compare(input.text.length, 0)
        }

        function test_BackspaceRemovesPrevItem() {
            removeLastSpy.clear()

            input.text = "1"
            // need to set focus
            input.forceActiveFocus()
            input.cursorPosition = 1
            keyClick(Qt.Key_Backspace)

            compare(removeLastSpy.count, 0)
            compare(input.length, 0)

            keyClick(Qt.Key_Backspace)
            compare(removeLastSpy.count, 1)
        }

        function test_SimplePasteNoComma() {
            tagAddedSpy.clear()
            tagsPastedSpy.clear()
            input.text = ""
            clipboard.setText("keyword")

            input.forceActiveFocus()
            keyClick(Qt.Key_V, Qt.ControlModifier)

            compare(input.text, "keyword")
            compare(tagAddedSpy.count, 0)
            compare(tagsPastedSpy.count, 0)
        }

        function test_SimplePasteWithComma() {
            tagAddedSpy.clear()
            tagsPastedSpy.clear()
            input.text = ""
            clipboard.setText("keyword1,keyword2")

            input.forceActiveFocus()
            keyClick(Qt.Key_V, Qt.ControlModifier)

            compare(input.text, "")
            compare(tagAddedSpy.count, 0)
            compare(tagsPastedSpy.count, 1)
            var list = tagsPastedSpy.signalArguments[0][0]
            compare(list, ["keyword1", "keyword2"])
        }

        function test_PasteOneItemWithComma() {
            tagAddedSpy.clear()
            tagsPastedSpy.clear()
            input.text = ""
            clipboard.setText("keyword , ")

            input.forceActiveFocus()
            keyClick(Qt.Key_V, Qt.ControlModifier)

            compare(input.text, "keyword , ")
            compare(tagsPastedSpy.count, 0)
            compare(tagAddedSpy.count, 0)
        }

        function test_PasteOnlyCommas() {
            tagAddedSpy.clear()
            tagsPastedSpy.clear()
            input.text = ""
            clipboard.setText(" , , ")

            input.forceActiveFocus()
            keyClick(Qt.Key_V, Qt.ControlModifier)

            compare(input.text, " , , ")
            compare(tagsPastedSpy.count, 0)
            compare(tagAddedSpy.count, 0)
        }

        function test_SimplePasteWithSemicolon() {
            tagAddedSpy.clear()
            tagsPastedSpy.clear()
            input.text = ""
            clipboard.setText(";;keyword1;keyword2;;;")

            input.forceActiveFocus()
            keyClick(Qt.Key_V, Qt.ControlModifier)

            compare(input.text, "")
            compare(tagAddedSpy.count, 0)
            compare(tagsPastedSpy.count, 1)
            var list = tagsPastedSpy.signalArguments[0][0]
            compare(list, ["keyword1", "keyword2"])
        }

        function test_CopyRequestWhenEmpty() {
            copyAllSpy.clear()
            input.text = ""

            input.forceActiveFocus()
            keyClick(Qt.Key_C, Qt.ControlModifier)

            compare(input.text, "")
            compare(copyAllSpy.count, 1)
        }

        function test_AutoCompleteFromThreeChars() {
            completionRequestedSpy.clear()
            input.text = ""
            input.forceActiveFocus()

            keyClick(Qt.Key_A)
            compare(completionRequestedSpy.count, 0)

            keyClick(Qt.Key_B)
            compare(completionRequestedSpy.count, 0)

            keyClick(Qt.Key_C)
            compare(completionRequestedSpy.count, 1)
        }

        function test_AutoCompleteBasicCtrlSpace() {
            completionRequestedSpy.clear()
            input.text = ""
            input.forceActiveFocus()

            input.text = "abc"
            input.cursorPosition = 3

            compare(completionRequestedSpy.count, 0)

            keyClick(Qt.Key_Space, Qt.ControlModifier)

            compare(completionRequestedSpy.count, 1)
        }

        function test_AutoCompleteCtrlSpaceTooShort() {
            completionRequestedSpy.clear()
            input.text = ""
            input.forceActiveFocus()

            input.text = "ab"
            input.cursorPosition = 2

            compare(completionRequestedSpy.count, 0)

            keyClick(Qt.Key_Space, Qt.ControlModifier)

            compare(completionRequestedSpy.count, 0)
        }

        function test_AutoCompletionCancelOnSpace() {
            completionRequestedSpy.clear()
            acSource.clear()

            input.text = ""
            input.forceActiveFocus()

            input.text = "abc"
            input.cursorPosition = 3

            compare(acSource.isCancelled, false)
            compare(completionRequestedSpy.count, 0)

            keyClick(Qt.Key_Space)

            compare(completionRequestedSpy.count, 0)
            compare(acSource.isCancelled, true)
        }

        function test_AcceptWithActiveAutoComplete() {
            completionRequestedSpy.clear()
            acSource.clear()

            input.text = ""
            input.forceActiveFocus()

            keyClick(Qt.Key_A)
            keyClick(Qt.Key_B)
            keyClick(Qt.Key_C)

            compare(acSource.isAccepted, false)
            acSource.isActive = true

            keyClick(Qt.Key_Return)

            compare(acSource.isAccepted, true)
        }

        function test_AcceptNoSelectedWithActiveAC() {
            tagAddedSpy.clear()
            completionRequestedSpy.clear()
            acSource.clear()

            input.text = ""
            input.forceActiveFocus()

            keyClick(Qt.Key_A)
            keyClick(Qt.Key_B)
            keyClick(Qt.Key_C)

            compare(acSource.isAccepted, false)
            compare(tagAddedSpy.count, 0)
            acSource.isActive = true
            acSource.isAnySelected = false

            keyClick(Qt.Key_Return)

            compare(acSource.isAccepted, true)
            compare(tagAddedSpy.count, 1)
        }

        function test_AutoCompletePreset() {
            completionRequestedSpy.clear()
            acSource.clear()

            input.text = ""
            input.forceActiveFocus()

            keyClick(Qt.Key_A)
            keyClick(Qt.Key_B)
            keyClick(Qt.Key_C)

            compare(acSource.isAccepted, false)
            compare(acSource.tryExpandPresetFlag, false)
            acSource.isActive = true

            keyClick(Qt.Key_Return, Qt.ControlModifier)

            compare(acSource.isAccepted, true)
            compare(acSource.tryExpandPresetFlag, true)
        }
    }
}

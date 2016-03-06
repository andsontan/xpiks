import QtQuick 2.0
import QtTest 1.1
import xpiks 1.0
import "../../xpiks-qt/Components"

Item {
    id: root
    width: 800
    height: 600

    QtObject {
        id: settingsModel
        property int keywordSizeScale: 1
    }

    QtObject {
        id: helpersWrapper
        property bool isValidMock: true

        function sanitizeKeyword(keyword) {
            return keyword;
        }

        function isKeywordValid(keyword) {
            return isValidMock;
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
            compare(tagAddedSpy.count, 0)

            input.text = "next_keyword"
            input.forceActiveFocus()
            keyClick(Qt.Key_Comma)

            compare(tagAddedSpy.count, 1)
        }

        function test_GetTextAPI() {
            input.text = "keyword"
            compare(editableTags.getEditedText(), "keyword")
        }

        function test_RaiseTagAdded() {
            tagAddedSpy.clear()
            compare(tagAddedSpy.count, 0)
            helpersWrapper.isValidMock = false
            editableTags.raiseAddTag("any text")
            compare(tagAddedSpy.count, 0)
            helpersWrapper.isValidMock = true
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
            input.forceActiveFocus()
            keyClick(Qt.Key_Backspace)
            compare(removeLastSpy.count, 0)

            keyClick(Qt.Key_Backspace)
            compare(removeLastSpy.count, 1)
        }

        function test_PasteNoComma() {
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

        function test_PasteWithComma() {
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
    }
}

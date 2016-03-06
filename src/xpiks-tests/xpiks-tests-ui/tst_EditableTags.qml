import QtQuick 2.0
import QtTest 1.1
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

        function sanitizeKeyword(keyword) {
            return keyword;
        }

        function isKeywordValid(keyword) {
            return true;
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

    TestCase {
        name: "EditableTagsTests"
        when: windowShown
        property var input

        function initTestCase() {
            input = findChild(editableTags, "nextTagTextInput")
        }

        function test_PressCommaAddsKeyword() {
            compare(tagAddedSpy.count, 0)

            input.text = "next_keyword"
            input.forceActiveFocus()
            keyClick(Qt.Key_Comma)

            compare(tagAddedSpy.count, 1)
        }
    }
}

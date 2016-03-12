import QtQuick 2.0
import QtTest 1.1
import xpiks 1.0
import "../../xpiks-qt/Dialogs"

Item {
    id: root
    width: 800
    height: 600

    property string path: ''
    property bool isselected: false

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

        function initDescriptionHighlighting(){}
        function initTitleHighlighting(){}
        function getKeywordsModel() { return {}; }
    }

    CombinedArtworksDialog {
        id: combinedDialog
    }

    TestCase {
        name: "CombinedEdit"
        when: windowShown
        property var descriptionInput
        property var titleInput
        property var keywordsInput
        property var descriptionCheckBox

        function initTestCase() {
            titleInput = findChild(combinedDialog, "titleTextInput")
            descriptionInput = findChild(combinedDialog, "descriptionTextInput")
            keywordsInput = findChild(combinedDialog, "keywordsInput")
            descriptionCheckBox = findChild(combinedDialog, "descriptionCheckBox")
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

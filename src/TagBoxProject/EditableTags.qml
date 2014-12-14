import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import TagBoxProject 1.0

Flickable {
    id: flowListView
    anchors.fill: parent
    contentWidth: parent.width;
    contentHeight: flow.childrenRect.height
    property alias count: repeater.count
    property int currentIndex: -1
    property variant currentItem;
    property alias delegate: repeater.delegate
    property alias flow: flow.flow
    property alias model: repeater.model

    signal tagAdded(string text)
    signal removeLast()

    function getEditedText() {
        var tagText = nextTagTextInput.text;
        return tagText.replace(/^\s+|\s+|-$/g, '');
    }

    function raiseAddTag(text) {
        var sanitizedTagText = text.replace(/^\s+|\s+|-$/g, '');
        if (getCharsCount(sanitizedTagText) > 2) {
            tagAdded(sanitizedTagText);
        }
    }

    function getCharsCount(text) {
        return (text.match(/[a-zA-Z]/g) || []).length
    }

    Flow {
        id: flow
        width: parent.width
        spacing: 5

        Repeater {
            id: repeater
            onCountChanged: {
                if (flowListView.currentIndex === -1 && count > 0) {
                    flowListView.currentIndex = 0
                }
                if (flowListView.currentIndex >= count) {
                    flowListView.currentIndex = count - 1
                }
            }
        }

        Item {
            width: 150
            height: 30
            TextInput {
                maximumLength: 30
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                id: nextTagTextInput
                color: "black"
                focus: true

                validator: RegExpValidator {
                    regExp: /^(?:[a-zA-Z]+(?:-| |$))+$/
                }

                onEditingFinished: {
                    var tagText = getEditedText();
                    raiseAddTag(tagText);

                    if (getCharsCount(tagText) > 2) {
                        nextTagTextInput.text = '';
                    }
                }

                Keys.onPressed: {
                    if(event.matches(StandardKey.Paste)) {
                        var clipboardText = clipboard.getText();

                        var words = clipboardText.split(',');
                        for (var i = 0; i < words.length; i++) {
                            raiseAddTag(words[i]);
                        }

                        event.accepted = true;
                    }
                    else if (event.key === Qt.Key_Comma) {
                        var tagText = getEditedText();
                        if (getCharsCount(tagText) > 2) {
                            raiseAddTag(tagText);
                            nextTagTextInput.text = '';
                        }

                        event.accepted = true;
                    }
                    else if (event.key === Qt.Key_Backspace) {
                        if (nextTagTextInput.length == 0) {
                            removeLast();
                            event.accepted = true;
                        }
                    }
                }
            }
        }

        ClipboardHelper {
            id: clipboard
        }
    }
}

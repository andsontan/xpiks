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
    /*onCurrentIndexChanged: {
        currentItem = model.get(currentIndex)
    }*/

    signal commaEntered(string text)

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

                Keys.onPressed: {
                    if(event.matches(StandardKey.Paste)) {
                        var clipboardText = clipboard.getText();

                        var words = clipboardText.split(',');
                        for (var i = 0; i < words.length; i++) {
                            commaEntered(words[i].replace(/^\s+|\s+$/g, ''));
                        }

                        event.accepted = true;
                    }
                    else if (event.key === Qt.Key_Comma) {
                        var tagText = nextTagTextInput.text;
                        commaEntered(tagText);
                        nextTagTextInput.text = ''

                        event.accepted = true;
                    }
                }
            }
        }

        ClipboardHelper {
            id: clipboard
        }
    }
}

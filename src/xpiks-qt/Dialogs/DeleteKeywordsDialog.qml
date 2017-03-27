/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.1
import QtQuick.Controls.Styles 1.1
import QtGraphicalEffects 1.0
import xpiks 1.0
import "../Constants"
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"
import "../Constants/UIConfig.js" as UIConfig

Item {
    id: deleteKeywordsComponent
    anchors.fill: parent

    property variant componentParent
    property var deleteKeywordsModel: helpersWrapper.getDeleteKeywordsModel()

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    Keys.onEscapePressed: closePopup()

    function closePopup() {
        deleteKeywordsModel.resetModel()
        deleteKeywordsComponent.destroy()
    }

    function mustUseConfirmation() {
        return settingsModel.mustUseConfirmations
    }

    Connections {
        target: helpersWrapper
        onGlobalBeforeDestruction: {
            console.debug("UI:DeleteKeywordsDialog # global
CloseRequested")
            closePopup()
        }
    }

    Connections {
        target: deleteKeywordsModel
        onRequestCloseWindow: {
            closePopup();
        }
    }

    PropertyAnimation { target: deleteKeywordsComponent; property: "opacity";
        duration: 400; from: 0; to: 1;
        easing.type: Easing.InOutQuad ; running: true }

    MessageDialog {
        id: confirmRemoveArtworksDialog
        property int itemsCount
        title: i18.n + qsTr("Confirmation")
        text: i18.n + qsTr("Are you sure you want to remove %1 item(s)?").arg(itemsCount)
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            doRemoveSelectedArtworks()
        }
    }

    function doRemoveSelectedArtworks() {
        deleteKeywordsModel.removeSelectedArtworks()
    }

    MessageDialog {
        id: clearKeywordsDialog

        title: i18.n + qsTr("Confirmation")
        text: i18.n + qsTr("Clear all keywords?")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: deleteKeywordsModel.clearKeywordsToDelete()
    }

    // This rectange is the a overlay to partially show the parent through it
    // and clicking outside of the 'dialog' popup will do 'nothing'
    Rectangle {
        anchors.fill: parent
        id: overlay
        color: "#000000"
        opacity: 0.6
        // add a mouse area so that clicks outside
        // the dialog window will not do anything
        MouseArea {
            anchors.fill: parent
        }
    }

    FocusScope {
        anchors.fill: parent

        MouseArea {
            anchors.fill: parent
            onWheel: wheel.accepted = true
            onClicked: mouse.accepted = true
            onDoubleClicked: mouse.accepted = true

            property real old_x : 0
            property real old_y : 0

            onPressed:{
                var tmp = mapToItem(deleteKeywordsComponent, mouse.x, mouse.y);
                old_x = tmp.x;
                old_y = tmp.y;
            }

            onPositionChanged: {
                var old_xy = Common.movePopupInsideComponent(deleteKeywordsComponent, dialogWindow, mouse, old_x, old_y);
                old_x = old_xy[0]; old_y = old_xy[1];
            }
        }

        RectangularGlow {
            anchors.fill: dialogWindow
            anchors.topMargin: glowRadius/2
            anchors.bottomMargin: -glowRadius/2
            glowRadius: 4
            spread: 0.0
            color: Colors.defaultControlColor
            cornerRadius: glowRadius
        }

        // This rectangle is the actual popup
        Rectangle {
            id: dialogWindow
            width: 730
            height: 610
            color: Colors.popupBackgroundColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            ColumnLayout {
                anchors.fill: parent
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                anchors.topMargin: 20
                anchors.bottomMargin: 20
                spacing: 0

                RowLayout {
                    anchors.left: parent.left
                    anchors.right: parent.right

                    StyledText {
                        text: i18.n + qsTr("Delete keywords")
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledText {
                        id: textItemsAvailable
                        text: i18.n + getOriginalText()

                        function getOriginalText() {
                            return deleteKeywordsModel.artworksCount === 1 ? qsTr("1 artwork selected") : qsTr("%1 artworks selected").arg(deleteKeywordsModel.artworksCount)
                        }

                        Connections {
                            target: deleteKeywordsModel
                            onArtworksCountChanged: {
                               var originalText = textItemsAvailable.getOriginalText();
                               textItemsAvailable.text = i18.n + originalText
                            }
                        }
                    }
                }

                Item {
                    height: 15
                }

                RowLayout {
                    spacing: 5
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: 40

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        text: i18.n + qsTr("Remove selected")
                        width: 150
                        tooltip: i18.n + qsTr("Remove selected artworks from this dialog")
                        enabled: deleteKeywordsModel.selectedArtworksCount > 0
                        onClicked: {
                            if (mustUseConfirmation()) {
                                confirmRemoveArtworksDialog.itemsCount = deleteKeywordsModel.selectedArtworksCount
                                confirmRemoveArtworksDialog.open()
                            } else {
                                doRemoveSelectedArtworks()
                            }
                        }
                    }
                }

                Item {
                    height: 10
                }

                Item {
                    height: 130
                    anchors.left: parent.left
                    anchors.right: parent.right

                    Rectangle {
                        anchors.fill: parent
                        color: Colors.defaultControlColor

                        StyledScrollView {
                            id: imagesScrollView
                            height: parent.height + 15
                            width: parent.width
                            anchors.margins: 10

                            ListView {
                                boundsBehavior: Flickable.StopAtBounds
                                anchors.fill: parent
                                anchors.margins: 10
                                orientation: Qt.Horizontal
                                spacing: 10
                                model: deleteKeywordsModel

                                add: Transition {
                                    NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 230 }
                                }

                                remove: Transition {
                                    NumberAnimation { property: "opacity"; to: 0; duration: 230 }
                                }

                                displaced: Transition {
                                    NumberAnimation { properties: "x,y"; duration: 230 }
                                }

                                addDisplaced: Transition {
                                    NumberAnimation { properties: "x,y"; duration: 230 }
                                }

                                removeDisplaced: Transition {
                                    NumberAnimation { properties: "x,y"; duration: 230 }
                                }

                                delegate: Item {
                                    property int delegateIndex: index
                                    id: imageWrapper
                                    height: 110
                                    width: height

                                    Image {
                                        anchors.fill: parent
                                        anchors.margins: 1
                                        source: "image://cached/" + filepath
                                        sourceSize.width: 150
                                        sourceSize.height: 150
                                        fillMode: settingsModel.fitSmallPreview ? Image.PreserveAspectFit : Image.PreserveAspectCrop
                                        asynchronous: true
                                        cache: false
                                    }

                                    Rectangle {
                                        anchors.fill: parent
                                        color: Colors.defaultControlColor
                                        opacity: isselected ? (mouseArea.containsMouse ? 0.6 : 0.7) : (mouseArea.containsMouse ? 0.4 : 0)
                                    }

                                    SelectedIcon {
                                        opacity: isselected ? (mouseArea.containsMouse ? 0.85 : 1) : (mouseArea.containsMouse ? 0.6 : 0)
                                        width: parent.width * 0.33
                                        height: parent.height * 0.33
                                        anchors.centerIn: parent
                                    }

                                    MouseArea {
                                        id: mouseArea
                                        anchors.fill: parent
                                        hoverEnabled: true
                                        onClicked: {
                                            deleteKeywordsModel.setArtworkSelected(delegateIndex, !isselected)
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                Item {
                    Layout.fillHeight: true
                }

                Item {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: 120

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 0
                        spacing: 0

                        RowLayout {
                            spacing: 5

                            StyledText {
                                id: keywordsLabel
                                text: i18.n + qsTr("Keywords to delete:")
                            }

                            StyledText {
                                text: i18.n + qsTr("(comma-separated)")
                                isActive: false
                            }

                            Item {
                                Layout.fillWidth: true
                            }

                            StyledText {
                                text: deleteKeywordsModel.keywordsToDeleteCount
                            }
                        }

                        Item {
                            height: 5
                        }

                        Rectangle {
                            id: keywordsToDeleteWrapper
                            border.color: Colors.artworkActiveColor
                            border.width: flv.isFocused ? 1 : 0
                            height: 80
                            anchors.rightMargin: 20
                            Layout.fillWidth: true
                            color: enabled ? Colors.inputBackgroundColor : Colors.inputInactiveBackground
                            property var keywordsModel: deleteKeywordsModel.getKeywordsToDeleteModel()

                            function removeKeyword(index) {
                                deleteKeywordsModel.removeKeywordToDeleteAt(index)
                            }

                            function removeLastKeyword() {
                                deleteKeywordsModel.removeLastKeywordToDelete()
                            }

                            function appendKeyword(keyword) {
                                deleteKeywordsModel.appendKeywordToDelete(keyword)
                            }

                            function pasteKeywords(keywordsList) {
                                deleteKeywordsModel.pasteKeywordsToDelete(keywordsList)
                            }

                            EditableTags {
                                id: flv
                                objectName: "keywordsInput"
                                anchors.fill: parent
                                model: keywordsToDeleteWrapper.keywordsModel
                                property int keywordHeight: 20 * settingsModel.keywordSizeScale + (settingsModel.keywordSizeScale - 1)*10
                                scrollStep: keywordHeight

                                delegate: KeywordWrapper {
                                    id: kw
                                    keywordText: keyword
                                    hasSpellCheckError: !iscorrect
                                    delegateIndex: index
                                    isHighlighted: true
                                    itemHeight: flv.keywordHeight
                                    onRemoveClicked: keywordsToDeleteWrapper.removeKeyword(delegateIndex)
                                    onActionDoubleClicked: {
                                        // DO NOTHING
                                    }
                                }

                                onTagAdded: {
                                    keywordsToDeleteWrapper.appendKeyword(text)
                                }

                                onRemoveLast: {
                                    keywordsToDeleteWrapper.removeLastKeyword()
                                }

                                onTagsPasted: {
                                    keywordsToDeleteWrapper.pasteKeywords(tagsList)
                                }

                                onBackTabPressed: {
                                    // BUMP
                                }

                                onCompletionRequested: {
                                    // BUMP
                                }
                            }

                            CustomScrollbar {
                                anchors.topMargin: -5
                                anchors.bottomMargin: -5
                                anchors.rightMargin: -15
                                flickable: flv
                            }
                        }

                        Item { height: 5 }

                        Item {
                            anchors.left: parent.left
                            anchors.right: parent.right
                            height: childrenRect.height

                            StyledText {
                                anchors.top: parent.top
                                anchors.rightMargin: 3
                                anchors.right: parent.right
                                text: i18.n + qsTr("Clear")
                                enabled: deleteKeywordsModel.keywordsToDeleteCount > 0
                                color: enabled ? (clearKeywordsMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor) : Colors.labelActiveForeground

                                MouseArea {
                                    id: clearKeywordsMA
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: {
                                        if (deleteKeywordsModel.keywordsToDeleteCount > 0) {
                                            clearKeywordsDialog.open()
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                Item {
                    height: 10
                }

                Item {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: 175

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 0
                        spacing: 5

                        RowLayout {
                            spacing: 5

                            StyledText {
                                text: i18.n + qsTr("Common keywords:")
                            }

                            Item {
                                Layout.fillWidth: true
                            }

                            StyledText {
                                text: deleteKeywordsModel.commonKeywordsCount
                            }
                        }

                        Rectangle {
                            id: commonKeywordsWrapper
                            border.width: 0
                            height: 155
                            anchors.rightMargin: 20
                            Layout.fillWidth: true
                            color: Colors.inputInactiveBackground
                            property var keywordsModel: deleteKeywordsModel.getCommonKeywordsModel()

                            function removeKeyword(index) {
                                var keyword = deleteKeywordsModel.removeCommonKeywordAt(index)
                                if (keyword.length !== 0) {
                                    deleteKeywordsModel.appendKeywordToDelete(keyword)
                                }
                            }

                            EditableTags {
                                id: flvCommon
                                objectName: "keywordsInput"
                                anchors.fill: parent
                                model: commonKeywordsWrapper.keywordsModel
                                property int keywordHeight: 20 * settingsModel.keywordSizeScale + (settingsModel.keywordSizeScale - 1)*10
                                scrollStep: keywordHeight
                                editEnabled: false

                                delegate: KeywordWrapper {
                                    keywordText: keyword
                                    hasSpellCheckError: !iscorrect
                                    delegateIndex: index
                                    isHighlighted: false
                                    hasPlusSign: true
                                    itemHeight: flvCommon.keywordHeight
                                    onRemoveClicked: commonKeywordsWrapper.removeKeyword(delegateIndex)
                                    onActionDoubleClicked: {
                                        // DO NOTHING
                                    }
                                }

                                onTagAdded: {
                                    // BUMP
                                }

                                onRemoveLast: {
                                    // BUMP
                                }

                                onTagsPasted: {
                                    // BUMP
                                }

                                onBackTabPressed: {
                                    // BUMP
                                }

                                onCompletionRequested: {
                                    // BUMP
                                }
                            }

                            CustomScrollbar {
                                anchors.topMargin: -5
                                anchors.bottomMargin: -5
                                anchors.rightMargin: -15
                                flickable: flvCommon
                            }
                        }
                    }
                }

                Item {
                    height: 20
                }

                Item {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: 24

                    RowLayout {
                        anchors.fill: parent
                        spacing: 20

                        StyledCheckbox {
                            id: caseSensitiveCheckbox
                            text: i18.n + qsTr("Case sensitive")
                            Component.onCompleted: caseSensitiveCheckbox.checked = deleteKeywordsModel.caseSensitive
                            onClicked: {
                                deleteKeywordsModel.caseSensitive = caseSensitiveCheckbox.checked
                            }
                        }

                        Item {
                            Layout.fillWidth: true
                        }

                        StyledButton {
                            text: i18.n + qsTr("Delete")
                            width: 100
                            onClicked: {
                                flv.onBeforeClose()
                                deleteKeywordsModel.deleteKeywords()
                                closePopup()
                            }
                        }

                        StyledButton {
                            text: i18.n + qsTr("Cancel")
                            width: 100
                            onClicked: {
                                closePopup()
                            }

                            tooltip: "Exit with no changes"
                        }
                    }
                }
            }
        }
    }

    ClipboardHelper {
        id: clipboard
    }

    Component.onCompleted: {
        focus = true
        flv.activateEdit()
    }
}

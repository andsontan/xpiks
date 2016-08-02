/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2016 Taras Kushnir <kushnirTV@gmail.com>
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
import QtQuick.Dialogs 1.1
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import "../Constants"
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"
import "../Constants/UIConfig.js" as UIConfig

Item {
    id: replacePreviewComponent
    anchors.fill: parent
    property variant componentParent
    property bool isRestricted: false

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    function closePopup() {
        replacePreviewComponent.destroy()
    }

    Component.onCompleted: focus = true
    Keys.onEscapePressed: closePopup()

    PropertyAnimation { target: replacePreviewComponent; property: "opacity";
        duration: 400; from: 0; to: 1;
        easing.type: Easing.InOutQuad ; running: true }

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
                var tmp = mapToItem(replacePreviewComponent, mouse.x, mouse.y);
                old_x = tmp.x;
                old_y = tmp.y;

                var dialogPoint = mapToItem(dialogWindow, mouse.x, mouse.y);
                if (!Common.isInComponent(dialogPoint, dialogWindow)) {
                    closePopup()
                }
            }

            onPositionChanged: {
                var old_xy = Common.movePopupInsideComponent(replacePreviewComponent, dialogWindow, mouse, old_x, old_y);
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
            width: 700
            height: 580
            color: Colors.selectedImageBackground
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            ColumnLayout {
                spacing: 10
                anchors.fill: parent
                anchors.margins: 20

                RowLayout {
                    spacing: 5

                    StyledText {
                        text: i18.n + qsTr("Replace")
                    }

                    StyledText {
                        text: '"' + replaceModel.replaceFrom + '"'
                    }

                    StyledText {
                        text: i18.n + qsTr("to")
                    }

                    StyledText {
                        text: '"' + replaceModel.replaceTo + '"'
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: Colors.defaultControlColor

                    StyledScrollView {
                        anchors.fill: parent
                        anchors.margins: 10

                        ListView {
                            id: replacePreviewList
                            model: replaceModel
                            spacing: 5

                            delegate: Rectangle {
                                id: imageWrapper
                                property int delegateIndex: index
                                color: isselected ? Colors.selectedImageBackground : Colors.artworkImageBackground
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.rightMargin: 10
                                height: columnRectangle.height

                                StyledCheckbox {
                                    id: applyReplaceCheckBox
                                    anchors.leftMargin: 14
                                    anchors.left: parent.left
                                    width: 20
                                    anchors.verticalCenter: parent.verticalCenter
                                    onClicked: editisselected = checked
                                    Component.onCompleted: applyReplaceCheckBox.checked = isselected

                                    Connections {
                                        target: replaceModel
                                        onAllSelectedChanged: {
                                            applyReplaceCheckBox.checked = isselected
                                        }
                                    }
                                }

                                Item {
                                    id: imageItem
                                    anchors.leftMargin: 0
                                    anchors.left: applyReplaceCheckBox.right
                                    anchors.top: parent.top
                                    width: 120
                                    height: parent.height

                                    ColumnLayout {
                                        anchors.centerIn: parent
                                        anchors.verticalCenterOffset: 7
                                        spacing: 7
                                        width: 110

                                        Item {
                                            id: imageContainer
                                            width: 100
                                            height: 100
                                            anchors.horizontalCenter: parent.horizontalCenter

                                            Image {
                                                anchors.fill: parent
                                                source: "image://cached/" + path
                                                sourceSize.width: 150
                                                sourceSize.height: 150
                                                fillMode: settingsModel.fitSmallPreview ? Image.PreserveAspectFit : Image.PreserveAspectCrop
                                                asynchronous: true
                                                cache: false
                                            }

                                            MouseArea {
                                                anchors.fill: parent
                                                onClicked: {
                                                    editisselected = !isselected
                                                    applyReplaceCheckBox.checked = isselected
                                                }
                                            }
                                        }

                                        StyledText {
                                            Layout.fillWidth: true
                                            elide: Text.ElideMiddle
                                            horizontalAlignment: Text.AlignHCenter
                                            text: path.split(/[\\/]/).pop()
                                            font.pixelSize: UIConfig.fontPixelSize
                                        }

                                        Item {
                                            Layout.fillHeight: true
                                        }
                                    }
                                }

                                Rectangle {
                                    id: columnRectangle
                                    anchors.left: imageItem.right
                                    anchors.top: parent.top
                                    anchors.right: parent.right
                                    height: (childrenRect.height < 130) ? 150 : (childrenRect.height + 20)
                                    color: isselected ? Colors.selectedArtworkBackground : Colors.artworkBackground

                                    Column {
                                        id: replaceHitText
                                        spacing: 3
                                        anchors.top: parent.top
                                        anchors.left: parent.left
                                        anchors.right: parent.right
                                        anchors.leftMargin: 10
                                        anchors.rightMargin: 10
                                        anchors.topMargin: 10

                                        StyledText {
                                            id: titleHit
                                            text: i18.n + qsTr("Title:")
                                            visible: hastitle
                                            isActive: isselected
                                        }

                                        Rectangle {
                                            id: titleRectangle
                                            height: childrenRect.height + 10
                                            color: isselected ? Colors.inputBackgroundColor : Colors.inputInactiveBackground
                                            anchors.right: parent.right
                                            anchors.left: parent.left
                                            visible: hastitle

                                            StyledTextEdit {
                                                id: titleText
                                                anchors.top: parent.top
                                                anchors.topMargin: 5
                                                anchors.right: parent.right
                                                anchors.left: parent.left
                                                anchors.leftMargin: 5
                                                anchors.rightMargin: 5
                                                wrapMode: TextEdit.Wrap
                                                text: i18.n + replaceModel.getSearchTitle(imageWrapper.delegateIndex)
                                                readOnly: true
                                                selectByKeyboard: false
                                                selectByMouse: false
                                                isActive: isselected
                                                Component.onCompleted: {
                                                    replaceModel.initHighlighting(titleText.textDocument)
                                                }
                                            }
                                        }

                                        Item {
                                            width: 10
                                            height: 1
                                        }

                                        StyledText {
                                            id: descriptionHit
                                            text: i18.n + qsTr("Description:")
                                            visible: hasdescription
                                            isActive: isselected
                                        }

                                        Rectangle {
                                            id: descriptionRectangle
                                            color: isselected ? Colors.inputBackgroundColor : Colors.inputInactiveBackground
                                            height: childrenRect.height + 10
                                            anchors.right: parent.right
                                            anchors.left: parent.left
                                            visible: hasdescription

                                            StyledTextEdit {
                                                id: descriptionText
                                                wrapMode: TextEdit.Wrap
                                                anchors.top: parent.top
                                                anchors.topMargin: 5
                                                anchors.right: parent.right
                                                anchors.left: parent.left
                                                anchors.leftMargin: 5
                                                anchors.rightMargin: 5
                                                text: i18.n + replaceModel.getSearchDescription(imageWrapper.delegateIndex).trim()
                                                readOnly: true
                                                selectByKeyboard: false
                                                selectByMouse: false
                                                isActive: isselected
                                                Component.onCompleted: {
                                                    replaceModel.initHighlighting(descriptionText.textDocument)
                                                }
                                            }
                                        }

                                        Item {
                                            width: 10
                                            height: 1
                                        }

                                        StyledText {
                                            id: keywordsHit
                                            text: i18.n + qsTr("Keywords:")
                                            visible: haskeywords
                                            isActive: isselected
                                        }

                                        Rectangle {
                                            color: isselected ? Colors.inputBackgroundColor : Colors.inputInactiveBackground
                                            height: childrenRect.height + 10
                                            anchors.right: parent.right
                                            anchors.left: parent.left
                                            visible: haskeywords

                                            StyledTextEdit {
                                                id: keywordsText
                                                anchors.top: parent.top
                                                anchors.topMargin: 5
                                                anchors.right: parent.right
                                                anchors.left: parent.left
                                                anchors.leftMargin: 5
                                                anchors.rightMargin: 5
                                                wrapMode: TextEdit.Wrap
                                                text: i18.n + replaceModel.getSearchKeywords(imageWrapper.delegateIndex)
                                                readOnly: true
                                                selectByKeyboard: false
                                                selectByMouse: false
                                                isActive: isselected

                                                Component.onCompleted: {
                                                    replaceModel.initHighlighting(keywordsText.textDocument)
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    Item {
                        anchors.fill: parent
                        visible: replaceModel.count == 0

                        StyledText {
                            text: i18.n + qsTr("There are no items to replace")
                            anchors.centerIn: parent
                            color: Colors.selectedArtworkBackground
                        }
                    }
                }

                Item {
                    height: 1
                }

                RowLayout {
                    spacing: 20
                    height: 24

                    StyledButton {
                        text: i18.n + qsTr("Select all")
                        width: 100
                        onClicked: {
                            replaceModel.selectAll();
                        }
                    }

                    StyledButton {
                        text: i18.n + qsTr("Unselect all")
                        width: 100
                        onClicked: {
                            replaceModel.unselectAll();
                        }
                    }

                    Item {
                        Layout.fillWidth: true
                    }

                    StyledButton {
                        text: i18.n + qsTr("Replace")
                        width: 100
                        onClicked: {
                            replaceModel.replace()
                            // replacePreviewComponent.closePopup()
                            componentParent.closePopup()
                        }
                    }

                    StyledButton {
                        text: i18.n + qsTr("Close")
                        width: 100
                        onClicked: {
                            replacePreviewComponent.closePopup()
                        }
                    }
                }
            }
        }
    }
}

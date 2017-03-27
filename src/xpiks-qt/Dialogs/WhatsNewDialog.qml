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
import "../Constants"
import "../Common.js" as Common;
import "../Components"
import "../StyledControls"

Item {
    id: whatsNewComponent
    anchors.fill: parent

    z: 10000

    signal dialogDestruction();
    Component.onDestruction: dialogDestruction();

    function closePopup() {
        whatsNewComponent.destroy()
    }

    Component.onCompleted: focus = true

    PropertyAnimation { target: whatsNewComponent; property: "opacity";
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
            height: 500
            color: Colors.popupBackgroundColor
            anchors.centerIn: parent
            Component.onCompleted: anchors.centerIn = undefined

            StyledText {
                id: header
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.topMargin: 20
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                text: i18.n + qsTr("What's new")
            }

            Rectangle {
                id: slidesHost
                anchors.top: header.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                anchors.topMargin: 10
                anchors.bottom: footer.top
                anchors.bottomMargin: 20
                color: Colors.defaultControlColor
                clip: true

                property int currentSlideIndex: 0
                property int previousSlideIndex: 0

                property list<Item> whatsNewSlides: [
                    PresentationSlide {
                        parent: slidesHost
                        id: firstSlide
                        title: i18.n + qsTr("Presets for keywords")
                        imageUrl: "qrc:/Graphics/presets.png"
                        width: slidesHost.width
                        height: slidesHost.height
                    },
                    PresentationSlide {
                        parent: slidesHost
                        id: secondSlide
                        title: i18.n + qsTr("Additional buffer for quick editing")
                        imageUrl: "qrc:/Graphics/quickbuffer.png"
                        width: slidesHost.width
                        height: slidesHost.height
                    },
                    PresentationSlide {
                        parent: slidesHost
                        id: thirdSlide
                        title: i18.n + qsTr("Find and Replace functionality")
                        imageUrl: "qrc:/Graphics/findandreplace.png"
                        width: slidesHost.width
                        height: slidesHost.height
                    },
                    PresentationSlide {
                        parent: slidesHost
                        id: fourthSlide
                        title: i18.n + qsTr("StarDict dictionaries support")
                        imageUrl: "qrc:/Graphics/translator.png"
                        width: slidesHost.width
                        height: slidesHost.height
                    },
                    PresentationSlide {
                        parent: slidesHost
                        id: fifthSlide
                        title: i18.n + qsTr("List of failed uploads")
                        imageUrl: "qrc:/Graphics/faileduploads.png"
                        width: slidesHost.width
                        height: slidesHost.height
                    },
                    PresentationSlide {
                        parent: slidesHost
                        id: sixsSlide
                        title: i18.n + qsTr("Bug fixes and improvements")
                        imageUrl: "qrc:/Graphics/gears.png"
                        width: slidesHost.width
                        height: slidesHost.height
                    }
                ]

                Component.onCompleted: {
                    slidesHost.currentSlideIndex = 0;
                    slidesHost.previousSlideIndex = 0;

                    for (var i = 0; i < whatsNewSlides.length; ++i) {
                        whatsNewSlides[i].hide();
                    }

                    whatsNewSlides[0].show(0);
                }

                function showSlide(screenIndex, direction) {
                    whatsNewSlides[previousSlideIndex].hide();
                    var xVal = direction === -1 ? 400 : -400;
                    whatsNewSlides[screenIndex].show(xVal);
                }

                function swipeForward() {
                    if (currentSlideIndex < whatsNewSlides.length - 1) {
                        previousSlideIndex = currentSlideIndex;
                        currentSlideIndex = currentSlideIndex + 1;

                        showSlide(currentSlideIndex, -1)
                    }
                }

                function swipeBackward() {
                    if (currentSlideIndex > 0) {
                        previousSlideIndex = currentSlideIndex;
                        currentSlideIndex = currentSlideIndex - 1;

                        showSlide(currentSlideIndex, 1)
                    }
                }

                function isFinished() {
                    return currentSlideIndex === whatsNewSlides.length - 1
                }
            }

            RowLayout {
                id: footer
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 20
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.right: parent.right
                anchors.rightMargin: 20
                height: 24
                spacing: 20

                StyledText {
                    text: i18.n + qsTr("Skip")
                    color: skipMA.pressed ? Colors.linkClickedColor : Colors.labelInactiveForeground

                    MouseArea {
                        id: skipMA
                        anchors.fill: parent
                        cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                        onClicked: {
                            settingsModel.saveCurrentVersion()
                            closePopup()
                        }
                    }
                }

                Item {
                    Layout.fillWidth: true
                }

                StyledText {
                    text: i18.n + qsTr("Previous")
                    color: previousMA.pressed ? Colors.linkClickedColor : Colors.labelActiveForeground

                    MouseArea {
                        id: previousMA
                        anchors.fill: parent
                        cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                        onClicked: {
                            slidesHost.swipeBackward()
                            actionButton.text = qsTr("Next")
                        }
                    }
                }

                StyledButton {
                    id: actionButton
                    text: i18.n + qsTr("Next")
                    isDefault: true
                    width: 100
                    onClicked: {
                        if (slidesHost.isFinished()) {
                            settingsModel.saveCurrentVersion()
                            closePopup()
                        } else {
                            slidesHost.swipeForward()
                            if (slidesHost.isFinished()) {
                                actionButton.text = qsTr("Close")
                            }
                        }
                    }
                }
            }
        }
    }
}

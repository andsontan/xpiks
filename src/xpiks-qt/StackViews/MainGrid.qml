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
import QtQuick.Window 2.0
import xpiks 1.0
import "../Constants" 1.0
import "../StyledControls"
import "../Dialogs"
import "../Components"
import "../Common.js" as Common
import "../Constants/UIConfig.js" as UIConfig

ColumnLayout {
    id: mainGridComponent
    spacing: 0
    property int myLeftMargin: applicationWindow.leftSideCollapsed ? 0 : 2

    Action {
        id: searchAction
        shortcut: StandardKey.Find
        onTriggered: filterText.forceActiveFocus()
        enabled: (artworkRepository.artworksSourcesCount > 0) && (applicationWindow.openedDialogsCount == 0)
    }

    Action {
        id: selectAllAction
        shortcut: StandardKey.SelectAll
        enabled: (artworkRepository.artworksSourcesCount > 0) && (applicationWindow.openedDialogsCount == 0)
        onTriggered: {
            if (selectAllCheckbox.checked) {
                filteredArtItemsModel.selectFilteredArtworks();
            }
            else {
                filteredArtItemsModel.unselectFilteredArtworks();
            }
        }
    }

    Menu {
        id: wordRightClickMenu
        property string word
        property int artworkIndex
        property int keywordIndex
        property bool showAddToDict : true
        property bool showExpandPreset : false

        MenuItem {
            visible: wordRightClickMenu.showAddToDict
            text: i18.n + qsTr("Add to dictionary")
            onTriggered: spellCheckService.addWordToUserDictionary(wordRightClickMenu.word);
        }

        Menu {
            id : presetSubMenu
            visible: wordRightClickMenu.showExpandPreset
            title: i18.n + qsTr("Expand as preset")
            Instantiator {
                id : presetsInstantiator
                model: filteredPresetsModel
                onObjectAdded: presetSubMenu.insertItem( index, object )
                onObjectRemoved: presetSubMenu.removeItem( object )
                delegate: MenuItem {
                    text: i18.n + qsTr("\"%1\"").arg(filteredPresetsModel.getName(index))
                    onTriggered: {
                        filteredArtItemsModel.replaceFromPreset(wordRightClickMenu.artworkIndex, wordRightClickMenu.word,  filteredPresetsModel.getOriginalIndex(index));
                    }

                }
            }
        }
    }

    Menu {
        id: presetsMenu
        property int artworkIndex
        Instantiator {
            model: filteredPresetsModel
            onObjectAdded:{
                presetsMenu.insertItem( index, object )
            }
            onObjectRemoved: presetsMenu.removeItem( object )
            delegate: MenuItem {
                text: i18.n + qsTr("Expand as preset \"%1\"").arg(name)
                onTriggered: {
                    filteredArtItemsModel.appendFromPreset(presetsMenu.artworkIndex, filteredPresetsModel.getOriginalIndex(index));
                }

            }
        }
    }

    function closeAutoComplete() {
        if (typeof workflowHost.autoCompleteBox !== "undefined") {
            workflowHost.autoCompleteBox.closePopup()
        }
    }

    function clearFilter() {
        filteredArtItemsModel.searchTerm = ''
        if (filterText.length > 0) {
            filterText.text = ''
        }
        filterClearTimer.start()
    }

    Rectangle {
        height: 45
        color: Colors.defaultDarkColor
        z: 2000
        anchors.left: parent.left
        anchors.leftMargin: mainGridComponent.myLeftMargin
        anchors.right: parent.right

        RowLayout {
            spacing: 10
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: 0
            anchors.rightMargin: mainScrollView.areScrollbarsVisible ? 35 : 20

            Item {
                width: 5
            }

            LayoutButton {
                enabled: artworkRepository.artworksSourcesCount > 0
                anchors.verticalCenter: parent.verticalCenter
                isListLayout: applicationWindow.listLayout
                onLayoutChanged: {
                    applicationWindow.listLayout = !applicationWindow.listLayout
                    closeAutoComplete()
                }
            }

            Item {
                Layout.fillWidth: true
            }

            StyledButton {
                text: i18.n + qsTr("Remove")
                width: 90
                action: removeAction
            }

            StyledButton {
                text: i18.n + qsTr("Edit")
                width: 90
                action: editAction
            }

            StyledButton {
                text: i18.n + qsTr("Save")
                width: mainScrollView.areScrollbarsVisible ? 88 : 98
                action: saveAction
            }

            StyledButton {
                text: i18.n + qsTr("Upload")
                width: 100
                action: uploadAction
            }
        }
    }

    Rectangle {
        anchors.left: parent.left
        anchors.leftMargin: mainGridComponent.myLeftMargin
        anchors.right: parent.right
        Layout.fillHeight: true
        color: Colors.defaultControlColor

        Item {
            id: workflowHost
            anchors.topMargin: 10
            anchors.fill: parent
            property var autoCompleteBox

            function onAutoCompleteClose() {
                autoCompleteBox = undefined
            }

            Item {
                id: filterRect
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: 34

                StyledCheckbox {
                    id: selectAllCheckbox
                    anchors.left: parent.left
                    anchors.leftMargin: 20
                    anchors.verticalCenter: parent.verticalCenter
                    isContrast: true
                    enabled: artworkRepository.artworksSourcesCount > 0
                    text: i18.n + getOriginalText()
                    checked: filteredArtItemsModel.selectedArtworksCount > 0

                    function getOriginalText() {
                        return filteredArtItemsModel.selectedArtworksCount === 0 ? qsTr("Select all") : qsTr("Select none")
                    }

                    onClicked: {
                        if (checked) {
                            filteredArtItemsModel.selectFilteredArtworks();
                        }
                        else {
                            filteredArtItemsModel.unselectFilteredArtworks();
                        }
                    }

                    Connections {
                        target: filteredArtItemsModel
                        onSelectedArtworksCountChanged: {
                            selectAllCheckbox.checked = filteredArtItemsModel.selectedArtworksCount > 0
                        }
                    }
                }

                Rectangle {
                    anchors.right: searchButton.left
                    anchors.rightMargin: 10
                    anchors.verticalCenter: parent.verticalCenter
                    color: Colors.defaultDarkColor
                    width: (artworkRepository.artworksSourcesCount > 0 && mainScrollView.areScrollbarsVisible) ? 288 : 298
                    height: UIConfig.textInputHeight

                    StyledTextInput {
                        id: filterText
                        width: (artworkRepository.artworksSourcesCount > 0 && mainScrollView.areScrollbarsVisible) ? 250 : 260
                        height: UIConfig.textInputHeight
                        clip: true
                        anchors.left: parent.left
                        anchors.leftMargin: 5
                        enabled: artworkRepository.artworksSourcesCount > 0
                        selectionColor: Colors.inputInactiveForeground
                        selectedTextColor: Colors.whiteColor

                        onAccepted: {
                            filteredArtItemsModel.searchTerm = text
                        }

                        Connections {
                            target: filteredArtItemsModel
                            onSearchTermChanged: filterText.text = filteredArtItemsModel.searchTerm
                        }
                    }

                    CloseIcon {
                        width: 14
                        height: 14
                        anchors.right: parent.right
                        anchors.rightMargin: 5
                        enabled: filterText.length > 0
                        anchors.verticalCenter: parent.verticalCenter
                        onItemClicked: clearFilter()
                    }

                    StyledText {
                        text: i18.n + qsTr("Search...   x:empty  x:modified")
                        visible: !(filterClearTimer.running || filterText.activeFocus || (filterText.length > 0))
                        color: Colors.inputHintForeground
                        anchors.left: parent.left
                        anchors.leftMargin: 7
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Timer {
                        id: filterClearTimer
                        running: false
                        interval: 750
                        repeat: false
                    }
                }

                StyledBlackButton {
                    id: searchButton
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.rightMargin:  mainScrollView.areScrollbarsVisible ? 35 : 20
                    width: 100
                    text: i18.n + qsTr("Search")
                    enabled: artworkRepository.artworksSourcesCount > 0
                    onClicked: filteredArtItemsModel.searchTerm = filterText.text
                }
            }

            Rectangle {
                id: undoRedoRect
                color: Colors.defaultDarkColor
                width: parent.width
                height: 4
                opacity: 0
                anchors.topMargin: 0
                anchors.top: filterRect.bottom

                states: [
                    State {
                        name: "canundo"
                        when: undoRedoManager.canUndo
                        PropertyChanges {
                            target: undoRedoRect
                            height: 40
                        }
                    }
                ]

                transitions: [
                    Transition {
                        from: ""; to: "canundo"
                        ParallelAnimation {
                            NumberAnimation {
                                target: undoRedoRect
                                properties: "height";
                                from: 4
                                to: 40
                                easing.type: "InOutQuad";
                                duration: 250
                            }
                            NumberAnimation {
                                target: undoRedoRect
                                properties: "anchors.topMargin";
                                from: 0
                                to: 4
                                easing.type: "InOutQuad";
                                duration: 250
                            }
                            NumberAnimation {
                                target: undoRedoRect
                                properties: "opacity";
                                from: 0
                                to: 1
                                easing.type: "InOutQuad";
                                duration: 250
                            }
                        }
                    },
                    Transition {
                        from: "canundo"; to: ""
                        ParallelAnimation {
                            NumberAnimation {
                                target: undoRedoRect
                                properties: "height";
                                from: 40
                                to: 4
                                easing.type: "InOutQuad";
                                duration: 250
                            }
                            NumberAnimation {
                                target: undoRedoRect
                                properties: "anchors.topMargin";
                                from: 4
                                to: 0
                                easing.type: "InOutQuad";
                                duration: 250
                            }
                            NumberAnimation {
                                target: undoRedoRect
                                properties: "opacity";
                                from: 1
                                to: 0
                                easing.type: "InOutQuad";
                                duration: 250
                            }
                        }
                    }
                ]

                Rectangle {
                    anchors.left: parent.left
                    color: Colors.artworkSavedColor
                    width: applicationWindow.listLayout ? 6 : 3
                    height: parent.height
                }

                RowLayout {
                    anchors.centerIn: parent

                    StyledText {
                        id: undoDescription
                        text: undoRedoManager.undoDescription
                        isActive: false
                    }

                    Item {
                        width: 10
                    }

                    StyledText {
                        text: i18.n + qsTr("Undo")
                        color: undoMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor

                        MouseArea {
                            id: undoMA
                            anchors.fill: parent
                            enabled: undoRedoManager.canUndo
                            cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                            onClicked: {
                                undoRedoManager.undoLastAction()
                                filteredArtItemsModel.updateFilter()
                            }
                        }
                    }

                    StyledText {
                        text: i18.n + qsTr("Dismiss (%1)").arg(settingsModel.dismissDuration - (autoDismissTimer.iterations % (settingsModel.dismissDuration + 1)))
                        color: dismissUndoMA.pressed ? Colors.linkClickedColor : Colors.labelInactiveForeground

                        MouseArea {
                            id: dismissUndoMA
                            anchors.fill: parent
                            enabled: undoRedoManager.canUndo
                            cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor
                            onClicked: {
                                undoRedoManager.discardLastAction()
                            }
                        }
                    }

                }

                Timer {
                    id: autoDismissTimer
                    property int iterations: 0
                    interval: 1000
                    repeat: true
                    running: undoRedoManager.canUndo
                    onTriggered: {
                        iterations += 1

                        if (iterations % (settingsModel.dismissDuration + 1) === settingsModel.dismissDuration) {
                            undoRedoManager.discardLastAction()
                            iterations = 0
                        }
                    }
                }

                Connections {
                    target: undoRedoManager
                    onItemRecorded: {
                        autoDismissTimer.iterations = 0
                    }
                }
            }

            Rectangle {
                id: separator
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.rightMargin: artworksHost.count > 3 ? 15 : 0
                anchors.topMargin: visible ? 2 : 4
                anchors.top: undoRedoRect.bottom
                height: visible ? 2 : 0
                color: Colors.defaultDarkColor
                visible: !undoRedoManager.canUndo && (artworksHost.count > 0)
            }

            Item {
                id: mainScrollView
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: separator.bottom
                anchors.bottom: parent.bottom
                clip: true
                property bool areScrollbarsVisible: artworksHost.contentHeight > artworksHost.height
                //__wheelAreaScrollSpeed: 50 + 10*settingsModel.scrollSpeedScale
                // does not work for now in Qt 5.4.1 in combination with ListView
                //verticalScrollBarPolicy: Qt.ScrollBarAlwaysOn

                GridView {
                    id: artworksHost
                    anchors.fill: parent
                    anchors.rightMargin: mainScrollView.areScrollbarsVisible ? 10 : 0
                    model: filteredArtItemsModel
                    boundsBehavior: Flickable.StopAtBounds
                    property int cellSpacing: 4
                    property double defaultRowHeight: 205
                    cellHeight: applicationWindow.listLayout ? (defaultRowHeight + 80*(settingsModel.keywordSizeScale - 1.0) + cellSpacing) : (defaultRowHeight + cellSpacing)
                    cellWidth: applicationWindow.listLayout ? artworksHost.width : (208 + cellSpacing)
                    highlightFollowsCurrentItem: false
                    currentIndex: -1
                    focus: true
                    flickDeceleration: 1500/settingsModel.scrollSpeedScale

                    function forceUpdateArtworks(needToMoveCurrentItem) {
                        console.debug("UI::forceUpdateArtworks # updating main listview")
                        artworksHost.returnToBounds()
                        artworksHost.forceLayout()
                        artworksHost.update()

                        // this piece of code is here in order to beat caching of ListView
                        // when you remove all items but few it fails to draw them
                        if (needToMoveCurrentItem) {
                            console.debug("UI::forceUpdateArtworks # Moving into current item " + artworksHost.currentIndex)
                            artworksHost.moveCurrentIndexDown()
                            artworksHost.positionViewAtIndex(artworksHost.currentIndex, GridView.Visible)
                        }
                    }

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

                    onContentYChanged: closeAutoComplete()

                    delegate: FocusScope {
                        id: wrappersScope
                        width: applicationWindow.listLayout ? (mainScrollView.areScrollbarsVisible ? (parent.width - 5) : parent.width) : 208
                        height: applicationWindow.listLayout ? (artworksHost.defaultRowHeight + 80*(settingsModel.keywordSizeScale - 1.0)) : artworksHost.defaultRowHeight

                        function updateCurrentIndex() {
                            GridView.view.currentIndex = rowWrapper.delegateIndex
                        }

                        Rectangle {
                            id: rowWrapper
                            objectName: "artworkDelegate"
                            property bool isHighlighted: isselected || wrappersScope.GridView.isCurrentItem
                            color: isHighlighted ? Colors.selectedArtworkBackground : Colors.artworkBackground
                            property var artworkModel: filteredArtItemsModel.getArtworkMetadata(index)
                            property var keywordsModel: filteredArtItemsModel.getBasicModel(index)
                            property int delegateIndex: index
                            property bool isItemSelected: isselected
                            anchors.fill: parent

                            function getIndex() {
                                return filteredArtItemsModel.getOriginalIndex(index)
                            }

                            function focusDescription() {
                                descriptionTextInput.forceActiveFocus()
                                descriptionTextInput.cursorPosition = descriptionTextInput.text.length
                            }

                            function isAnyFocused() {
                                return descriptionTextInput.activeFocus || flv.isFocused || titleTextInput.activeFocus;
                            }

                            function focusIfNeeded() {
                                if (!isAnyFocused()) {
                                    focusDescription()
                                }

                                if (!isselected) {
                                    flv.removeFocus()
                                }
                            }

                            function switchChecked() {
                                editisselected = !isselected
                                itemCheckedCheckbox.checked = isselected
                                wrappersScope.updateCurrentIndex()
                            }

                            Connections {
                                target: rowWrapper.artworkModel

                                onFocusRequested: {
                                    if (directionSign === +1) {
                                        descriptionTextInput.forceActiveFocus()
                                        descriptionTextInput.cursorPosition = descriptionTextInput.text.length
                                    } else {
                                        flv.activateEdit()
                                    }

                                    artworksHost.positionViewAtIndex(rowWrapper.delegateIndex, GridView.Contain)
                                }
                            }

                            Connections {
                                target: rowWrapper.keywordsModel

                                onSpellCheckResultsReady: {
                                    descriptionTextInput.deselect()
                                    if (columnLayout.isWideEnough) {
                                        titleTextInput.deselect()
                                    }
                                }

                                onCompletionsAvailable: {
                                    if (typeof workflowHost.autoCompleteBox !== "undefined") {
                                        // update completion
                                        return
                                    }

                                    var directParent = mainScrollView;
                                    var currWordStartRect = flv.editControl.getCurrentWordStartRect()

                                    var tmp = flv.editControl.mapToItem(directParent,
                                                                        currWordStartRect.x - 17,
                                                                        flv.editControl.height + 1)

                                    var visibleItemsCount = Math.min(acSource.getCount(), 5);
                                    var popupHeight = visibleItemsCount * (25 + 1) + 10

                                    var isBelow = (tmp.y + popupHeight) < directParent.height;

                                    var options = {
                                        model: acSource,
                                        isBelowEdit: isBelow,
                                        "anchors.left": directParent.left,
                                        "anchors.leftMargin": Math.min(tmp.x, directParent.width - 200),
                                        "anchors.top": directParent.top
                                    }

                                    if (isBelow) {
                                        options["anchors.topMargin"] = tmp.y
                                    } else {
                                        options["anchors.topMargin"] = tmp.y - popupHeight - flv.editControl.height - 2
                                    }

                                    var component = Qt.createComponent("../Components/CompletionBox.qml");
                                    if (component.status !== Component.Ready) {
                                        console.debug("Component Error: " + component.errorString());
                                    } else {
                                        var instance = component.createObject(directParent, options);

                                        instance.boxDestruction.connect(workflowHost.onAutoCompleteClose)
                                        instance.itemSelected.connect(flv.editControl.acceptCompletion)
                                        workflowHost.autoCompleteBox = instance

                                        instance.openPopup()
                                    }
                                }
                            }

                            Item {
                                anchors.fill: parent

                                Rectangle {
                                    id: isModifiedRectangle
                                    color: ismodified ? Colors.artworkModifiedColor : Colors.artworkSavedColor
                                    width: applicationWindow.listLayout ? 6 : 3
                                    anchors.left: parent.left
                                    anchors.top: parent.top
                                    anchors.bottom: parent.bottom
                                }

                                Item {
                                    id: checkboxSpacer
                                    width: applicationWindow.listLayout ? 15 : 8
                                    anchors.left: isModifiedRectangle.right
                                    anchors.top: parent.top
                                    anchors.bottom: parent.bottom

                                    MouseArea {
                                        anchors.fill: parent
                                        enabled: applicationWindow.listLayout
                                        onClicked: {
                                            rowWrapper.switchChecked()
                                            rowWrapper.focusIfNeeded()
                                        }
                                    }
                                }

                                Item {
                                    id: checkboxRectangle
                                    anchors.left: checkboxSpacer.right
                                    anchors.top: parent.top
                                    anchors.bottom: parent.bottom
                                    width: 20

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            rowWrapper.switchChecked()
                                            rowWrapper.focusIfNeeded()
                                        }
                                    }

                                    StyledCheckbox {
                                        id: itemCheckedCheckbox
                                        //checked: isselected
                                        focus: false
                                        anchors.left: parent.left
                                        anchors.verticalCenter: parent.verticalCenter
                                        activeFocusOnPress: false
                                        onClicked: editisselected = checked
                                        Component.onCompleted: itemCheckedCheckbox.checked = isselected

                                        Connections {
                                            target: filteredArtItemsModel
                                            onAllItemsSelectedChanged: {
                                                itemCheckedCheckbox.checked = rowWrapper.isItemSelected
                                            }
                                            onForceUnselected: {
                                                itemCheckedCheckbox.checked = false
                                            }
                                        }
                                    }
                                }

                                Item {
                                    id: imageColumnWrapper
                                    width: applicationWindow.listLayout ? 180 : 160
                                    anchors.left: checkboxRectangle.right
                                    anchors.top: parent.top
                                    anchors.bottom: parent.bottom

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            rowWrapper.switchChecked()
                                            rowWrapper.focusIfNeeded()
                                        }
                                    }

                                    Item {
                                        anchors.fill: parent
                                        anchors.leftMargin: applicationWindow.listLayout ? 10 : 0
                                        anchors.rightMargin: applicationWindow.listLayout ? 15 : 0

                                        Item {
                                            id: imageHost
                                            anchors.top: parent.top
                                            anchors.topMargin: descriptionText.height + 24
                                            anchors.horizontalCenter: parent.horizontalCenter
                                            width: 150
                                            property double desiredHeight: descriptionRect.height + keywordsWrapper.height + keywordsLabel.height + 10
                                            height: desiredHeight > 150 ? 150 : desiredHeight

                                            Image {
                                                id: artworkImage
                                                anchors.fill: parent
                                                source: "image://cached/" + filename
                                                sourceSize.width: 150
                                                sourceSize.height: 150
                                                fillMode: settingsModel.fitSmallPreview ? Image.PreserveAspectFit : Image.PreserveAspectCrop
                                                asynchronous: true
                                                // caching is implemented on different level
                                                cache: false
                                            }

                                            Image {
                                                id: imageTypeIcon
                                                visible: hasvectorattached
                                                enabled: hasvectorattached
                                                source: "qrc:/Graphics/vector-icon.svg"
                                                sourceSize.width: 20
                                                sourceSize.height: 20
                                                anchors.left: artworkImage.left
                                                anchors.bottom: artworkImage.bottom
                                                cache: true
                                            }

                                            MouseArea {
                                                anchors.fill: parent
                                                propagateComposedEvents: true
                                                acceptedButtons: Qt.LeftButton | Qt.RightButton

                                                function dblClickHandler() {
                                                    var index = rowWrapper.delegateIndex
                                                    var originalIndex = rowWrapper.getIndex()
                                                    var metadata = filteredArtItemsModel.getArtworkMetadata(index)
                                                    startOneItemEditing(metadata, index, originalIndex)
                                                }

                                                Timer {
                                                    id: dblClickTimer
                                                    interval: 200
                                                    onTriggered: {
                                                        rowWrapper.switchChecked()
                                                        rowWrapper.focusIfNeeded()
                                                    }
                                                }

                                                onClicked: {
                                                    if (mouse.button == Qt.RightButton && openedDialogsCount == 0) {
                                                        console.log("Context menu for artwork")
                                                        artworkContextMenu.filename = filename;
                                                        artworkContextMenu.popup()
                                                    } else {
                                                        if (dblClickTimer.running) {
                                                            dblClickTimer.stop()
                                                            dblClickHandler()
                                                        } else {
                                                            dblClickTimer.restart()
                                                        }
                                                    }
                                                }
                                            }
                                        }

                                        StyledText {
                                            anchors.top: imageHost.bottom
                                            anchors.topMargin: 3
                                            width: parent.width
                                            elide: Text.ElideMiddle
                                            horizontalAlignment: Text.AlignHCenter
                                            text: basefilename
                                            isActive: rowWrapper.isHighlighted
                                        }
                                    }
                                }

                                Item {
                                    id: columnRectangle
                                    visible: applicationWindow.listLayout
                                    anchors.top: applicationWindow.listLayout ? parent.top : undefined
                                    anchors.bottom: applicationWindow.listLayout ? parent.bottom : undefined
                                    anchors.left: applicationWindow.listLayout ? imageColumnWrapper.right : undefined
                                    anchors.leftMargin: applicationWindow.listLayout ? 5 : 0
                                    anchors.right: applicationWindow.listLayout ? parent.right : undefined

                                    Item {
                                        id: columnLayout
                                        anchors.fill: parent
                                        anchors.leftMargin: 0
                                        anchors.rightMargin: 20
                                        anchors.topMargin: 20
                                        property bool isWideEnough: width > 450

                                        StyledText {
                                            id: descriptionText
                                            anchors.left: parent.left
                                            anchors.top: parent.top
                                            text: i18.n + qsTr("Description:")
                                            isActive: rowWrapper.isHighlighted
                                        }

                                        StyledText {
                                            id: titleText
                                            anchors.left: titleRect.left
                                            anchors.top: parent.top
                                            visible: columnLayout.isWideEnough
                                            text: i18.n + qsTr("Title:")
                                            isActive: rowWrapper.isHighlighted
                                        }

                                        Rectangle {
                                            id: descriptionRect
                                            height: 30
                                            anchors.left: parent.left
                                            anchors.right: titleRect.left
                                            anchors.rightMargin: columnLayout.isWideEnough ? 20 : 0
                                            anchors.top: descriptionText.bottom
                                            anchors.topMargin: 3
                                            color: rowWrapper.isHighlighted ? Colors.inputBackgroundColor : Colors.inputInactiveBackground
                                            border.color: Colors.artworkActiveColor
                                            border.width: descriptionTextInput.activeFocus ? 1 : 0
                                            clip: true
                                            focus: false

                                            Flickable {
                                                id: descriptionFlick
                                                contentWidth: descriptionTextInput.paintedWidth
                                                contentHeight: descriptionTextInput.paintedHeight
                                                anchors.left: parent.left
                                                anchors.right: parent.right
                                                anchors.leftMargin: 5
                                                anchors.rightMargin: 5
                                                interactive: false
                                                flickableDirection: Flickable.HorizontalFlick
                                                height: parent.height
                                                anchors.verticalCenter: parent.verticalCenter
                                                clip: true
                                                focus: false

                                                function ensureVisible(r) {
                                                    if (contentX >= r.x)
                                                        contentX = r.x;
                                                    else if (contentX+width <= r.x+r.width)
                                                        contentX = r.x+r.width-width;
                                                }

                                                StyledTextEdit {
                                                    id: descriptionTextInput
                                                    width: paintedWidth > descriptionFlick.width ? paintedWidth : descriptionFlick.width
                                                    height: descriptionFlick.height
                                                    text: description
                                                    focus: true
                                                    isActive: rowWrapper.isHighlighted
                                                    onTextChanged: model.editdescription = text
                                                    userDictEnabled: true

                                                    onActionRightClicked: {
                                                        console.log("Context menu for add word " + rightClickedWord)
                                                        var showAddToDict = filteredArtItemsModel.hasDescriptionWordSpellError(rowWrapper.delegateIndex, rightClickedWord)
                                                        wordRightClickMenu.showAddToDict = showAddToDict
                                                        wordRightClickMenu.word = rightClickedWord
                                                        wordRightClickMenu.showExpandPreset = false
                                                        if (wordRightClickMenu.showAddToDict ||
                                                                wordRightClickMenu.showExpandPreset) {
                                                            wordRightClickMenu.popup()
                                                        }

                                                    }


                                                    Keys.onTabPressed: {
                                                        if (columnLayout.isWideEnough) {
                                                            titleTextInput.forceActiveFocus()
                                                            titleTextInput.cursorPosition = titleTextInput.text.length
                                                        } else {
                                                            flv.activateEdit()
                                                        }
                                                    }

                                                    onActiveFocusChanged: {
                                                        filteredArtItemsModel.spellCheckDescription(rowWrapper.delegateIndex)
                                                        if (activeFocus) {
                                                            wrappersScope.updateCurrentIndex()
                                                        }
                                                    }

                                                    Keys.onBacktabPressed: {
                                                        filteredArtItemsModel.focusPreviousItem(rowWrapper.delegateIndex)
                                                    }

                                                    Keys.onPressed: {
                                                        if(event.matches(StandardKey.Paste)) {
                                                            var clipboardText = clipboard.getText();
                                                            if (Common.safeInsert(descriptionTextInput, clipboardText)) {
                                                                event.accepted = true
                                                            }
                                                        } else if ((event.key === Qt.Key_Return) || (event.key === Qt.Key_Enter)) {
                                                            event.accepted = true
                                                        }
                                                    }

                                                    Component.onCompleted: {
                                                        var index = rowWrapper.getIndex()
                                                        artItemsModel.initDescriptionHighlighting(index, descriptionTextInput.textDocument)
                                                    }

                                                    onCursorRectangleChanged: descriptionFlick.ensureVisible(cursorRectangle)
                                                }
                                            }
                                        }

                                        StyledText {
                                            text: descriptionTextInput.length
                                            anchors.right: descriptionRect.right
                                            anchors.bottom: descriptionRect.top
                                            anchors.bottomMargin: 3
                                            isActive: rowWrapper.isHighlighted
                                        }

                                        Rectangle {
                                            id: titleRect
                                            height: 30
                                            visible: columnLayout.isWideEnough
                                            enabled: columnLayout.isWideEnough
                                            width: columnLayout.isWideEnough ? ((columnLayout.width / 2) - 10) : 0
                                            anchors.right: parent.right
                                            anchors.top: descriptionText.bottom
                                            anchors.topMargin: 3
                                            color: rowWrapper.isHighlighted ? Colors.inputBackgroundColor : Colors.inputInactiveBackground
                                            border.color: Colors.artworkActiveColor
                                            border.width: titleTextInput.activeFocus ? 1 : 0
                                            clip: true
                                            focus: false

                                            Flickable {
                                                id: titleFlick
                                                contentWidth: titleTextInput.paintedWidth
                                                contentHeight: titleTextInput.paintedHeight
                                                height: parent.height
                                                anchors.verticalCenter: parent.verticalCenter
                                                anchors.left: parent.left
                                                anchors.right: parent.right
                                                anchors.leftMargin: 5
                                                anchors.rightMargin: 5
                                                clip: true
                                                flickableDirection: Flickable.HorizontalFlick
                                                interactive: false
                                                focus: false

                                                function ensureVisible(r) {
                                                    if (contentX >= r.x)
                                                        contentX = r.x;
                                                    else if (contentX+width <= r.x+r.width)
                                                        contentX = r.x+r.width-width;
                                                }

                                                StyledTextEdit {
                                                    id: titleTextInput
                                                    width: paintedWidth > titleFlick.width ? paintedWidth : titleFlick.width
                                                    height: titleFlick.height
                                                    text: title
                                                    focus: true
                                                    isActive: rowWrapper.isHighlighted
                                                    onTextChanged: model.edittitle = text
                                                    userDictEnabled: true

                                                    onActionRightClicked: {
                                                        console.log("Context menu for add word " + rightClickedWord)
                                                        var showAddToDict = filteredArtItemsModel.hasTitleWordSpellError(rowWrapper.delegateIndex, rightClickedWord)
                                                        wordRightClickMenu.showAddToDict = showAddToDict
                                                        wordRightClickMenu.word = rightClickedWord
                                                        wordRightClickMenu.showExpandPreset = false
                                                        if (wordRightClickMenu.showAddToDict ||
                                                                wordRightClickMenu.showExpandPreset) {
                                                            wordRightClickMenu.popup()
                                                        }
                                                    }

                                                    Keys.onTabPressed: {
                                                        flv.activateEdit()
                                                    }

                                                    Keys.onBacktabPressed: {
                                                        descriptionTextInput.forceActiveFocus()
                                                        descriptionTextInput.cursorPosition = descriptionTextInput.text.length
                                                    }

                                                    onActiveFocusChanged: {
                                                        filteredArtItemsModel.spellCheckTitle(rowWrapper.delegateIndex)
                                                        if (activeFocus) {
                                                            wrappersScope.updateCurrentIndex()
                                                        }
                                                    }

                                                    Keys.onPressed: {
                                                        if (event.matches(StandardKey.Paste)) {
                                                            var clipboardText = clipboard.getText();
                                                            if (Common.safeInsert(titleTextInput, clipboardText)) {
                                                                event.accepted = true
                                                            }
                                                        } else if ((event.key === Qt.Key_Return) || (event.key === Qt.Key_Enter)) {
                                                            event.accepted = true
                                                        }
                                                    }

                                                    Component.onCompleted: {
                                                        var index = rowWrapper.getIndex()
                                                        artItemsModel.initTitleHighlighting(index, titleTextInput.textDocument)
                                                    }

                                                    onCursorRectangleChanged: titleFlick.ensureVisible(cursorRectangle)
                                                }
                                            }
                                        }

                                        StyledText {
                                            text: titleTextInput.length
                                            anchors.right: titleRect.right
                                            anchors.bottom: titleRect.top
                                            anchors.bottomMargin: 3
                                            isActive: rowWrapper.isHighlighted
                                            visible: columnLayout.isWideEnough
                                        }

                                        StyledText {
                                            id: keywordsLabel
                                            anchors.left: parent.left
                                            anchors.top: descriptionRect.bottom
                                            anchors.topMargin: 7
                                            text: i18.n + qsTr("Keywords:")
                                            isActive: rowWrapper.isHighlighted
                                        }

                                        StyledText {
                                            text: keywordscount
                                            anchors.right: parent.right
                                            anchors.top: descriptionRect.bottom
                                            anchors.topMargin: 7
                                            isActive: rowWrapper.isHighlighted
                                        }

                                        Rectangle {
                                            id: keywordsWrapper
                                            anchors.top: keywordsLabel.bottom
                                            anchors.topMargin: 3
                                            anchors.left: parent.left
                                            anchors.right: parent.right
                                            height: 80*settingsModel.keywordSizeScale
                                            border.color: Colors.artworkActiveColor
                                            border.width: flv.isFocused ? 1 : 0
                                            color: rowWrapper.isHighlighted ? Colors.inputBackgroundColor : Colors.inputInactiveBackground

                                            function removeKeyword(index) {
                                                artItemsModel.removeKeywordAt(rowWrapper.getIndex(), index)
                                            }

                                            function removeLastKeyword() {
                                                artItemsModel.removeLastKeyword(rowWrapper.getIndex())
                                            }

                                            function appendKeyword(keyword) {
                                                artItemsModel.appendKeyword(rowWrapper.getIndex(), keyword)
                                            }

                                            function pasteKeywords(keywords) {
                                                artItemsModel.pasteKeywords(rowWrapper.getIndex(), keywords)
                                            }

                                            EditableTags {
                                                id: flv
                                                model: rowWrapper.keywordsModel
                                                anchors.fill: parent
                                                property int keywordHeight: 20 * settingsModel.keywordSizeScale + (settingsModel.keywordSizeScale - 1)*10
                                                scrollStep: keywordHeight
                                                stealWheel: false
                                                focus: true

                                                delegate: KeywordWrapper {
                                                    id: kw
                                                    isHighlighted: rowWrapper.isHighlighted
                                                    delegateIndex: index
                                                    keywordText: keyword
                                                    itemHeight: flv.keywordHeight
                                                    hasSpellCheckError: !iscorrect
                                                    onRemoveClicked: keywordsWrapper.removeKeyword(kw.delegateIndex)

                                                    onActionDoubleClicked: {
                                                        var callbackObject = {
                                                            onSuccess: function(replacement) {
                                                                artItemsModel.editKeyword(rowWrapper.getIndex(), kw.delegateIndex, replacement)
                                                            },
                                                            onClose: function() {
                                                                flv.activateEdit()
                                                            }
                                                        }

                                                        Common.launchDialog("../Dialogs/EditKeywordDialog.qml",
                                                                            applicationWindow,
                                                                            {
                                                                                callbackObject: callbackObject,
                                                                                previousKeyword: keyword,
                                                                                keywordIndex: kw.delegateIndex,
                                                                                keywordsModel: filteredArtItemsModel.getBasicModel(rowWrapper.delegateIndex)
                                                                            })
                                                    }

                                                    onActionRightClicked: {
                                                        wordRightClickMenu.showAddToDict = !iscorrect
                                                        var keyword = kw.keywordText
                                                        wordRightClickMenu.word = keyword
                                                        filteredPresetsModel.searchTerm = keyword
                                                        wordRightClickMenu.showExpandPreset = (filteredPresetsModel.getItemsCount() !== 0 )
                                                        wordRightClickMenu.artworkIndex =  rowWrapper.delegateIndex
                                                        wordRightClickMenu.keywordIndex = kw.delegateIndex
                                                        if (wordRightClickMenu.showAddToDict ||
                                                                wordRightClickMenu.showExpandPreset) {
                                                            wordRightClickMenu.popup()
                                                        }
                                                    }
                                                }

                                                onTagAdded: {
                                                    keywordsWrapper.appendKeyword(text)
                                                }

                                                onTagsPasted: {
                                                    keywordsWrapper.pasteKeywords(tagsList)
                                                }

                                                onRemoveLast: {
                                                    keywordsWrapper.removeLastKeyword()
                                                }

                                                onBackTabPressed: {
                                                    if (columnLayout.isWideEnough) {
                                                        titleTextInput.forceActiveFocus()
                                                        titleTextInput.cursorPosition = titleTextInput.text.length
                                                    } else {
                                                        descriptionTextInput.forceActiveFocus()
                                                        descriptionTextInput.cursorPosition = descriptionTextInput.text.length
                                                    }
                                                }

                                                onTabPressed: {
                                                    filteredArtItemsModel.focusNextItem(rowWrapper.delegateIndex)
                                                }

                                                onCopyRequest: clipboard.setText(keywordsstring)

                                                onCompletionRequested: {
                                                    helpersWrapper.autoCompleteKeyword(prefix,
                                                                                       rowWrapper.keywordsModel)
                                                }

                                                onEditActivated: {
                                                    wrappersScope.updateCurrentIndex()
                                                    flv.activateEdit()
                                                }

                                                onRightClickedInside: {
                                                    presetsMenu.artworkIndex = rowWrapper.delegateIndex
                                                    presetsMenu.popup()
                                                }
                                            }

                                            CustomScrollbar {
                                                anchors.topMargin: -5
                                                anchors.bottomMargin: -5
                                                anchors.rightMargin: -15
                                                flickable: flv
                                            }
                                        }

                                        RowLayout {
                                            anchors.left: parent.left
                                            anchors.right: parent.right
                                            anchors.rightMargin: 3
                                            anchors.top: keywordsWrapper.bottom
                                            anchors.topMargin: 3
                                            spacing: 5

                                            StyledText {
                                                id: plainTextText
                                                text: i18.n + qsTr("<u>edit in plain text</u>")
                                                color: plainTextMA.containsMouse ? Colors.linkClickedColor : Colors.labelActiveForeground
                                                visible: rowWrapper.isHighlighted

                                                MouseArea {
                                                    id: plainTextMA
                                                    anchors.fill: parent
                                                    hoverEnabled: true
                                                    enabled: rowWrapper.isHighlighted
                                                    preventStealing: true
                                                    cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
                                                    onClicked: {
                                                        // strange bug with clicking on the keywords field
                                                        if (!containsMouse) { return; }

                                                        var callbackObject = {
                                                            onSuccess: function(text) {
                                                                artItemsModel.plainTextEdit(rowWrapper.getIndex(), text)
                                                            },
                                                            onClose: function() {
                                                                flv.activateEdit()
                                                            }
                                                        }

                                                        Common.launchDialog("../Dialogs/PlainTextKeywordsDialog.qml",
                                                                            applicationWindow,
                                                                            {
                                                                                callbackObject: callbackObject,
                                                                                keywordsText: keywordsstring,
                                                                                keywordsModel: filteredArtItemsModel.getBasicModel(rowWrapper.delegateIndex)

                                                                            });
                                                    }
                                                }
                                            }

                                            Item {
                                                Layout.fillWidth: true
                                            }

                                            StyledText {
                                                id: fixSpellingText
                                                text: i18.n + qsTr("Fix spelling")
                                                enabled: rowWrapper.keywordsModel ? rowWrapper.keywordsModel.hasSpellErrors : false
                                                color: enabled ? (fixSpellingMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor)  : (rowWrapper.isHighlighted ? Colors.labelActiveForeground : Colors.labelInactiveForeground)

                                                MouseArea {
                                                    id: fixSpellingMA
                                                    anchors.fill: parent
                                                    cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

                                                    onClicked: {
                                                        artItemsModel.suggestCorrections(rowWrapper.getIndex())
                                                        Common.launchDialog("../Dialogs/SpellCheckSuggestionsDialog.qml",
                                                                            applicationWindow,
                                                                            {})
                                                    }
                                                }
                                            }

                                            StyledText {
                                                text: "|"
                                                isActive: rowWrapper.isHighlighted
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            StyledText {
                                                text: i18.n + qsTr("Suggest")
                                                color: suggestKeywordsMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor

                                                MouseArea {
                                                    id: suggestKeywordsMA
                                                    anchors.fill: parent
                                                    cursorShape: Qt.PointingHandCursor
                                                    onClicked: {
                                                        var callbackObject = {
                                                            promoteKeywords: function(keywords) {
                                                                artItemsModel.addSuggestedKeywords(rowWrapper.getIndex(), keywords)
                                                            }
                                                        }

                                                        Common.launchDialog("../Dialogs/KeywordsSuggestion.qml",
                                                                            applicationWindow,
                                                                            {callbackObject: callbackObject});
                                                    }
                                                }
                                            }

                                            StyledText {
                                                text: "|"
                                                isActive: rowWrapper.isHighlighted
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            StyledText {
                                                text: i18.n + qsTr("Copy")
                                                color: copyKeywordsMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor

                                                MouseArea {
                                                    id: copyKeywordsMA
                                                    anchors.fill: parent
                                                    cursorShape: Qt.PointingHandCursor
                                                    onClicked: clipboard.setText(keywordsstring)
                                                }
                                            }

                                            StyledText {
                                                text: "|"
                                                isActive: rowWrapper.isHighlighted
                                                verticalAlignment: Text.AlignVCenter
                                            }

                                            StyledText {
                                                text: i18.n + qsTr("Clear")
                                                color: clearKeywordsMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor

                                                MouseArea {
                                                    id: clearKeywordsMA
                                                    anchors.fill: parent
                                                    cursorShape: Qt.PointingHandCursor
                                                    onClicked: filteredArtItemsModel.clearKeywords(rowWrapper.delegateIndex)
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    Connections {
                        target: artItemsModel
                        onArtworksChanged: artworksHost.forceUpdateArtworks(needToMoveCurrentItem)
                    }

                    Connections {
                        target: filteredArtItemsModel
                        onAfterInvalidateFilter: {
                            artworksHost.forceUpdateArtworks(true)
                            artworksHost.positionViewAtBeginning()
                        }
                    }
                }

                CustomScrollbar {
                    id: mainScrollBar
                    anchors.topMargin: 0
                    anchors.bottomMargin: 0
                    anchors.rightMargin: -10
                    flickable: artworksHost
                }
            }

            Item {
                visible: artworksHost.count == 0
                anchors.fill: parent

                RowLayout {
                    anchors.centerIn: parent
                    spacing: 5

                    StyledText {
                        text: i18.n + qsTr("No items available.")
                        isActive: false
                    }

                    StyledText {
                        text: i18.n + qsTr("Add files", "link")
                        color: addFilesMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor

                        MouseArea {
                            id: addFilesMA
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: chooseArtworksDialog.open()
                        }
                    }

                    StyledText {
                        text: i18.n + qsTr("or")
                        isActive: false
                    }

                    StyledText {
                        text: i18.n + qsTr("clear the filter")
                        color: clearFilterMA.pressed ? Colors.linkClickedColor : Colors.artworkActiveColor

                        MouseArea {
                            id: clearFilterMA
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                            onClicked: clearFilter()
                        }
                    }
                }
            }
        }
    }

    ClipboardHelper {
        id: clipboard
    }
}


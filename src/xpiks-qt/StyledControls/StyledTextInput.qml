import QtQuick 2.4
import "../Constants"
import "../Constants/Colors.js" as Colors

TextInput {
    font.family: "Helvetica"
    font.pixelSize: 12
    verticalAlignment: TextInput.AlignVCenter
    selectedTextColor: Colors.defaultLightColor
    selectionColor: Colors.defaultControlColor
    renderType: Text.NativeRendering
    selectByMouse: true
    clip: true
    color: Colors.defaultLightColor
    KeyNavigation.priority: KeyNavigation.BeforeItem
}


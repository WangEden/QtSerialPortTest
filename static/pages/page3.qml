import QtQuick 2.15
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.15
import "../components" as Utils
import ManualQWidgets 1.0

Item {
    id: storgeTab
    anchors.fill:parent
    property real progress1: 0.45
    property real progress2: 0.68
    property real progress3: 0.43
    property real progress4: 0.21

    Rectangle {
        width: parent.width
        height: parent.height
        color: "transparent"
        Rectangle {
            id: storgeTab_root
            width: 0.888 * parent.width
            height: 0.533 * width
            border.color: "yellow"
            anchors.centerIn: parent
            color: "transparent"

            QWidgetRenderer {
                width: parent.width
                height: parent.height
                anchors.centerIn: parent

            }
            
        }
    }
}

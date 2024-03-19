import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.impl 2.12
import QtQuick.Templates 2.12 as T
import QtGraphicalEffects 1.15

T.Button {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding)
    padding: 0
    font.pixelSize: 14
    font.weight: Font.Black

    property int radius: 8
    property color textColor: "#FFFFFF"
    property color lightColor: "#FF66B8FF"  // 起始颜色
    property color darkColor: "#FF338BFF"   // 终止颜色

    contentItem: Label {
        font: control.font
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        opacity: control.enabled ? 1 : 0.4
        color: control.textColor
        text: control.text
    }

    background: Rectangle {
        id: backgroundRect
        implicitWidth: 160
        implicitHeight: 44

        property var targetGradient: gradient

        radius: control.radius
        opacity: control.enabled ? 1 : 0.4
        LinearGradient {
            id: gradient1
            anchors.fill: parent
            source: parent
            start: Qt.point(0.6 * parent.width, 0.6 * parent.height)
            end: Qt.point(0, parent.height)
            gradient: Gradient {
                GradientStop {
                    position: 0;
                    color: Qt.rgba(85 / 256, 62 / 256, 231 / 256, 1)
                }
                GradientStop {
                    position: 1;
                    color: Qt.rgba(187 / 256, 52 / 256, 103 / 256, 1)
                }
            }
        }
        layer.enabled: true
        layer.effect: LinearGradient {
            start: Qt.point(0, 0)
            end: Qt.point(width, 0)  //横向渐变按钮
//            end: Qt.point(width, height)  //斜向渐变按钮
            gradient: Gradient {
                GradientStop {
                    position: 0.0
                    color: control.pressed ? Qt.rgba(187 / 256, 52 / 256, 103 / 256, 1) : Qt.rgba(85 / 256, 62 / 256, 231 / 256, 1)
                }
                GradientStop {
                    position: 1.0
                    color: control.pressed ? Qt.rgba(85 / 256, 62 / 256, 231 / 256, 1) : Qt.rgba(187 / 256, 52 / 256, 103 / 256, 1)
                }
            }
        }

        // ColorAnimation {
        //     id: backgroundRect
        //     target: rect
        //     property: "color"
        //     duration: 500
        //     running: false
        //     onRunningChanged: {
        //         if (!running) {
        //             rect.color = gradientAnimation.to.stops[0].color; // 更新颜色为目标渐变色起始颜色
        //         }
        //     }
        //     values: [gradient.stops[0].color, gradient2.stops[1].color]
        // }


        // NumberAnimation {
        //     id:animationPressed1
        //     target:backgroundRect
        //     properties: "color"
        //     from:1
        //     to:0.2
        //     duration: 150
        // }
        // NumberAnimation {
        //     id:animationReleased1
        //     target:backgroundRect
        //     properties: "color"
        //     from:0.2
        //     to:1
        //     duration: 150
        // }
    }
}


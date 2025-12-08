import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Dialogs
import org.kde.kirigami as Kirigami

import com.gstark

Kirigami.Page {
    id: diffPage
    title: "MergeConflict"

    width: parent.width
    height: parent.height

    DiffModel {
        id: diffModel
    }

    function createDiff(common, local, remote) {
        diffModel.createDiff(common, local, remote);
    }

    footer: ColumnLayout {
        width: parent.width
        Kirigami.NavigationTabBar {
            implicitWidth: parent.width
            actions: [
                Kirigami.Action {
                    text: "Keep Remote"
                    icon.name: "cloud-download"
                },
                Kirigami.Action {
                    text: "Keep Local"
                    icon.name: "cloud-upload"
                }
            ]
        }
    }

    ListView {
        id: mergeListView

        model: diffModel
        anchors.fill: parent
        delegate: Row {
            required property bool conflicting
            required property string display
            required property string leftContent
            required property string rightContent
            Text {
                text: parent.display
                visible: !parent.conflicting
            }
            Button {
                text: "use left"
                visible: parent.conflicting
            }
            Text {
                text: parent.leftContent
                visible: parent.conflicting
            }
            Button {
                text: "use right"
                visible: parent.conflicting
            }
            Text {
                text: parent.rightContent
                visible: parent.conflicting
            }
        }
    }
}

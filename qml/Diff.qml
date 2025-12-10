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

    signal diffResolved(data: string)

    DiffModel {
        id: diffModel
        onResolved: saveAction.enabled = true
    }

    function createDiff(common, local, remote) {
        diffModel.createDiff(common, local, remote);
        saveAction.enabled = false;
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
                },
                Kirigami.Action {
                    id: saveAction
                    text: "Save"
                    icon.name: "save"
                    enabled: false
                    onTriggered: diffPage.diffResolved(diffModel.getData())
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
            required property int index
            TextArea {
                text: parent.display
                width: mergeListView.width
                visible: !parent.conflicting
            }
            Button {
                text: "use left"
                visible: parent.conflicting
                onClicked: diffModel.mergeLeft(parent.index)
            }
            Text {
                text: parent.leftContent
                visible: parent.conflicting
            }
            Button {
                text: "use right"
                visible: parent.conflicting
                onClicked: diffModel.mergeRight(parent.index)
            }
            Text {
                text: parent.rightContent
                visible: parent.conflicting
            }
        }
    }
}

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

    property string path

    signal diffResolved(path: string, data: string)

    DiffModel {
        id: diffModel
        onConflictingChanged: {
            saveAction.enabled = !diffModel.conflicting;
        }
    }

    function createDiff(path, common, local, remote) {
        diffPage.path = path;
        diffModel.createDiff(common, local, remote);
        saveAction.enabled = !diffModel.conflicting;
    }

    footer: ColumnLayout {
        width: parent.width
        Kirigami.NavigationTabBar {
            implicitWidth: parent.width
            actions: [
                Kirigami.Action {
                    text: "Keep Remote"
                    icon.name: "cloud-download"
                    onTriggered: diffModel.useRemote()
                },
                Kirigami.Action {
                    text: "Keep Local"
                    icon.name: "cloud-upload"
                    onTriggered: diffModel.useLocal()
                },
                Kirigami.Action {
                    id: saveAction
                    text: "Save"
                    icon.name: "save"
                    enabled: !diffModel.conflicting
                    onTriggered: diffPage.diffResolved(diffPage.path, diffModel.getData())
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
                onTextChanged: diffModel.update(parent.index, text)
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

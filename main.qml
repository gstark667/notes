import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Dialogs
import org.kde.kirigami 2.20 as Kirigami

import com.gstark 1.0

Kirigami.ApplicationWindow {
    id: root
    width: 800
    height: 600
    visible: true
    title: "Notes"

    Settings {
        id: settings
        property string notesDir
    }

    FileModel {
        id: fileModel
    }

    TreeModel {
        id: treeModel
        path: settings.notesDir
    }

    FileDialog {
        id: testDialog
        title: "Select a File"
        onAccepted: {
            console.log("Selected file:", fileDialog.fileUrl)
        }
        onRejected: {
            console.log("File dialog rejected")
        }
    }


    FolderDialog {
        id: fileDialog
        currentFolder: settings.notesDir
        onAccepted: settings.notesDir = selectedFolder
    }


    Kirigami.PromptDialog {
        id: newFileDialog
        title: "New File"

        property string path: ""

        standardButtons: Kirigami.Dialog.NoButton
        customFooterActions: [
            Kirigami.Action {
                text: "Create"
                icon.name: "dialog-ok"
                onTriggered: {
                    fileModel.createFile(newFileDialog.path, newFileName.text)
                    mainText.text = ""
                    treeModel.path = settings.notesDir
                    fileTree.expand(0)
                    newFileDialog.close();
                }
            },
            Kirigami.Action {
                text: "Cancel"
                icon.name: "dialog-cancel"
                onTriggered: {
                    newFileDialog.close();
                }
            }
        ]
        ColumnLayout {
            TextField {
                id: newFileName
                Layout.fillWidth: true
                placeholderText: "File name…"
            }
        }
    }


    Menu {
        id: contextMenu
        property string path: ""
        property bool   isDirectory: false
        property int    index: -1

        MenuItem {
            text: "New File"
            enabled: contextMenu.isDirectory
            onTriggered: {
                newFileDialog.path = contextMenu.path
                newFileDialog.open()
            }
        }
        MenuItem {
            text: "New Folder"
            enabled: contextMenu.isDirectory
        }
        MenuItem {
            text: "Delete"
            onTriggered: {
                fileModel.remove(contextMenu.path)
                treeModel.path = settings.notesDir
                fileTree.positionViewAtRow(0, TableView.Visible)
            }
            enabled: contextMenu.index > 0
        }
    }


    Kirigami.GlobalDrawer {
        id: sidebar
        title: "Menu"
        modal: Kirigami.Settings.isMobile
        collapsible: !Kirigami.Settings.isMobile
        collapsed: false

        actions: [
            Kirigami.Action {
                text: "Settings"
                icon.name: "settings-configure"
                onTriggered: fileDialog.open()
            },
            Kirigami.Action {
                text: "Journal"
                icon.name: "journal-new"
            },
            Kirigami.Action {
                text: "Sync"
                icon.name: "folder-sync"
            },
            Kirigami.Action {
                text: "Collapse"
                icon.name: "sidebar-collapse"
                onTriggered: sidebar.collapsed = !sidebar.collapsed
            }
        ]

        ColumnLayout {
            id: treeScrollView
            clip: true
            height: 400
            width: parent.width
            TreeView {
                id: fileTree
                Layout.fillWidth: true
                Layout.fillHeight: true 
                model: treeModel
                delegate: TreeViewDelegate {
                    text: model.display
                    implicitWidth: treeScrollView.width

                    TapHandler {
                        acceptedButtons: Qt.RightButton

                        onTapped: {
                            contextMenu.path = model.path
                            contextMenu.isDirectory = model.isDirectory
                            contextMenu.index = index
                            contextMenu.popup()
                        }
                    }

                    TapHandler {
                        onTapped: {
                            console.log(model.path)
                            mainText.text = fileModel.open(model.path)
                        }
                        onLongPressed: {
                            contextMenu.path = model.path
                            contextMenu.isDirectory = model.isDirectory
                            contextMenu.index = index
                            contextMenu.popup()
                        }
                    }
                }
            }
        }
    }

    Kirigami.Page {
        title: "Main Page"
        anchors.fill: parent
        anchors.leftMargin: Kirigami.Settings.isMobile ? 0 : sidebar.width

        ScrollView {
            anchors.fill: parent

            TextArea {
                id: mainText
                wrapMode: TextArea.Wrap
                placeholderText: "Write something here..."
                //textFormat: TextEdit.MarkdownText

                Shortcut {
                    sequences: [StandardKey.Save]
                    onActivated: {
                        if (fileModel.save(mainText.text)) {
                            showPassiveNotification("Saved");
                        } else {
                            showPassiveNotification("Failed to Save");
                        }
                    }
                }
            }
        }
    }
}

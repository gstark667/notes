import QtCore
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15
import QtQuick.Window
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

    Kirigami.PromptDialog {
        id: textPromptDialog
        title: "Select Folder"

        standardButtons: Kirigami.Dialog.NoButton
        customFooterActions: [
            Kirigami.Action {
                text: "Select Folder"
                icon.name: "dialog-ok"
                onTriggered: {
                    showPassiveNotification("Open");
                    settings.notesDir = notesDirField.text
                    textPromptDialog.close();
                }
            },
            Kirigami.Action {
                text: "Cancel"
                icon.name: "dialog-cancel"
                onTriggered: {
                    textPromptDialog.close();
                }
            }
        ]
        ColumnLayout {
            TextField {
                id: notesDirField
                Layout.fillWidth: true
                placeholderText: "Folder name…"
                text: settings.notesDir
            }
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
                onTriggered: textPromptDialog.open()
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
                        onTapped: {
                            console.log(model.path)
                            mainText.text = fileModel.open(model.path)
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

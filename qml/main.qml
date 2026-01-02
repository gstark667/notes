pragma ComponentBehavior: Bound

import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Dialogs
import org.kde.kirigami as Kirigami

import com.gstark

Kirigami.ApplicationWindow {
    id: root
    title: "Notes"
    flags: Qt.Window

    Settings {
        id: settings
        property string notesDir
        property string webdavUrl
        property string webdavUsername
        property string webdavPassword
    }

    TreeModel {
        id: treeModel
        path: settings.notesDir
        onPathChanged: function (path) {
            syncer.path = path;
        }
    }

    Highlighter {
        id: syntaxHighlighter
        textDocument: mainText.textDocument
    }

    Syncer {
        id: syncer
        onFileCreated: function (path) {
            treeModel.createFile(path);
        }
        onFileUpdated: function (path) {
            mainText.checkReload(path);
        }
        onMergeConflict: function (path) {
            treeModel.mergeConflict(path, true);
        }
        onConflictAvailable: function (path, common, local, remote) {
            root.pageStack.push(diffLoader.item);
            diffLoader.item.createDiff(path, common, local, remote);
        }
    }

    Loader {
        id: diffLoader
        sourceComponent: Diff {
            id: diffPage
            onBackRequested: {
                root.pageStack.removePage(diffLoader.item);
            }
        }
    }
    Connections {
        target: diffLoader.item
        onDiffResolved: function (path, data) {
            syncer.resolveConflict(path, data);
            treeModel.mergeConflict(path, false);
            root.pageStack.removePage(diffLoader.item);
        }
    }

    FolderDialog {
        id: fileDialog
        currentFolder: settings.notesDir
        onAccepted: settings.notesDir = selectedFolder
    }

    Kirigami.PromptDialog {
        id: newFileDialog
        title: isDirectory ? "New Folder" : "New File"

        property string path: ""
        property string newPath: ""
        property bool isDirectory: false
        property int index: -1

        standardButtons: Kirigami.Dialog.NoButton
        customFooterActions: [
            Kirigami.Action {
                id: newFileDialogCreate
                text: "Create"
                icon.name: "dialog-ok"
                onTriggered: {
                    treeModel.create(newFileDialog.path, newFileName.text, newFileDialog.isDirectory);
                    fileTree.expand(newFileDialog.index);
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
                placeholderText: newFileDialog.isDirectory ? "Folder name..." : "File name..."
                focus: true
                Keys.onReturnPressed: {
                    newFileDialogCreate.trigger();
                }
            }
        }

        onVisibleChanged: {
            if (visible) {
                newFileName.forceActiveFocus();
            } else {
                newFileName.text = "";
            }
        }
    }

    SettingsMenu {
        id: settingsMenu

        onWebdavSettings: function (url, username, password) {
            settings.webdavUrl = url;
            settings.webdavUsername = username;
            settings.webdavPassword = password;
            syncer.open(url, username, password);
        }
    }

    Kirigami.MenuDialog {
        id: contextMenu
        property string path: ""
        property bool isDirectory: false
        property int index: -1

        title: "File Actions"

        function doOpen(path, isDirectory, index) {
            this.path = path;
            this.isDirectory = isDirectory;
            this.index = index;
            this.open();
        }

        actions: [
            Kirigami.Action {
                text: "New File"
                enabled: contextMenu.isDirectory
                onTriggered: {
                    newFileDialog.path = contextMenu.path;
                    newFileDialog.isDirectory = false;
                    newFileDialog.index = contextMenu.index;
                    newFileDialog.open();
                }
            },
            Kirigami.Action {
                text: "New Folder"
                enabled: contextMenu.isDirectory
                onTriggered: {
                    newFileDialog.path = contextMenu.path;
                    newFileDialog.isDirectory = true;
                    newFileDialog.index = contextMenu.index;
                    newFileDialog.open();
                }
            },
            Kirigami.Action {
                text: "Delete"
                onTriggered: {
                    treeModel.remove(contextMenu.path);
                }
                enabled: contextMenu.index > 0
            }
        ]
    }

    globalDrawer: Kirigami.GlobalDrawer {
        id: sidebar
        title: "Menu"
        modal: Kirigami.Settings.isMobile
        collapsible: !Kirigami.Settings.isMobile
        collapsed: Kirigami.Settings.isMobile

        actions: [
            Kirigami.Action {
                text: "Settings"
                icon.name: "settings-configure"
                onTriggered: settingsMenu.doOpen(settings.webdavUrl, settings.webdavUsername, settings.webdavPassword)
            },
            Kirigami.Action {
                text: "Journal"
                icon.name: "journal-new"
            },
            Kirigami.Action {
                text: "Sync"
                icon.name: "folder-sync"
                onTriggered: syncer.sync()
            },
            Kirigami.Action {
                text: "Collapse"
                icon.name: "sidebar-collapse"
                onTriggered: sidebar.collapsed = !sidebar.collapsed
            }
        ]

        onDrawerOpenChanged: {
            if (drawerOpen) {
                mainText.focus = false;
            }
        }

        ColumnLayout {
            id: treeScrollView
            clip: true
            height: 400
            width: parent.width
            TreeView {
                id: fileTree
                Layout.fillWidth: true
                Layout.fillHeight: true
                alternatingRows: false
                model: treeModel
                selectionModel: ItemSelectionModel {}

                delegate: TreeViewDelegate {
                    text: model.display
                    implicitWidth: treeScrollView.width
                    background: Rectangle {
                        color: model.isConflict ? "red" : "transparent"
                    }

                    TapHandler {
                        acceptedButtons: Qt.RightButton
                        enabled: !Kirigami.Settings.isMobile
                        onTapped: contextMenu.doOpen(model.path, model.isDirectory, model.index)
                    }

                    TapHandler {
                        onTapped: {
                            if (!model.isDirectory) {
                                if (model.isConflict) {
                                    syncer.openMergeConflict(model.path);
                                } else {
                                    mainText.open(model.path);
                                }
                            }
                            fileTree.selectionModel.setCurrentIndex(fileTree.index(model.index, 0), ItemSelectionModel.NoUpdate);
                        }
                        onLongPressed: contextMenu.doOpen(model.path, model.isDirectory, model.index)
                    }
                }
            }
        }
    }

    Kirigami.Page {
        id: editorPage
        title: "Main Page"

        width: parent.width
        height: parent.height

        footer: ColumnLayout {
            width: parent.width
            Kirigami.NavigationTabBar {
                implicitWidth: parent.width
                actions: [
                    Kirigami.Action {
                        id: saveAction
                        text: "Save"
                        icon.name: "save"
                        onTriggered: mainText.save()
                    },
                    Kirigami.Action {
                        text: "Reload"
                        icon.name: "reload"
                        onTriggered: mainText.reload()
                    }
                ]
            }
        }

        ScrollView {
            anchors.fill: parent
            focusPolicy: Qt.NoFocus

            TextArea {
                id: mainText
                placeholderText: "Write something here..."
                font.pixelSize: 12
                wrapMode: TextEdit.Wrap
                focusPolicy: Qt.ClickFocus
                onPressed: forceActiveFocus()
                inputMethodHints: Qt.ImhMultiLine | Qt.ImhNoPredictiveText

                property string path: ""

                function open(newPath) {
                    path = newPath;
                    text = treeModel.open(newPath);
                    editorPage.title = treeModel.getName(newPath);

                    if (Kirigami.Settings.isMobile) {
                        sidebar.drawerOpen = false;
                    }
                }

                function save() {
                    if (treeModel.save(mainText.path, mainText.text)) {
                        root.showPassiveNotification("Saved");
                        //syncer.putFile(mainText.path); TODO: check for remote changes
                    } else {
                        root.showPassiveNotification("Failed to Save");
                    }
                }

                function reload() {
                    open(path);
                }

                function checkReload(aPath) {
                    // TODO: check for edits before put
                    if (aPath == path) {
                        open(aPath);
                    }
                }

                Shortcut {
                    sequences: [StandardKey.Save]
                    onActivated: mainText.save()
                }
            }
        }
    }

    pageStack.initialPage: editorPage

    Component.onCompleted: {
        syncer.open(settings.webdavUrl, settings.webdavUsername, settings.webdavPassword);
        syncer.sync();
    }
}

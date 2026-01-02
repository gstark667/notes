import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import QtQuick.Dialogs
import org.kde.kirigami as Kirigami

import com.gstark

Kirigami.PromptDialog {
    id: settingsMenu
    title: "Settings"

    signal webdavSettings(url: string, username: string, password: string)

    function doOpen(url, username, password) {
        webdavUrlField.text = url;
        webdavUsernameField.text = username;
        webdavPasswordField.text = password;
        settingsMenu.open();
    }

    standardButtons: Kirigami.Dialog.NoButton
    customFooterActions: [
        Kirigami.Action {
            text: "Accept"
            icon.name: "dialog-ok"
            onTriggered: {
                settingsMenu.webdavSettings(webdavUrlField.text, webdavUsernameField.text, webdavPasswordField.text);
                settingsMenu.close();
            }
        },
        Kirigami.Action {
            text: "Cancel"
            icon.name: "dialog-cancel"
            onTriggered: {
                settingsMenu.close();
            }
        }
    ]

    ColumnLayout {
        // WebDAV
        Kirigami.FormLayout {
            Layout.fillWidth: true
            Kirigami.Separator {
                Kirigami.FormData.isSection: true
                Kirigami.FormData.label: "WebDAV Settings"
            }
            TextField {
                id: webdavUrlField
                Kirigami.FormData.label: "URL:"
                text: settings.webdavUrl
                placeholderText: "https://example.com/webdav"
            }
            TextField {
                id: webdavUsernameField
                Kirigami.FormData.label: "Username:"
                text: settings.webdavUsername
                placeholderText: "username"
            }
            TextField {
                id: webdavPasswordField
                Kirigami.FormData.label: "Password:"
                text: settings.webdavPassword
                placeholderText: "password"
                echoMode: TextInput.Password
            }
        }

        // Notes directory
        Kirigami.FormLayout {
            Layout.fillWidth: true
            Kirigami.Separator {
                Kirigami.FormData.isSection: true
                Kirigami.FormData.label: "Notes Directory"
            }
            Button {
                text: "Select Directory"
                icon.name: "folder-open"
                onClicked: fileDialog.open()
            }
        }
    }
}
